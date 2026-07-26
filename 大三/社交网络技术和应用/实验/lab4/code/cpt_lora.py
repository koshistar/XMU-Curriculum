import argparse
import json
import logging
import math
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, List, Optional, Set

import torch
from PIL import Image, ImageFile
from transformers import (
    AutoModelForImageTextToText,
    AutoProcessor,
    Trainer,
    TrainingArguments,
    set_seed,
)

from peft import LoraConfig, TaskType, get_peft_model

ImageFile.LOAD_TRUNCATED_IMAGES = True

LOGGER = logging.getLogger("smolvlm_cpt_lora")


def setup_logging():
    logging.basicConfig(
        format="%(asctime)s | %(levelname)s | %(name)s | %(message)s",
        level=logging.INFO,
    )


def parse_args():
    parser = argparse.ArgumentParser(description="LoRA continued pretraining for SmolVLM on jpg+text data.")
    parser.add_argument("--model_name", type=str, default="HuggingFaceTB/SmolVLM-500M-Base")
    parser.add_argument("--train_file", type=str, required=True)
    parser.add_argument("--valid_file", type=str, default=None)
    parser.add_argument("--output_dir", type=str, required=True)

    parser.add_argument("--max_length", type=int, default=256)
    parser.add_argument("--per_device_train_batch_size", type=int, default=1)
    parser.add_argument("--per_device_eval_batch_size", type=int, default=1)
    parser.add_argument("--gradient_accumulation_steps", type=int, default=4)
    parser.add_argument("--learning_rate", type=float, default=2e-4)
    parser.add_argument("--weight_decay", type=float, default=0.0)
    parser.add_argument("--num_train_epochs", type=float, default=1.0)
    parser.add_argument("--warmup_ratio", type=float, default=0.03)
    parser.add_argument("--logging_steps", type=int, default=10)
    parser.add_argument("--save_steps", type=int, default=200)
    parser.add_argument("--eval_steps", type=int, default=200)
    parser.add_argument("--save_total_limit", type=int, default=2)
    parser.add_argument("--dataloader_num_workers", type=int, default=0)
    parser.add_argument("--seed", type=int, default=42)

    parser.add_argument("--bf16", action="store_true")
    parser.add_argument("--fp16", action="store_true")
    parser.add_argument("--gradient_checkpointing", action="store_true")

    parser.add_argument("--image_longest_edge", type=int, default=512)
    parser.add_argument("--max_train_samples", type=int, default=None)
    parser.add_argument("--max_eval_samples", type=int, default=None)

    parser.add_argument("--lora_r", type=int, default=16)
    parser.add_argument("--lora_alpha", type=int, default=32)
    parser.add_argument("--lora_dropout", type=float, default=0.05)

    return parser.parse_args()


def load_jsonl(path: str) -> List[Dict[str, str]]:
    records = []
    with open(path, "r", encoding="utf-8") as f:
        for lineno, line in enumerate(f, start=1):
            line = line.strip()
            if not line:
                continue
            try:
                obj = json.loads(line)
            except json.JSONDecodeError as e:
                raise ValueError(f"{path} line {lineno} is not valid JSON: {e}") from e

            if "image" not in obj or "text" not in obj:
                raise ValueError(f"{path} line {lineno} must contain keys: image, text")

            image_path = str(obj["image"]).strip()
            text = str(obj["text"]).strip()

            if not image_path:
                raise ValueError(f"{path} line {lineno}: image path is empty")
            if not text:
                raise ValueError(f"{path} line {lineno}: text is empty")

            records.append({"image": image_path, "text": text})

    if not records:
        raise ValueError(f"No valid samples found in {path}")
    return records


def verify_and_filter_records(records: List[Dict[str, str]], limit: Optional[int] = None) -> List[Dict[str, str]]:
    filtered = []
    bad = 0

    for ex in records:
        path = Path(ex["image"])
        if not path.exists():
            bad += 1
            LOGGER.warning("Skip missing image: %s", path)
            continue

        try:
            with Image.open(path) as img:
                img.verify()
        except Exception as e:
            bad += 1
            LOGGER.warning("Skip unreadable image %s: %s", path, e)
            continue

        filtered.append(ex)
        if limit is not None and len(filtered) >= limit:
            break

    if not filtered:
        raise ValueError("All samples were filtered out. Check your image paths and files.")

    LOGGER.info("Kept %d samples, filtered %d bad samples.", len(filtered), bad)
    return filtered


