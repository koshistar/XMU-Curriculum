import heapq
from collections import defaultdict
class TreeNode:
    def __init__(self,character,frequency):
        self.character=character
        self.frequency=frequency
        self.left=None
        self.right=None
    def __lt__(self, other):
        if self.frequency != other.frequency:
            return self.frequency<other.frequency
        elif self.character is not None and other.character is not None:
            return ord(self.character)<ord(other.character)
        elif other.character is None:
            return True
        elif self.character is None:
            return False

def build_frequency_map(text,frequency_map):
    for char in text:
        frequency_map[char]+=1
def build_huffman_tree(frequency_map):
    priority_queue=[TreeNode(char,frequency) for char,frequency in frequency_map.items()]
    heapq.heapify(priority_queue)

    while len(priority_queue)>1:
        left=heapq.heappop(priority_queue)
        right=heapq.heappop(priority_queue)
        merged_node=TreeNode(None,left.frequency+right.frequency)
        merged_node.left=left
        merged_node.right=right
        heapq.heappush(priority_queue,merged_node)

    return priority_queue[0]
def generateHuffmanCode(root):
    def dfs(node,code,result):
        if node:
            result[node.character]=code
            dfs(node.left,code+'0',result)
            dfs(node.right,code+'1',result)
    huffman_codes={}
    dfs(root,'',huffman_codes)
    return huffman_codes
def showHuffmanCode(huffman_codes,text):
    for char in text:
        print(huffman_codes[char],end="")

# file=open('实验五素材\\ababcab.txt','r+')
# file=open('实验五素材\\alphaonce.txt','r+')
# file=open('实验五素材\\alphatwice.txt','r+')
# file=open('实验五素材\\as.txt','r+')
file=open('实验五素材\\dictionary.txt','r+')
texts=file.readlines()
frequency_map=defaultdict(int)
for index in range(len(texts)):
    if index != len(texts)-1:
        build_frequency_map(texts[index]+'\n',frequency_map)
    else:
        build_frequency_map(texts[index]+chr(256),frequency_map)

tree=build_huffman_tree(frequency_map)
codes=generateHuffmanCode(tree)
# for char,code in codes.items():
#     print(f"{char}:{code}")
for index in range(len(texts)):
    if index != len(texts)-1:
        showHuffmanCode(codes,texts[index]+'\n')
    else:
        showHuffmanCode(codes,texts[index]+chr(256))