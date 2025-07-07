import matplotlib.pyplot as plt
file=open('alphatwice.txt',"r+")
wordcount={}
for word in file.read():
    if word.isalpha():
        if word not in wordcount:
            wordcount[word]=1
        else:
            wordcount[word]+=1
cnt=sorted(wordcount.items(),key=lambda x:x[1],reverse=True)
x=[val for val in dict(cnt).keys()]
y=[val for val in dict(cnt).values()]
plt.xlabel("letters")
plt.ylabel("frequency")
plt.title("lab2-3")
plt.bar(x[:10],y[:10],width=1)
plt.show()
file.close()