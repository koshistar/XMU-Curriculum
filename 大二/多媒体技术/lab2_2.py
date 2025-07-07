import sounddevice as sd
import soundfile as sf
from pydub import AudioSegment
from pydub.playback import play
import numpy as np
#使用sounddevice和soundfile
# data,fs=sf.read('op.mp3',dtype="float32")
# duration=len(data)/fs
# newData=[]
# for second in range(int(duration)):
#     if second %2!=0:
#         start=int(second*fs)
#         end=int((second+1)*fs)
#         newData.append(data[start:end])
# newData=np.concatenate(newData,axis=0)
# sd.play(newData,fs)
# sf.write("op_seg1.mp3",newData,fs)
# status=sd.wait()
#使用pydub
audio=AudioSegment.from_wav("music.wav")
newAudio=AudioSegment.empty()
duration=len(audio)
second=0
while second<duration:
    if second%2000!=0:
        start=second
        end=min(second+1000,duration)
        newAudio+=audio[start:end]
    second+=1000
play(newAudio)