def open_rgb_image(path: str) -> Image.Image:
    with Image.open(path) as img:
        return img.convert("RGB")


class JsonlImageTextDataset(torch.utils.data.Dataset):
    def __init__(self, records: List[Dict[str, str]]):
        self.records = records

    def __len__(self):
        return len(self.records)

    def __getitem__(self, idx: int) -> Dict[str, str]:
        return self.records[idx]


@dataclass
class SmolVLMCollator:
    processor: Any
    max_length: int

    def __call__(self, features: List[Dict[str, str]]) -> Dict[str, torch.Tensor]:
        texts: List[str] = []
        images: List[List[Image.Image]] = []

        for ex in features:
            image = open_rgb_image(ex["image"])
            target_text = ex["text"]

            messages = [
                {
                    "role": "user",
                    "content": [
                        {"type": "image"},
                        {"type": "text", "text": "Describe the image."},
                    ],
                },
                {
                    "role": "assistant",
                    "content": [
                        {"type": "text", "text": target_text},
                    ],
                },
            ]

            prompt = self.processor.apply_chat_template(
                messages,
                add_generation_prompt=False,
                tokenize=False,
            )
            texts.append(prompt)
            images.append([image])

        batch = self.processor(
            text=texts,
            images=images,
            return_tensors="pt",
            padding=True,
            truncation=True,
            max_length=self.max_length,
        )

        if "input_ids" not in batch:
            raise RuntimeError("Processor output does not contain input_ids. Check transformers version.")

        labels = batch["input_ids"].clone()

        pad_token_id = self.processor.tokenizer.pad_token_id
        if pad_token_id is not None:
            labels[labels == pad_token_id] = -100

        special_ids = set(self.processor.tokenizer.all_special_ids)
        for token_id in special_ids:
            labels[labels == token_id] = -100

        batch["labels"] = labels
        return batch


def maybe_patch_image_processor(processor: Any, longest_edge: Optional[int]):
    image_processor = getattr(processor, "image_processor", None)
    if image_processor is None:
        LOGGER.warning("Processor has no image_processor attribute; skip resizing patch.")
        return processor

    if hasattr(image_processor, "do_resize"):
        image_processor.do_resize = True

    if longest_edge is not None:
        if hasattr(image_processor, "size"):
            image_processor.size = {"longest_edge": longest_edge}

        if hasattr(image_processor, "max_image_size"):
            image_processor.max_image_size = {"longest_edge": longest_edge}

        if hasattr(image_processor, "resolution_max_side"):
            image_processor.resolution_max_side = longest_edge

        LOGGER.info(
            "Patched image processor with longest_edge=%s, size=%s, max_image_size=%s, resolution_max_side=%s",
            longest_edge,
            getattr(image_processor, "size", None),
            getattr(image_processor, "max_image_size", None),
            getattr(image_processor, "resolution_max_side", None),
        )
        return processor

    max_image_size = getattr(image_processor, "max_image_size", None)
    resolution_max_side = getattr(image_processor, "resolution_max_side", None)

    max_edge = None
    if isinstance(max_image_size, dict):
        max_edge = max_image_size.get("longest_edge", None)
    elif isinstance(max_image_size, int):
        max_edge = max_image_size

    if (
        max_edge is not None
        and resolution_max_side is not None
        and resolution_max_side > max_edge
        and hasattr(image_processor, "resolution_max_side")
    ):
        image_processor.resolution_max_side = max_edge
        LOGGER.warning(
            "Patched image processor: resolution_max_side (%s) > max_image_size (%s), set resolution_max_side = %s",
            resolution_max_side,
            max_edge,
            max_edge,
        )

    return processor


def compute_metrics(eval_pred):
    return {}


def find_lora_target_modules(model: torch.nn.Module) -> List[str]:
    """
    自动寻找常见的线性投影层名称后缀，避免手写模块名时不兼容。
    """
    candidate_suffixes = {
        "q_proj",
        "k_proj",
        "v_proj",
        "o_proj",
        "gate_proj",
        "up_proj",
        "down_proj",
    }
    found: Set[str] = set()

    for name, module in model.named_modules():
        if isinstance(module, torch.nn.Linear):
            suffix = name.split(".")[-1]
            if suffix in candidate_suffixes:
                found.add(suffix)

    targets = sorted(found)
    if not targets:
        raise RuntimeError(
            "No LoRA target modules were found automatically. "
            "Please inspect model.named_modules() and set target modules manually."
        )

    LOGGER.info("LoRA target modules: %s", targets)
    return targets


