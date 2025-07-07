data = [1,2,3,12,20,100,6,9,12,8,8,3]
def average():
    sum=0
    for i in data:
        sum+=i
    print(sum/len(data))
def maxData():
    m=0
    for i in data:
        if i>m:
            m=i
    print(m)
def bubbleSort():
    n=len(data)
    for i in range(n):
        for j in range(0,n-i-1):
            if data[j]>data[j+1]:
                data[j],data[j+1]=data[j+1],data[j]
    print(data)
def insert(position,value):
    data.insert(position,value)
    print(data)
def erase(position):
    data.pop(position)
    print(data)
average()
maxData()
bubbleSort()
insert(3,4)
erase(5)
