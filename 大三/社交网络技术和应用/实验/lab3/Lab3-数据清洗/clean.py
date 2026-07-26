import re
import json

def clean_global_noise(text):
    """去除全局噪音和爬虫残留标记"""
    text = text.replace('\r\n', '\n').replace('\r', '\n')
    text = re.sub(r'\n{3,}', '\n\n', text)
    text = re.sub(r'[ \t\u3000]+', ' ', text)

    # 去除显式无关噪音标记及挂号元数据
    text = re.sub(r'（\d{4}-\d{2}-\d{2}填写）', '', text)
    text = re.sub(r'医生已经通过语音回答了您的问题，请及时收听。：?\s*(?:预约医生：.*)?', '', text)
    text = re.sub(r'请尽快填表，我需要了解你的情况：.*?（.*?）：?', '', text, flags=re.S)
    text = re.sub(r'附件(?:\(\d+\))?：.*', '', text)
    text = re.sub(r'仅医生和患者本人可见', '', text)
    
    # 增加过滤门诊挂号相关的元数据噪音
    text = re.sub(r'(就诊患者|就诊时间|就诊医院|就诊地址|挂号费用|疾病|病情描述|希望获得的帮助|过敏史|既往病史|患病多久|用药情况|开药需求)：.*?(?=\n|$)', '', text)

    text = re.sub(r'[\x00-\x09\x0b-\x0c\x0e-\x1f\x7f\u200b\u200c\u200d]+', '', text)
    return text

def post_process_messages(messages, max_message_length=2000, min_assistant_length=8):
    """
    后处理消息列表：
    1. Strip 所有消息的前后空白符
    2. 合并过短的 assistant 消息到前一条或删除
    3. 截断超长消息
    4. 确保消息对齐（user-assistant-user-...）
    """
    if not messages:
        return messages

    # Step 1: Strip 所有消息内容
    for msg in messages:
        msg['content'] = msg['content'].strip()

    # Step 2: 过滤空内容消息，合并过短的 assistant 消息
    cleaned_messages = [messages[0]]  # 保留 system
    
    for i in range(1, len(messages)):
        msg = messages[i]
        
        # 跳过内容为空的消息
        if not msg['content']:
            continue
        
        # 如果是过短的 assistant 消息（可配置的最小长度），尝试合并
        if msg['role'] == 'assistant' and len(msg['content']) < min_assistant_length:
            # 特殊的单字符/标点符号，直接跳过
            if msg['content'] in ['。', '，', '、', '…', '！', '？', 'yes', 'no', '是', '否', '可以', '不行']:
                continue
            # 如果前一条也是 assistant，合并
            if cleaned_messages and cleaned_messages[-1]['role'] == 'assistant':
                cleaned_messages[-1]['content'] += ' ' + msg['content']
                continue
            # 否则保留（可能是有意义的短问句）
        
        # 如果是过短的 user 消息（<3字），多数是噪音，跳过
        if msg['role'] == 'user' and len(msg['content']) < 3:
            continue
        
        # Step 3: 截断超长消息
        if len(msg['content']) > max_message_length:
            msg['content'] = msg['content'][:max_message_length] + f"\n...【内容因过长被截断，原长度{len(msg['content'])}字】"
        
        cleaned_messages.append(msg)
    
    # Step 4: 避免连续的相同角色消息，确保交替出现
    final_messages = [cleaned_messages[0]]  # system
    for msg in cleaned_messages[1:]:
        # 如果和前一条角色相同（除了 system），合并它们
        if final_messages and final_messages[-1]['role'] == msg['role'] and final_messages[-1]['role'] != 'system':
            final_messages[-1]['content'] += '\n\n' + msg['content']
        else:
            final_messages.append(msg)
    
    return final_messages


