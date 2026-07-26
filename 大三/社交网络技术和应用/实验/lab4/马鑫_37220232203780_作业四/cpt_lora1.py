import argparse
import json
import logging
import math
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, List, Optional, Set

import torch
from transformers import (
    AutoModelForCausalLM,
    AutoTokenizer,
    Trainer,
    TrainingArguments,
    set_seed,
)

from peft import LoraConfig, TaskType, get_peft_model

LOGGER = logging.getLogger("smolvlm_cpt_lora")


def setup_logging():
    logging.basicConfig(
        format="%(asctime)s | %(levelname)s | %(name)s | %(message)s",
        level=logging.INFO,
    )


def parse_args():
    parser = argparse.ArgumentParser(description="LoRA continued pretraining for SmolVLM on text data.")
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

            if "text" not in obj:
                raise ValueError(f"{path} line {lineno} must contain key: text")

            text = str(obj["text"]).strip()

            if not text:
                raise ValueError(f"{path} line {lineno}: text is empty")

            records.append({"text": text})

    if not records:
        raise ValueError(f"No valid samples found in {path}")
    return records


def verify_and_filter_records(records: List[Dict[str, str]], limit: Optional[int] = None) -> List[Dict[str, str]]:
    filtered = []
    bad = 0

    for ex in records:
        text = ex["text"]
        if not text or len(text.strip()) == 0:
            bad += 1
            LOGGER.warning("Skip empty text")
            continue

        filtered.append(ex)
        if limit is not None and len(filtered) >= limit:
            break

    if not filtered:
        raise ValueError("All samples were filtered out. Check your text data.")

    LOGGER.info("Kept %d samples, filtered %d bad samples.", len(filtered), bad)
    return filtered


class JsonlTextDataset(torch.utils.data.Dataset):
    def __init__(self, records: List[Dict[str, str]]):
        self.records = records

    def __len__(self):
        return len(self.records)

    def __getitem__(self, idx: int) -> Dict[str, str]:
        return self.records[idx]


@dataclass
class TextCollator:
    tokenizer: Any
    max_length: int

    def __call__(self, features: List[Dict[str, str]]) -> Dict[str, torch.Tensor]:
        texts = [ex["text"] for ex in features]

        batch = self.tokenizer(
            text=texts,
            return_tensors="pt",
            padding=True,
            truncation=True,
            max_length=self.max_length,
        )

        if "input_ids" not in batch:
            raise RuntimeError("Tokenizer output does not contain input_ids. Check transformers version.")

        labels = batch["input_ids"].clone()

        pad_token_id = self.tokenizer.pad_token_id
        if pad_token_id is not None:
            labels[labels == pad_token_id] = -100

        special_ids = set(self.tokenizer.all_special_ids)
        for token_id in special_ids:
            labels[labels == token_id] = -100

        batch["labels"] = labels
        return batch


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
        # 添加GPT-2模型的层名称
        "c_attn",
        "c_proj",
        "c_fc",
        "c_mlp",
    }
    found: Set[str] = set()

    for name, module in model.named_modules():
        if isinstance(module, torch.nn.Linear):
            suffix = name.split(".")[-1]
            if suffix in candidate_suffixes:
                found.add(suffix)

    targets = sorted(found)
    if not targets:
        # 如果仍然没有找到，尝试打印所有模块名称
        LOGGER.warning("No LoRA target modules found with common suffixes. Printing all module names:")
        for name, module in model.named_modules():
            if isinstance(module, torch.nn.Linear):
                LOGGER.warning("  %s: %s", name, type(module).__name__)
        
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

    try:
        LOGGER.info("Loading tokenizer and model from %s", args.model_name)
        tokenizer = AutoTokenizer.from_pretrained(args.model_name)
        LOGGER.info("Tokenizer loaded successfully")
        
        # 确保tokenizer有pad_token
        if tokenizer.pad_token is None:
            tokenizer.pad_token = tokenizer.eos_token
            LOGGER.info("Set pad_token to eos_token")

        dtype = None
        if args.bf16:
            dtype = torch.bfloat16
        elif args.fp16:
            dtype = torch.float16
        
        LOGGER.info("Loading model with dtype: %s", dtype)
        model = AutoModelForCausalLM.from_pretrained(
            args.model_name,
            torch_dtype=dtype,
        )
        LOGGER.info("Model loaded successfully")

        if args.gradient_checkpointing:
            model.gradient_checkpointing_enable()
            if hasattr(model.config, "use_cache"):
                model.config.use_cache = False
            LOGGER.info("Gradient checkpointing enabled")

        # 根据模型类型设置目标模块
        if "gpt2" in args.model_name.lower():
            # GPT-2特定的目标模块
            target_modules = ["c_attn", "c_proj", "c_fc"]
            LOGGER.info("Using GPT-2 specific target modules: %s", target_modules)
        else:
            # 其他模型使用自动检测
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
        LOGGER.info("LoRA model created")
        model.print_trainable_parameters()

        LOGGER.info("Loading training data from %s", args.train_file)
        train_records = load_jsonl(args.train_file)
        LOGGER.info("Loaded %d training records", len(train_records))
        
        train_records = verify_and_filter_records(train_records, args.max_train_samples)
        LOGGER.info("After filtering: %d training records", len(train_records))
        
        train_dataset = JsonlTextDataset(train_records)
        LOGGER.info("Training dataset created")

        eval_dataset = None
        if args.valid_file:
            LOGGER.info("Loading validation data from %s", args.valid_file)
            valid_records = load_jsonl(args.valid_file)
            valid_records = verify_and_filter_records(valid_records, args.max_eval_samples)
            eval_dataset = JsonlTextDataset(valid_records)
            LOGGER.info("Validation dataset created with %d records", len(eval_dataset))

        collator = TextCollator(tokenizer=tokenizer, max_length=args.max_length)
        LOGGER.info("Data collator created")

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
        LOGGER.info("Training arguments created")

        trainer = Trainer(
            model=model,
            args=training_args,
            train_dataset=train_dataset,
            eval_dataset=eval_dataset,
            data_collator=collator,
            compute_metrics=compute_metrics if eval_dataset is not None else None,
        )
        LOGGER.info("Trainer created")

        LOGGER.info("Start LoRA training")
        train_result = trainer.train()
        LOGGER.info("Training completed")

        trainer.save_model(args.output_dir)
        tokenizer.save_pretrained(args.output_dir)
        LOGGER.info("Model and tokenizer saved to %s", args.output_dir)

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
    except Exception as e:
        LOGGER.error("Error during training: %s", str(e))
        import traceback
        LOGGER.error(traceback.format_exc())
        raise

if __name__ == "__main__":
    main()
