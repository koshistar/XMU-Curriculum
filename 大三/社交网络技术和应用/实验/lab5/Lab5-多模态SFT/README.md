# Simple Multimodal SFT Dataset - 100 Images

This folder contains 100 synthetic simple images and JSONL files for SmolVLM multimodal SFT.

## Files

- images/: 100 PNG images, 512 x 512
- sft_train.jsonl: 90 training samples
- sft_valid.jsonl: 10 validation samples
- sft_all.jsonl: all 100 samples

## JSONL format

Each line is one sample:

{"image": "./images/001_red_circle.png", "question": "What is the main object in this image?", "answer": "The image shows a red circle on a white background."}

## Suggested training command on Windows PowerShell

python sft_lora.py --model_name HuggingFaceTB/SmolVLM-500M-Instruct --train_file sft_train.jsonl --valid_file sft_valid.jsonl --output_dir ./outputs/smolvlm_sft_lora --num_train_epochs 5 --per_device_train_batch_size 1 --gradient_accumulation_steps 1 --max_length 512 --logging_steps 1 --save_steps 50

## Suggested inference command

python infer_lora.py --base_model HuggingFaceTB/SmolVLM-500M-Instruct --adapter_dir ./outputs/smolvlm_sft_lora --image ./images/001_red_circle.png --question "What is the main object in this image?"
