import argparse
import json
import logging
from pathlib import Path
from typing import List, Dict

import torch
from transformers import AutoTokenizer, AutoModelForCausalLM
from peft import PeftModel


LOGGER = logging.getLogger("text_model_infer")


def setup_logging():
    logging.basicConfig(
        format="%(asctime)s | %(levelname)s | %(name)s | %(message)s",
        level=logging.INFO,
    )


def parse_args():
    parser = argparse.ArgumentParser(description="Run inference on valid.jsonl with text model.")
    parser.add_argument("--base_model_name", type=str, default="gpt2", help="基础模型名称或路径")
    parser.add_argument("--adapter_path", type=str, default=None, help="LoRA适配器路径")
    parser.add_argument("--model_dir", type=str, default=None, help="训练后保存的模型目录（如果提供，将忽略base_model_name和adapter_path）")
    parser.add_argument("--valid_file", type=str, required=True, help="valid1.jsonl 路径")
    parser.add_argument("--output_file", type=str, default="predictions.jsonl", help="输出预测文件")
    parser.add_argument("--max_samples", type=int, default=None, help="最多推理多少条")
    parser.add_argument("--max_new_tokens", type=int, default=128)
    parser.add_argument("--prompt", type=str, default="Continue the text:", help="提示词")
    parser.add_argument("--device", type=str, default="auto", choices=["auto", "cpu", "cuda"])
    parser.add_argument("--temperature", type=float, default=0.7, help="生成温度")
    parser.add_argument("--top_p", type=float, default=0.9, help="nucleus采样概率")
    parser.add_argument("--repetition_penalty", type=float, default=1.0, help="重复惩罚")
    return parser.parse_args()


def load_jsonl(path: str) -> List[Dict[str, str]]:
    records = []
    with open(path, "r", encoding="utf-8") as f:
        for lineno, line in enumerate(f, start=1):
            line = line.strip()
            if not line:
                continue
            obj = json.loads(line)
            if "text" not in obj:
                raise ValueError(f"{path} line {lineno} must contain 'text'")
            records.append(
                {
                    "text": str(obj["text"]).strip(),
                }
            )
    return records


def pick_device(device_arg: str) -> str:
    if device_arg == "cpu":
        return "cpu"
    if device_arg == "cuda":
        if not torch.cuda.is_available():
            raise RuntimeError("你指定了 --device cuda，但当前 torch.cuda.is_available() 为 False")
        return "cuda"
    return "cuda" if torch.cuda.is_available() else "cpu"


def pick_dtype(device: str):
    if device == "cuda":
        if torch.cuda.is_bf16_supported():
            return torch.bfloat16
        return torch.float16
    return torch.float32


def main():
    setup_logging()
    args = parse_args()

    device = pick_device(args.device)
    dtype = pick_dtype(device)

    # 确定模型加载方式
    if args.model_dir:
        # 使用训练后保存的完整模型目录
        LOGGER.info("Loading model from %s", args.model_dir)
        model_dir = args.model_dir
        use_lora = False
    elif args.adapter_path:
        # 使用基础模型+LoRA适配器
        LOGGER.info("Loading base model from %s and adapter from %s", args.base_model_name, args.adapter_path)
        model_dir = args.base_model_name
        adapter_path = args.adapter_path
        use_lora = True
    else:
        # 只使用基础模型
        LOGGER.info("Loading base model from %s", args.base_model_name)
        model_dir = args.base_model_name
        use_lora = False

    try:
        # 加载tokenizer
        LOGGER.info("Loading tokenizer from %s", model_dir)
        tokenizer = AutoTokenizer.from_pretrained(model_dir)
        
        # 确保tokenizer有pad_token
        if tokenizer.pad_token is None:
            tokenizer.pad_token = tokenizer.eos_token
        
        # 加载模型
        LOGGER.info("Loading model from %s", model_dir)
        model = AutoModelForCausalLM.from_pretrained(
            model_dir,
            torch_dtype=dtype,
        )
        
        # 如果使用LoRA，加载适配器
        if use_lora:
            LOGGER.info("Loading LoRA adapter from %s", adapter_path)
            model = PeftModel.from_pretrained(model, adapter_path)
        
        model = model.to(device)
        model.eval()
    except Exception as e:
        LOGGER.error("Failed to load model: %s", str(e))
        LOGGER.error("If you're experiencing SSL errors, try using a local model path or check your internet connection.")
        raise

    records = load_jsonl(args.valid_file)
    if args.max_samples is not None:
        records = records[: args.max_samples]

    LOGGER.info("Loaded %d samples from %s", len(records), args.valid_file)

    output_path = Path(args.output_file)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    with open(output_path, "w", encoding="utf-8") as fout:
        for idx, ex in enumerate(records, start=1):
            input_text = ex["text"]
            
            # 构建提示
            prompt = f"{args.prompt} {input_text}"
            
            # 对输入进行编码
            inputs = tokenizer(
                prompt,
                return_tensors="pt",
                truncation=True,
                max_length=512,
            )
            
            inputs = {
                k: v.to(device) if hasattr(v, "to") else v
                for k, v in inputs.items()
            }

            with torch.no_grad():
                output_ids = model.generate(
                    **inputs,
                    max_new_tokens=args.max_new_tokens,
                    do_sample=True,  # 启用采样
                    temperature=args.temperature,
                    top_p=args.top_p,
                    repetition_penalty=args.repetition_penalty,
                    pad_token_id=tokenizer.pad_token_id,
                    eos_token_id=tokenizer.eos_token_id,
                )

            # 只解码新生成的token
            input_ids = inputs["input_ids"]
            generated_ids = output_ids[:, input_ids.shape[1]:]
            pred_text = tokenizer.batch_decode(
                generated_ids,
                skip_special_tokens=True,
            )[0]

            result = {
                "id": idx,
                "input_text": input_text,
                "prompt": args.prompt,
                "prediction": pred_text,
            }
            fout.write(json.dumps(result, ensure_ascii=False) + "\n")

            LOGGER.info("[%d/%d] done", idx, len(records))

    LOGGER.info("Saved predictions to %s", str(output_path))


if __name__ == "__main__":
    main()