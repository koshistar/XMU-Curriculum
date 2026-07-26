import random
import math
import sys
import time

# 常量定义（对应原C++宏定义）
maxpop = 160
maxstring = 81
n = 10
pi = 3.141592653589793

# 全局参数设置
popsize = 50          # 种群大小
lchrom = 10           # 每个变量的染色体长度（二进制位数）
pcross = 0.6          # 交叉概率
pmutation = 0.03      # 变异概率
sumfitness = 0.0      # 总适应度
avg = 0.0             # 平均适应度
p = 0                 # 优化变量的个数
min_val = []          # 每个变量的最小值
max_val = []          # 每个变量的最大值

# 个体类（对应原C++的struct individual）
class Individual:
    def __init__(self):
        self.chrom = [0] * maxstring  # 染色体（二进制编码）
        self.x = [0.0] * n            # 变量取值
        self.fitness = 0.0            # 适应度值

# 定义适应度函数
def f(popu):
    #检验函数1,求全局极小值
	# func=100*(popu.x[0]*popu.x[0]-popu.x[1])*(popu.x[0]*popu.x[0]-popu.x[1])
    #      +(1-popu.x[0])*(1-popu.x[0]);
    #检验函数2,求全局极小值
	# func=(1+(popu.x[0]+popu.x[1]+1)*(popu.x[0]
    #      +popu.x[1]+1)*(19-14*popu.x[0]+3*popu.x[0]*popu.x[0]-14*popu.x[1]
    #      +6*popu.x[0]*popu.x[1]+3*popu.x[1]*popu.x[1]))*(30+(2*popu.x[0]-3*popu.x[1])
    #          *(2*popu.x[0]-3*popu.x[1])*(18-32*popu.x[0]+12*popu.x[0]*popu.x[0]+48*popu.x[1]
    #              -36*popu.x[0]*popu.x[1]+27*popu.x[1]*popu.x[1]));
    #检验函数3,求全局极大值
    func = 0.5 - (math.sin(math.sqrt(popu.x[0] * popu.x[0] + popu.x[1] * popu.x[1])) **2 - 0.5) / \
           ((1.0 + 0.001 * (popu.x[0] * popu.x[0] + popu.x[1] * popu.x[1]))** 2)
    return func

# 生成指定范围的随机数
def ran(minimum, maximum):
    return minimum + (maximum - minimum) * random.random()

# 爬山法（局部搜索，提升最优个体的适应度）
def power(popu):
    gg = f(popu)
    for pq in range(p):
        popu.x[pq] += 0.1
        # 超出变量上限则回退
        if popu.x[pq] > max_val[pq]:
            popu.x[pq] -= 0.1
            continue
        ff = f(popu)
        # 正向搜索（当前值更优则继续）
        if gg < ff:
            while gg < ff:
                gg = ff
                popu.x[pq] += 0.1
                if popu.x[pq] > max_val[pq]:
                    popu.x[pq] -= 0.1
                    break
                ff = f(popu)
        # 反向搜索（当前值更差则回退）
        else:
            while gg > ff:
                ff = gg
                popu.x[pq] -= 0.1
                gg = f(popu)
                if popu.x[pq] < min_val[pq]:
                    popu.x[pq] += 0.1
                    break

# 编码：将变量x的值转换为二进制染色体
def decode(population):
    for i in range(popsize):
        # 初始化染色体为0
        for j in range(lchrom * p):
            if j < len(population[i].chrom):
                population[i].chrom[j] = 0
        # 逐个变量编码
        for j in range(p):
            # 将x[j]映射到0-1023的整数（10位二进制）
            v = int((population[i].x[j] - min_val[j]) * 1023.0 / (max_val[j] - min_val[j]))
            for m in range(lchrom):
                num1 = lchrom * j + m
                if num1 >= len(population[i].chrom):
                    continue
                # 二进制位赋值
                population[i].chrom[num1] = 1 if (v % 2 == 1) else 0
                v = v // 2
                if v == 0:
                    break

# 概率翻转：根据概率返回1（翻转）或0（不翻转）
def flip(pp):
    return 1 if (random.random() < pp) else 0

# 选择操作（轮盘赌法）
def select(sumfitness, population):
    r_val = random.random() * sumfitness
    partsum = 0.0
    for i in range(popsize):
        partsum += population[i].fitness
        if partsum >= r_val:
            return i
    return popsize - 1

