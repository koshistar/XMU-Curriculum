"""
使用API进行医疗数据质量评估，这里选择的是OpenRouter.AI 
"""

import json
import os
import requests
from tqdm import tqdm
import sys
import re
import time


# ==========================================
# 配置区：请替换为你的 Model和 API Key
# ==========================================
class StrictQwenFilter:

    def __init__(self, api_key=None, model="openrouter/auto"):
       
        self.api_key = api_key or os.environ.get("API_KEY")
        if not self.api_key:
            print("❌ 错误：未设置 API 密钥")
            print("请设置环境变量 API_KEY 或在代码中传入 api_key 参数")
            sys.exit(1)
        
        self.model = model
        #请替换为你选择的模型
        self.api_url = "https://openrouter.ai/api/v1/chat/completions"
        self.headers = {
            "Authorization": f"Bearer {self.api_key}",
            "Content-Type": "application/json"
        }
        
        print(f"  已初始化")
        print(f"  模型: {self.model}")
        print(f"  API: {self.api_url}\n")
    
    def build_prompt(self, patient_issue, doctor_response):
        """构建更严格的评估提示"""
        return f"""你是一位资深医学审核专家。请严格评估以下医疗对话的质量。

【患者描述】
{patient_issue}

【医生回答】
{doctor_response}

请从以下6个维度进行评估，给出严格的评分（0-100）：

1. 医学准确性：医学知识是否准确？是否有错误或误导？
2. 诊断明确性：诊断过程是否清晰？是否需要进一步检查？
3. 建议质量：建议是否具体、可执行、科学根据充分？
4. 安全性：是否有任何潜在的危险建议？
5. 患者友好度：表述是否易懂？是否具有同理心？
6. 完整性：回答是否完整？是否漏掉重要内容？

总体评分标准：
- 90-100分：优秀（各维度均表现出色）
- 75-89分：良好（大部分维度表现良好）
- 60-74分：及格（基本满足要求但有不足）
- 40-59分：一般（有明显缺陷）
- 0-39分：较差（质量问题较多）

请按以下格式严格回复：
总体评分: X/100
医学准确性: X/100
诊断明确性: X/100
建议质量: X/100
安全性: X/100
患者友好度: X/100
完整性: X/100
---
判断: 高质量/良好/及格/一般/较差
理由: (简要说明主要优缺点)"""
    
    def parse_response(self, response):
        """改进的响应解析"""
        try:
            lines = response.split('\n')
            
            scores = {}
            overall_score = 50
            reason = ""
            
            for line in lines:
                # 提取总体评分
                if "总体评分:" in line or "总评分:" in line:
                    nums = re.findall(r'\d+', line)
                    if nums:
                        overall_score = int(nums[0])
                        if overall_score > 100:
                            overall_score = 100
                
                # 提取各维度评分
                if "医学准确性:" in line:
                    nums = re.findall(r'\d+', line)
                    if nums:
                        scores['accuracy'] = int(nums[0])
                
                if "建议质量:" in line:
                    nums = re.findall(r'\d+', line)
                    if nums:
                        scores['advice'] = int(nums[0])
                
                # 提取理由
                if "理由:" in line or "原因:" in line:
                    reason = line.split(":")[-1].strip()
            
            # 如果没找到总体评分但找到维度评分，计算平均值
            if overall_score == 50 and scores:
                overall_score = sum(scores.values()) / len(scores)
                overall_score = min(100, int(overall_score))
            
            # 判断等级
            if overall_score >= 75:
                quality = "高质量"
            elif overall_score >= 60:
                quality = "中等"
            else:
                quality = "低质量"
            
            return {
                "score": overall_score,
                "quality_level": quality,
                "reason": reason,
                "dimension_scores": scores
            }
        except Exception as e:
            return {
                "score": 50,
                "quality_level": "中等",
                "reason": f"解析错误: {str(e)[:30]}",
                "dimension_scores": {}
            }
    
    def evaluate_dialogue(self, dialogue_data):
        """评估单条对话"""
        messages = dialogue_data.get("messages", [])
        
        assistant_responses = []
        user_questions = []
        
        for msg in messages:
            role = msg.get("role", "")
            content = msg.get("content", "").strip()
            
            if role == "assistant" and content:
                assistant_responses.append(content)
            elif role == "user" and content and not content.startswith("system"):
                user_questions.append(content)
        
        if not assistant_responses or not user_questions:
            return {"score": 0, "quality_level": "低质量", "reason": "数据不完整"}
        
        patient_issue = user_questions[0][:300]
        doctor_response = assistant_responses[0][:400]
        
        prompt = self.build_prompt(patient_issue, doctor_response)
        
        try:
            # 调用API
            payload = {
                "model": self.model,
                "messages": [
                    {"role": "user", "content": prompt}
                ],
                "temperature": 0.1,
                "top_p": 0.95,
                "max_tokens": 400
            }
            
            response = requests.post(
                self.api_url,
                headers=self.headers,
                json=payload,
                timeout=30
            )
            
            if response.status_code != 200:
                error_msg = response.text
                print(f"API 错误 ({response.status_code}): {error_msg[:100]}")
                return {"score": 50, "quality_level": "中等", "reason": f"API 错误: {response.status_code}"}
            
            result = response.json()
            
            # 提取生成的文本
            if "choices" not in result or len(result["choices"]) == 0:
                print(f"API 响应格式错误")
                return {"score": 50, "quality_level": "中等", "reason": "API 响应格式错误"}
            
            generated_text = result["choices"][0].get("message", {}).get("content", "")
            
            # 添加速率限制（避免频繁调用）
            time.sleep(0.5)
            
            return self.parse_response(generated_text)
            
        except requests.exceptions.Timeout:
            print(f"API 请求超时")
            return {"score": 50, "quality_level": "中等", "reason": "API 请求超时"}
        except requests.exceptions.RequestException as e:
            print(f"网络错误: {str(e)[:50]}")
            return {"score": 50, "quality_level": "中等", "reason": f"网络错误: {str(e)[:50]}"}
        except Exception as e:
            print(f"评估出错: {e}")
            return {"score": 50, "quality_level": "中等", "reason": f"错误: {str(e)[:50]}"}
    
    def filter_dataset(self, input_file, output_dir="filtered_by_openrouter", threshold_high=75, threshold_medium=60):
        """筛选数据集"""
        os.makedirs(output_dir, exist_ok=True)
        
        print(f"开始筛选数据集: {input_file}")
        print(f"高质量阈值: {threshold_high}分")
        print(f"中等质量阈值: {threshold_medium}分\n")
        
        with open(input_file, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        print(f"总数据条数: {len(data)}\n")
        
        high_quality = []
        medium_quality = []
        low_quality = []
        
        for idx, item in enumerate(tqdm(data, desc="评估中"), 1):
            result = self.evaluate_dialogue(item)
            
            item["qwen_evaluation"] = result
            item["qwen_score"] = result["score"]
            
            score = result["score"]
            if score >= threshold_high:
                high_quality.append(item)
            elif score >= threshold_medium:
                medium_quality.append(item)
            else:
                low_quality.append(item)
        
        # 保存结果
        print(f"\n正在保存结果...\n")
        
        with open(os.path.join(output_dir, "high_quality.json"), 'w', encoding='utf-8') as f:
            json.dump(high_quality, f, ensure_ascii=False, indent=2)
        print(f"✓ 高质量数据: {len(high_quality)} 条")
        
        with open(os.path.join(output_dir, "medium_quality.json"), 'w', encoding='utf-8') as f:
            json.dump(medium_quality, f, ensure_ascii=False, indent=2)
        print(f"✓ 中等质量数据: {len(medium_quality)} 条")
        
        with open(os.path.join(output_dir, "low_quality.json"), 'w', encoding='utf-8') as f:
            json.dump(low_quality, f, ensure_ascii=False, indent=2)
        print(f"✓ 低质量数据: {len(low_quality)} 条")
        
        with open(os.path.join(output_dir, "all_with_scores.json"), 'w', encoding='utf-8') as f:
            json.dump(data, f, ensure_ascii=False, indent=2)
        print(f"✓ 全部数据（含评分）已保存")
        
        # 计算统计信息
        scores = [item["qwen_score"] for item in data]
        print("\n" + "="*60)
        print("筛选完成")
        print("="*60)
        print(f"高质量(≥{threshold_high}分): {len(high_quality):>3} 条 ({100*len(high_quality)/len(data):.1f}%)")
        print(f"中等({threshold_medium}-{threshold_high-1}分): {len(medium_quality):>3} 条 ({100*len(medium_quality)/len(data):.1f}%)")
        print(f"低质量(<{threshold_medium}分):   {len(low_quality):>3} 条 ({100*len(low_quality)/len(data):.1f}%)")
        print("-"*60)
        print(f"平均分: {sum(scores)/len(scores):.1f}")
        print(f"最高分: {max(scores)}")
        print(f"最低分: {min(scores)}")
        print("="*60 + "\n")


def main():
    print("\n" + "="*60)
    print("医疗数据质量筛选工具")
    print("="*60 + "\n")
    
    # 从环境变量读取 API 密钥
    api_key = os.environ.get("API_KEY")
    if not api_key:
        print("请设置环境变量 API_KEY")
        print("使用方法:")
        print("  $env:API_KEY='your-api-key'  # PowerShell")
        print("  export API_KEY='your-api-key'  # bash")
        sys.exit(1)
    
    filter_tool = StrictQwenFilter(api_key=api_key)
    filter_tool.filter_dataset("clean_dataset.json", output_dir="filtered_by_openrouter", threshold_high=75, threshold_medium=60)
    
    print("全部完成!")


if __name__ == "__main__":
    main()