def parse_record(raw_record):
    """将单条原始文本解析为结构化的对话格式"""
    messages = []
    
    # 1. 提取医生所属科室 (构建 System Prompt)
    faculty_match = re.search(r'Doctor faculty\n(.*?)(?=\n|$)', raw_record)
    system_prompt = "你是一位专业的在线问诊医生，请根据患者的描述提供专业、耐心的医疗建议。"
    if faculty_match and faculty_match.group(1).strip():
        faculty = faculty_match.group(1).strip()
        system_prompt = f"你是一位来自【{faculty}】的专业医生，请根据患者的描述提供专业、耐心的医疗建议。"
    messages.append({"role": "system", "content": system_prompt})

    # 使用关键字分块策略，应对乱序问题
    # 将文本按照 Description, Dialogue, Diagnosis and suggestions 切割
    blocks = re.split(r'\n(?=Description\n|Dialogue\n|Diagnosis and suggestions\n)', '\n' + raw_record)
    
    user_initial_content = []
    dialogue_lines = []
    diagnosis_content = ""

    # 将切好的块归类
    for block in blocks:
        block = block.strip()
        if block.startswith('Description'):
            desc_text = block.replace('Description', '').strip()
            user_initial_content.append(desc_text)
        elif block.startswith('Dialogue'):
            dialogue_lines.extend(block.replace('Dialogue', '').strip().split('\n'))
        elif block.startswith('Diagnosis and suggestions'):
            diagnosis_content += block.replace('Diagnosis and suggestions', '').strip() + "\n"

    # 2. 构建首个 User Prompt
    full_desc = "\n".join(user_initial_content).strip()
    if full_desc:
        clean_desc_lines = [line.strip() for line in full_desc.split('\n') if line.strip() and line.strip() != "无" and "未填写" not in line]
        if clean_desc_lines:
            messages.append({"role": "user", "content": "【患者基本信息与诉求】\n" + "\n".join(clean_desc_lines)})

    # 3. 解析多轮对话
    current_role = None
    current_content = []
    
    for line in dialogue_lines:
        line = line.strip()
        if not line: continue
        
        if line.startswith('病人：') or line.startswith('患者：'):
            if current_role == 'assistant' and current_content:
                messages.append({"role": "assistant", "content": "\n".join(current_content)})
                current_content = []
            current_role = 'user'
            current_content.append(line.replace('病人：', '').replace('患者：', '').strip())
            
        elif line.startswith('医生：'):
            if current_role == 'user' and current_content:
                if messages and messages[-1]['role'] == 'user':
                    messages[-1]['content'] += "\n\n【患者补充】\n" + "\n".join(current_content)
                else:
                    messages.append({"role": "user", "content": "\n".join(current_content)})
                current_content = []
            current_role = 'assistant'
            current_content.append(line.replace('医生：', '').strip())
        else:
            if current_role:
                current_content.append(line)
                
    # 扫尾最后的对话内容
    if current_role and current_content:
        if current_role == 'user':
            if messages and messages[-1]['role'] == 'user':
                messages[-1]['content'] += "\n\n【患者补充】\n" + "\n".join(current_content)
            else:
                messages.append({"role": "user", "content": "\n".join(current_content)})
        else:
            messages.append({"role": "assistant", "content": "\n".join(current_content)})

    # 4. 追加诊断与建议
    diagnosis_content = diagnosis_content.strip()
    if diagnosis_content:
        # 如果最后一句是医生的回复，直接拼进去；如果是患者结尾或暂无对话，新建一个 assistant 节点
        if messages and messages[-1]['role'] == 'assistant':
            messages[-1]['content'] += "\n\n【诊断与建议】\n" + diagnosis_content
        else:
            messages.append({"role": "assistant", "content": "【诊断与建议】\n" + diagnosis_content})

    # 5. 质量校验
    has_user = any(m['role'] == 'user' for m in messages)
    has_assistant = any(m['role'] == 'assistant' for m in messages)
    if not (has_user and has_assistant):
        return None

    # 6. 后处理：清理格式、合并短回复、截断超长
    # min_assistant_length: 医生回复的最小字符数，低于此的会被过滤或合并
    messages = post_process_messages(messages, max_message_length=2000, min_assistant_length=8)

    # 7. 再次校验（防止后处理后变空）
    if len(messages) < 3:  # system + user + assistant 最少
        return None

    return {"messages": messages}

def main():
    input_file = 'qa.txt'
    output_file = 'clean_dataset.json'

    with open(input_file, 'r', encoding='utf-8') as f:
        raw_text = f.read()

    cleaned_text = clean_global_noise(raw_text)

    raw_records = []
    for m in re.finditer(
        r'(?:\n)?(?:\[?source:\s*\d+\]?\s*)?id=(\d+)\s*\n(https?://\S+)[ \t]*\n(.*?)(?=(?:\n(?:\[?source:|id=\d+)|\Z))',
        cleaned_text,
        re.S | re.M,
    ):
        raw_records.append({
            'id': m.group(1),
            'url': m.group(2),
            'body': m.group(3).strip(),
        })

    if not raw_records:
        raw_records = [{'id': None, 'url': None, 'body': rec.strip()} for rec in re.split(r'\n*id=\d+\s*\nhttps?://\S+\n*', cleaned_text) if rec.strip()]

    dataset = []
    stats = {'total': 0, 'parsed': 0, 'skipped': 0, 'skipped_detail': {}} 

    for rec in raw_records:
        stats['total'] += 1
        parsed_record = parse_record(rec['body'])
        if parsed_record:
            parsed_record['meta'] = {'id': rec.get('id'), 'url': rec.get('url')}
            dataset.append(parsed_record)
            stats['parsed'] += 1
        else:
            stats['skipped'] += 1
            stats['skipped_detail'].setdefault('invalid_format', 0)
            stats['skipped_detail']['invalid_format'] += 1

    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(dataset, f, ensure_ascii=False, indent=2)

    print(f"数据清洗完成！共处理出 {len(dataset)} 条有效对话记录。")
    print(f"总记录数 {stats['total']}，成功 {stats['parsed']}，跳过 {stats['skipped']}。")
    if stats['skipped']:
        print(f"跳过原因（合计）：{stats['skipped_detail']}")

if __name__ == "__main__":
    main()