# 交叉+变异操作（精英保留：前8%个体不参与）
def crossmutation(population):
    nv = lchrom * p          # 染色体总长度
    per = 0.08               # 精英比例
    beg = int(popsize * per) # 非精英个体起始索引

    # 仅对非精英个体进行交叉变异
    for i in range(beg, popsize // 2):
        # 随机选择两个非精英个体
        m = beg + random.randint(0, popsize - beg - 1)
        z = beg + random.randint(0, popsize - beg - 1)
        
        # 交叉操作（单点交叉）
        if flip(pcross):
            bit = random.randint(1, nv - 1)  # 随机交叉点
            for j in range(bit, nv):
                if j >= len(population[m].chrom) or j >= len(population[z].chrom):
                    continue
                # 交换染色体位
                population[m].chrom[j], population[z].chrom[j] = population[z].chrom[j], population[m].chrom[j]
        
        # 变异操作（位翻转）
        if flip(pmutation):
            bit = random.randint(1, nv - 1)  # 随机变异点
            if bit < len(population[m].chrom):
                population[m].chrom[bit] = 1 - population[m].chrom[bit]
            if bit < len(population[z].chrom):
                population[z].chrom[bit] = 1 - population[z].chrom[bit]

# 解码：将二进制染色体转换为变量x的值，并计算适应度
def undecode(population):
    global sumfitness
    sumfitness = 0.0
    for i in range(popsize):
        for j in range(p):
            accum = 0.0
            powerof2 = 1.0
            # 二进制转十进制
            for m in range(lchrom):
                num2 = j * lchrom + m
                if num2 >= len(population[i].chrom):
                    continue
                if population[i].chrom[num2] == 1:
                    accum += powerof2
                powerof2 *= 2.0
            # 映射回变量范围
            population[i].x[j] = accum * (max_val[j] - min_val[j]) / 1023.0 + min_val[j]
        # 计算适应度
        population[i].fitness = f(population[i])
        sumfitness += population[i].fitness

# 主函数
def main():
    global p, avg, sumfitness
    # 初始化随机数种子
    random.seed(time.time())
    
    eps = 0.01    # 收敛阈值：平均适应度与最优适应度的差值小于该值则停止
    times = 0     # 迭代次数
    
    # 输入变量个数
    print("请输入优化变量的个数: ", end="")
    p = int(input())
    
    # 输入每个变量的最小值和最大值
    print("请输入每个变量的最小值和最大值（每行输入一个变量的范围，用空格分隔）:")
    min_val.clear()
    max_val.clear()
    for j in range(p):
        min_v, max_v = map(float, input().split())
        min_val.append(min_v)
        max_val.append(max_v)
    
    # 初始化种群
    population = [Individual() for _ in range(popsize)]
    sumfitness = 0.0
    for i in range(popsize):
        # 随机初始化变量x
        for j in range(p):
            population[i].x[j] = ran(min_val[j], max_val[j])
        # 计算初始适应度
        population[i].fitness = f(population[i])
        sumfitness += population[i].fitness
    avg = sumfitness / popsize  # 初始平均适应度
    
    # 种群排序（按适应度降序）
    population.sort(key=lambda ind: ind.fitness, reverse=True)
    
    print("\n开始迭代...")
    
    # 迭代直到收敛或达到最大次数
    while abs(avg - population[0].fitness) > eps:
        # 对最优个体进行爬山法局部优化
        power(population[0])
        # 编码：x -> 染色体
        decode(population)
        # 交叉变异
        crossmutation(population)
        # 解码：染色体 -> x + 计算适应度
        undecode(population)
        
        # 重新计算总适应度和平均适应度
        sumfitness = 0.0
        for i in range(popsize):
            population[i].fitness = f(population[i])
            sumfitness += population[i].fitness
        avg = sumfitness / popsize
        
        # 种群重新排序
        population.sort(key=lambda ind: ind.fitness, reverse=True)
        
        # 输出当前迭代信息
        times += 1
        print(f"第 {times} 代   最优值: {population[0].fitness:.6f}   变量: {population[0].x[0]:.6f}  {population[0].x[1]:.6f}")
        
        # 最大迭代次数限制
        if times > 200:
            print("达到最大迭代次数，提前终止！")
            break
    
    # 输出最终结果
    print("\n迭代结束！")
    print(f"最优适应度值为: {population[0].fitness:.6f}")
    print(f"对应变量值为: {population[0].x[0]:.6f}   {population[0].x[1]:.6f}")
    print(f"平均适应度: {avg:.6f}")
    print(f"迭代次数: {times}")

if __name__ == "__main__":
    main()