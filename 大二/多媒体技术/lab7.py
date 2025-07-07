import math
from bitarray import bitarray
window_size=10
lookahead_buffer_size=15
def find_longest_match(data,pos):
    end_of_buffer=min(pos+lookahead_buffer_size,len(data)+1)
    best_match_distance=-1
    best_match_length=-1

    for j in range(pos+2,end_of_buffer):
        start_index=max(0,pos-window_size)
        substring=data[pos:j]
        if len(substring)>best_match_length:
            for i in range(start_index,pos):
                repetitions=len(substring)//(pos-i)
                last=len(substring)%(pos-i)
                matched_string=data[i:pos]*repetitions+data[i:i+last]
                if matched_string==substring:
                    best_match_distance=pos-i
                    best_match_length=len(substring)

    if best_match_distance>0 and best_match_length>0:
        return best_match_distance,best_match_length
    return None

def compress(input_file_path,output_file_path):
    data=None
    i=0
    output_buffer=bitarray(endian='big')

    try:
        with open(input_file_path,'rb') as input_file:
            data=input_file.read()
    except IOError:
        print("不能打开路径"+input_file_path)
        raise
    while i < len(data):
        match=find_longest_match(data,i)
        if match:
            (bestMatchDistance,bestMatchLength)=match
            output_buffer.append(True)
            output_buffer.frombytes(bytes([bestMatchDistance>>4]))
            output_buffer.frombytes(bytes([((bestMatchDistance&0xf)<<4)|bestMatchLength]))
            i+=bestMatchLength
        else:
            output_buffer.append(False)
            output_buffer.frombytes(bytes([data[i]]))
            i+=1
    output_buffer.fill()

    if output_file_path:
        try:
            with open(output_file_path,'wb') as output_file:
                output_file.write(output_buffer.tobytes())
                return None
        except IOError:
            print("不能打开文件路径"+output_file_path)
            raise
    #return output_buffer

def decompress(input_file_path,output_file_path):
    data=bitarray(endian='big')
    output_buffer=[]

    try:
        with open(input_file_path,'rb') as input_file:
            data.fromfile(input_file)
    except IOError:
        print("不能打开文件路径"+input_file_path)
        raise
    while len(data)>=9:
        flag=data.pop(0)
        if not flag:
            byte=data[0:8].tobytes()
            output_buffer.append(byte)
            del data[0:8]
        else:
            byte1=ord(data[0:8].tobytes())
            byte2=ord(data[8:16].tobytes())
            del data[0:16]
            distance=(byte1<<4)|(byte2>>4)
            length=(byte2&0xf)
            for i in range(length):
                output_buffer.append(output_buffer[-distance])
    out_data=b''.join(output_buffer)
    if output_file_path:
        try:
            with open(output_file_path,'wb') as output_file:
                output_file.write(out_data)
                return None
        except IOError:
            print("不能打开文件路径"+output_file_path)
            raise
    #return out_data

input_file_path='text file/asciiart.txt'
output_file_path='output1.txt'
compress(input_file_path,output_file_path)
de_output_file_path='output2.txt'
decompress(output_file_path,de_output_file_path)