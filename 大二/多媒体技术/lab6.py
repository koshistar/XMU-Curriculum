import math
from collections import defaultdict
from decimal import Decimal,getcontext

def build_frequency_map(text,frequency_map):
    for char in text:
        frequency_map[char]+=1

def build_probability_map(texts):
    frequency_map=defaultdict(int)
    countChar=0
    for index in range(len(texts)):
        if index != len(texts)-1:
            build_frequency_map(texts[index]+'\n',frequency_map)
        else:
            build_frequency_map(texts[index]+chr(256),frequency_map)
        countChar+=len(texts[index])+1
    probability_map={char:count/countChar for char,count in frequency_map.items()}
    return probability_map

def dec2bin(value,precision=64):
    binary=[]
    for _ in range(precision):
        value*=2
        bit=int(value)
        binary.append(str(bit))
        value-=bit
        if value==0:
            break
    return ''.join(binary)

def bin2dec(binary):
    value=0.0
    for i,bit in enumerate(binary):
        value+=int(bit)*(2**-(i+1))
    return value

def cal_required_bits(probability_map,length):
    min_prob=min(probability_map.values())
    required_bits=math.ceil(-length*math.log2(min_prob))+10
    return max(required_bits,64)

def encode(texts,probability_map):
    low=0.0
    high=1.0
    cum_probs={}
    cumulative=0.0
    for char,prob in probability_map.items():
        cum_probs[char]=(cumulative,cumulative+prob)
        cumulative+=prob
    length=0
    for index in range(len(texts)):
        if index != len(texts) - 1:
            for char in texts[index]:
                range_width=high-low
                high=low+range_width*cum_probs[char][1]
                low=low+range_width*cum_probs[char][0]
            range_width=high-low
            high=low+range_width*cum_probs['\n'][1]
            low=low+range_width*cum_probs['\n'][0]
        else:
             for char in texts[index]:
                range_width = high - low
                high = low + range_width * cum_probs[char][1]
                low = low + range_width * cum_probs[char][0]
             range_width = high - low
             high = low + range_width * cum_probs[chr(256)][1]
             low = low + range_width * cum_probs[chr(256)][0]
        length+=len(texts[index])+1
    mid=(low+high)/2
    binary=dec2bin(mid,cal_required_bits(probability_map,length))
    return binary,length

def decode(encoded_binary,length,probability_map):
    getcontext().prec=50
    low=Decimal(0)
    high=Decimal(1)
    decoded_text=[]
    encoded_value=Decimal(str(bin2dec(encoded_binary)))

    cum_probs={}
    cumulative=Decimal(0)
    for char,prob in probability_map.items():
        prob_dec=Decimal(str(prob))
        cum_probs[char]=(cumulative,cumulative+prob_dec)
        cumulative+=prob_dec
    for _ in range(length):
        range_width=high-low
        if range_width<=0:
            raise ValueError("精度不足")
        current_value=(encoded_value-low)/range_width

        char=None
        for c,(lower,upper) in cum_probs.items():
            if lower<=current_value<upper:
                char=c
                break
        if char is None:
            raise ValueError("解码错误")
        decoded_text.append(char)
        high=low+range_width*cum_probs[char][1]
        low=low+range_width*cum_probs[char][0]
    return ''.join(decoded_text)


file=open("实验五素材\\ababcab.txt",'r+')
# file=open("实验五素材\\alphaonce.txt",'r+')
texts=file.readlines()
probability_map=build_probability_map(texts)
result_coded=encode(texts,probability_map)
print(result_coded[0])
print(decode(result_coded[0],result_coded[1],probability_map))