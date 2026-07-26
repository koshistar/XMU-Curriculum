import json
import os
import requests
import jieba
import jieba.analyse
from urllib.parse import quote
import time
import hashlib

# ===================== 配置区（修改这里即可）=====================
INPUT_FILE = 'data_pretrain.jsonl'       # 你的输入文件
OUTPUT_FILE = 'image_info.jsonl'        # 输出文件
IMAGES_DIR = 'images'                   # 图片保存文件夹
COOKIE = "BAIDUID=C3C001E032314FEABC4D336BB9CFD549:FG=1; BAIDUID_BFESS=C3C001E032314FEABC4D336BB9CFD549:FG=1; __bid_n=19b544f8673988896cfbf5; BAIDU_WISE_UID=wapp_1766681302919_565; ZFY=XMcsPTyQkviI:AJq1Fwp2ZzjNZVqhawu9vhuso3rukFg:C; BIDUPSID=C3C001E032314FEABC4D336BB9CFD549; PSTM=1768398438; H_PS_PSSID=63147_67078_67224_67316_67432_67499_67540_67556_67601_67644_67714_67746_67752_67730_67813_67823_67824_67827_67837_67857_67851_67853_67864_67859_67862; jsdk-uuid=97d208b7-ff46-48c2-aab1-1fb50fc88b93; newlogin=1; H_WISE_SIDS=68166_68372_68448_68464_68547_68623_68676_68923_68978_68999_69005_69010_69020_69013_69021_69072_69036_69111_69167_69183_69202_69215_69244_69228_69238_69231_69237_69291_69295_68779_69319_69250_69253_69254_69257_69259_69081_69377_69364; ab_sr=1.0.1_OTFkOGIwODhmNDBhOTJmNTliMTcxNDk3MmRiYjU5ZGNiNmIwYTUxZjU5NTk4ODkyN2Q0NTRjODNjOTQwY2VkMzkzYzIzY2NkZjhlMTc2ZWZlNTE2MWI3MWI0NWQyNWZmMjdjNDU1MmM2YjUzYjU1YTZhN2M1NDI5ZTQzYTkxY2UyZTI4YmFiMzJhMzRiMmJkY2QzZjdiOGM1NjEwY2E4Mg=="
# =================================================================

# 确保文件夹存在
os.makedirs(IMAGES_DIR, exist_ok=True)

def extract_keywords(text, top_k=5):
    """提取关键词"""
    keywords = jieba.analyse.extract_tags(text, topK=top_k, allowPOS=('n', 'vn', 'nz'))
    return keywords

def search_images_baidu(keyword, max_images=3):
    """百度图片 稳定JSON接口（返回图片URL+描述）"""
    # 百度图片官方异步JSON接口（最稳定，不会失效）
    url = "https://image.baidu.com/search/acjson"
    
    # 请求参数（官方标准参数）
    params = {
        "tn": "resultjson_com",
        "logid": "110110110110110",
        "ipn": "rj",
        "ct": "201326592",
        "fp": "result",
        "queryWord": keyword,
        "word": keyword,
        "pn": "0",      # 页码
        "rn": "30",     # 每页30张
        "ie": "utf-8",
        "oe": "utf-8",
        "adpicid": "",
        "st": "-1",
        "z": "",
        "ic": "0",
        "hd": "1",      # 高清图
        "latest": "0",
        "copyright": "0"
    }

    # 【核心】完整请求头 + Cookie，绕过反爬
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) Chrome/130.0.0.0 Safari/537.36",
        "Referer": "https://image.baidu.com/",
        "Accept": "application/json, text/plain, */*",
        "Accept-Language": "zh-CN,zh;q=0.9",
        "Host": "image.baidu.com",
        "Cookie": COOKIE
    }

    try:
        time.sleep(1.5)  # 强制延时，防封IP
        response = requests.get(url, headers=headers, params=params, timeout=10)
        response.raise_for_status()
        
        # 直接解析JSON（无HTML解析，彻底解决报错）
        data = response.json()
        
        # 提取图片URL + 描述（核心修改：新增描述字段提取）
        image_infos = []
        for item in data.get("data", [])[:max_images]:
            # 高清原图URL
            img_url = item.get("middleURL") or item.get("thumbURL") or item.get("objURL")
            # 图片描述（优先取desc，无则取raw_title，仍无则用关键词兜底）
            img_desc = item.get("desc") or item.get("raw_title") or item.get("fromPageTitleEnc") or item.get("fromPageTitle") or item.get("pageTitle") or keyword
            if img_url:
                image_infos.append({
                    "url": img_url,
                    "desc": img_desc.strip()  # 去除首尾空格
                })
        
        return image_infos  # 改为返回包含URL和描述的列表

    except Exception as e:
        print(f"搜索失败 {keyword}: {str(e)[:50]}")
        return []

def download_image(url, save_path):
    """下载图片"""
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) Chrome/130.0.0.0 Safari/537.36",
        "Referer": "https://image.baidu.com/"
    }
    try:
        time.sleep(0.5)
        res = requests.get(url, headers=headers, timeout=10)
        with open(save_path, "wb") as f:
            f.write(res.content)
        print(f"✅ 下载成功: {save_path}")
        return True
    except:
        print(f"❌ 下载失败: {url}")
        return False

def main():
    jieba.initialize()
    with open(INPUT_FILE, "r", encoding="utf-8") as f:
        for line_num, line in enumerate(f, 1):
            line = line.strip()
            if not line:
                continue
            
            data = json.loads(line)
            text = data.get("text", "")
            if not text:
                continue

            # 提取3个关键词
            keywords = extract_keywords(text, 3)
            print(f"\n===== 处理第 {line_num} 行，关键词: {keywords} =====")

            for idx, kw in enumerate(keywords, 1):
                print(f"\n关键词 {idx}/3: {kw}")
                # 改为获取图片URL+描述的列表
                image_infos = search_images_baidu(kw)
                if not image_infos:
                    print("无图片")
                    continue

                # 下载图片（遍历图片信息，而非仅URL）
                for img_info in image_infos:
                    img_url = img_info["url"]
                    img_desc = img_info["desc"]  # 获取图片描述
                    # 生成图片文件名
                    name = f"{line_num}_{idx}_{hashlib.md5(img_url.encode()).hexdigest()[:8]}.jpg"
                    path = os.path.join(IMAGES_DIR, name)
                    # 下载图片
                    if download_image(img_url, path):
                        # 写入结果（核心修改：text字段用图片描述）
                        with open(OUTPUT_FILE, "a", encoding="utf-8") as f_out:
                            json.dump({
                                "image": path.replace("\\", "/"), 
                                "text": img_desc  # 替换为图片描述
                            }, f_out, ensure_ascii=False)
                            f_out.write("\n")

if __name__ == "__main__":
    main()