def main():
    setup_logging()
    args = parse_args()

    if args.bf16 and args.fp16:
        raise ValueError("Choose only one of --bf16 or --fp16")

    set_seed(args.seed)

    LOGGER.info("Loading processor and model from %s", args.model_name)
    processor = AutoProcessor.from_pretrained(args.model_name)
    processor = maybe_patch_image_processor(processor, args.image_longest_edge)

    dtype = None
    if args.bf16:
        dtype = torch.bfloat16
    elif args.fp16:
        dtype = torch.float16

    model = AutoModelForImageTextToText.from_pretrained(
        args.model_name,
        torch_dtype=dtype,
    )

    if args.gradient_checkpointing:
        model.gradient_checkpointing_enable()
        if hasattr(model.config, "use_cache"):
            model.config.use_cache = False

    target_modules = find_lora_target_modules(model)

    lora_config = LoraConfig(
        r=args.lora_r,
        lora_alpha=args.lora_alpha,
        lora_dropout=args.lora_dropout,
        bias="none",
        task_type=TaskType.CAUSAL_LM,
        target_modules=target_modules,
    )

    model = get_peft_model(model, lora_config)
    model.print_trainable_parameters()

    train_records = load_jsonl(args.train_file)
    train_records = verify_and_filter_records(train_records, args.max_train_samples)
    train_dataset = JsonlImageTextDataset(train_records)

    eval_dataset = None
    if args.valid_file:
        valid_records = load_jsonl(args.valid_file)
        valid_records = verify_and_filter_records(valid_records, args.max_eval_samples)
        eval_dataset = JsonlImageTextDataset(valid_records)

    collator = SmolVLMCollator(processor=processor, max_length=args.max_length)

    training_args = TrainingArguments(
        output_dir=args.output_dir,
        per_device_train_batch_size=args.per_device_train_batch_size,
        per_device_eval_batch_size=args.per_device_eval_batch_size,
        gradient_accumulation_steps=args.gradient_accumulation_steps,
        learning_rate=args.learning_rate,
        weight_decay=args.weight_decay,
        num_train_epochs=args.num_train_epochs,
        warmup_ratio=args.warmup_ratio,
        logging_steps=args.logging_steps,
        save_steps=args.save_steps,
        eval_steps=args.eval_steps if eval_dataset is not None else None,
        evaluation_strategy="steps" if eval_dataset is not None else "no",
        save_strategy="steps",
        save_total_limit=args.save_total_limit,
        dataloader_num_workers=args.dataloader_num_workers,
        bf16=args.bf16,
        fp16=args.fp16,
        remove_unused_columns=False,
        report_to="none",
        gradient_checkpointing=args.gradient_checkpointing,
        label_names=["labels"],
    )

    trainer = Trainer(
        model=model,
        args=training_args,
        train_dataset=train_dataset,
        eval_dataset=eval_dataset,
        data_collator=collator,
        compute_metrics=compute_metrics if eval_dataset is not None else None,
    )

    LOGGER.info("Start LoRA training")
    train_result = trainer.train()

    trainer.save_model(args.output_dir)
    processor.save_pretrained(args.output_dir)

    metrics = train_result.metrics
    metrics["train_samples"] = len(train_dataset)
    trainer.log_metrics("train", metrics)
    trainer.save_metrics("train", metrics)
    trainer.save_state()

    if eval_dataset is not None:
        LOGGER.info("Running evaluation")
        metrics = trainer.evaluate()
        if "eval_loss" in metrics:
            try:
                metrics["eval_perplexity"] = math.exp(metrics["eval_loss"])
            except OverflowError:
                metrics["eval_perplexity"] = float("inf")
        metrics["eval_samples"] = len(eval_dataset)
        trainer.log_metrics("eval", metrics)
        trainer.save_metrics("eval", metrics)

    LOGGER.info("LoRA training finished. Adapter saved to %s", args.output_dir)


if __name__ == "__main__":
    main()