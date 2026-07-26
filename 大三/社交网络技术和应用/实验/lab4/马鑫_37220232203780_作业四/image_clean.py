import json
import os
from collections import defaultdict

def process_image_info(jsonl_file):
    # 读取jsonl文件
    data = []
    with open(jsonl_file, 'r', encoding='utf-8') as f:
        for line in f:
            data.append(json.loads(line))
    
    # 先清洗text字段
    for item in data:
        if 'text' in item:
            # 保留中文、英文、数字和基本标点
            import re
            item['text'] = re.sub(r'[^\u4e00-\u9fa5a-zA-Z0-9\s，。！？、：；""''（）《》【】\-\.,?!:;""\'()\[\]{}]', '', item['text'])
            # 去除多余空格
            item['text'] = ' '.join(item['text'].split())
    
    # 去除重复条目（基于text字段）
    seen_texts = set()
    unique_data = []
    duplicate_images = set()
    
    for item in data:
        text_key = item.get('text', '')
        if text_key not in seen_texts:
            seen_texts.add(text_key)
            unique_data.append(item)
        else:
            duplicate_images.add(item['image'])
    
    # 删除重复图片
    image_dir = os.path.dirname(jsonl_file)
    for image_name in duplicate_images:
        image_path = os.path.join(image_dir, image_name)
        if os.path.exists(image_path):
            os.remove(image_path)
    
    # 保存处理后的数据
    output_file = jsonl_file.replace('.jsonl', '_processed.jsonl')
    with open(output_file, 'w', encoding='utf-8') as f:
        for item in unique_data:
            f.write(json.dumps(item, ensure_ascii=False) + '\n')
    
    return output_file

# 使用示例
if __name__ == "__main__":
    input_file = "image_info.jsonl"
    process_image_info(input_file)
