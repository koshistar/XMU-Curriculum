import random
import math
import sys
import argparse
from time import time

# 常量定义
maxpop = 201
maxstring = 81
n = 10  # 变量最大个数
pi = 3.141592653589793
popsize = 151  # 种群大小
lchrom = 10    # 每个变量的二进制编码长度
maxgen = 25    # 迭代次数（原代码注释说明实际用25）
pcross = 0.6   # 交叉概率
pmutation = 0.02  # 变异概率

# 全局变量
min_val = [0.0] * n  # 各变量最小值
max_val = [0.0] * n  # 各变量最大值
r = [0.0] * n
sumfitness = 0.0     # 总适应度
avg = 0.0            # 平均适应度
p = 0                # 实际变量个数

# 个体类
class Individual:
    def __init__(self):
        self.chrom = [0] * maxstring  # 染色体（二进制编码）
        self.x = [0.0] * n            # 变量实数值
        self.fitness = 0.0            # 适应度值

# 初始化种群
population = [Individual() for _ in range(maxpop)]
oldpopu = [Individual() for _ in range(maxpop)]
midst = Individual()
middle = Individual()
port = Individual()

# 适应度函数 f
def f(popu):
    func1, func2, func = 0.0, 0.0, 0.0
    y = [0.0] * n
    u, dcu, cz, b, wm = 0.0, 0.0, 0.0, 0.0, 0.0
    cx, tt = 0.0, 0.0

    # 复制变量值到y数组
    for j in range(p):
        y[j] = popu.x[j]

    # 目标函数计算逻辑
    u = pi * y[0] * 1000.0 / 60.0
    cz = 200.0 / (y[0] * y[0] * (1.0 - y[1] * y[1]) * pi)
    dcu = 980.0 / (1.26 * u * 0.8)
    wm = math.exp(0.5 * math.log(cz * cz + (u - dcu / 2.0) * (u - dcu / 2.0)))
    b = 117600.0 / (y[3] * y[2] * 1.26 * 800.0 * wm)
    cx = 0.022 - 0.006 * pi * y[0] / y[2] / b + 0.018 * y[3] * y[3] + 0.02 * 2.0 * pi / (y[2] * (1.0 - y[1]))
    tt = pi * y[0] / y[2]

    r[0] = b / tt - 0.5
    r[1] = 1.5 - b / tt

    func1 = 1.0 - cx / y[3] * wm * wm / (u * cz) - 0.1 * y[1] * y[1] * (2.0 - y[1] * y[1]) * cz * cz / (u * dcu)
    func2 = 56.0 * math.log(u) + 10.0 * math.log(980.0 * y[0] * (1.0 - y[1]) * y[2] * math.exp(0.6 * math.log(b)) / 2.0) - 26.22
    func = 100.0 * func1 - func2 + 200.0
    return func

# 另一个适应度函数 f2
def f2(popu):
    func1, func2, func = 0.0, 0.0, 0.0
    y = [0.0] * n
    u, dcu, cz, b, wm = 0.0, 0.0, 0.0, 0.0, 0.0
    cx = 0.0

    # 复制变量值到y数组
    for j in range(p):
        y[j] = popu.x[j]

    # 目标函数计算逻辑
    u = pi * y[0] * 1000.0 / 60.0
    cz = 200.0 / (y[0] * y[0] * (1.0 - y[1] * y[1]) * pi)
    dcu = 980.0 / (1.26 * u * 0.8)
    wm = math.exp(0.5 * math.log(cz * cz + (u - dcu / 2.0) * (u - dcu / 2.0)))
    b = 117600.0 / (y[3] * y[2] * 1.26 * 800.0 * wm)
    cx = 0.022 - 0.006 * pi * y[0] / y[2] / b + 0.018 * y[3] * y[3] + 0.02 * 2.0 * pi / (y[2] * (1.0 - y[1]))

    func1 = 1.0 - cx / y[3] * wm * wm / (u * cz) - 0.1 * y[1] * y[1] * (2.0 - y[1] * y[1]) * cz * cz / (u * dcu)
    func2 = 56.0 * math.log(u) + 10.0 * math.log(980.0 * y[0] * (1.0 - y[1]) * y[2] * math.exp(0.6 * math.log(b)) / 2.0)
    func = 100.0 * func1 - func2 + 500.0
    return func

# 生成指定范围的随机数
def ran(minimum, maximum):
    # Python直接用uniform生成区间内随机数，无需循环
    return random.uniform(minimum, maximum)

# 局部搜索（爬山法）：优化最优个体的变量值
def power(popu):
    gg = f(popu)
    for pq in range(p):
        popu.x[pq] += 0.1
        # 超出最大值则回退
        if popu.x[pq] > max_val[pq]:
            popu.x[pq] -= 0.1
            continue
        ff = f(popu)
        # 适应度提升则继续增加
        if gg < ff:
            while gg < ff:
                gg = ff
                popu.x[pq] += 0.1
                if popu.x[pq] > max_val[pq]:
                    popu.x[pq] -= 0.1
                    break
                ff = f(popu)
        # 适应度下降则减少
        else:
            while gg > ff:
                ff = gg
                popu.x[pq] -= 0.1
                gg = f(popu)
                if popu.x[pq] < min_val[pq]:
                    popu.x[pq] += 0.1
                    break

# 初始化种群
def initialize():
    global sumfitness, avg
    sumfitness = 0.0
    # 输入变量个数
    print("Please input the number of the variable: ", end="")
    global p
    p = int(input())
    # 输入每个变量的最小值和最大值
    print("Please input minimum and maximum of each variable:")
    for j in range(p):
        min_val[j], max_val[j] = map(float, input().split())

    # 初始化种群中每个个体的变量和适应度
    for i in range(1, popsize):  # 索引从1开始（兼容原C++逻辑）
        for j in range(p):
            population[i].x[j] = ran(min_val[j], max_val[j])
        population[i].fitness = f(population[i])
        sumfitness += population[i].fitness
    avg = sumfitness / (popsize - 1)
    print(f"\naverage of the sumfitness is {avg:.6f}")

# 适应度平移（处理负值，保证轮盘赌选择有效）
def change():
    global sumfitness, avg
    # 找到最小适应度
    ff = population[1].fitness
    for i in range(2, popsize):
        if ff > population[i].fitness:
            ff = population[i].fitness
    sumfitness = 0.0
    # 所有适应度减去最小值（平移为非负）
    for i in range(1, popsize):
        population[i].fitness -= ff
        sumfitness += population[i].fitness
    avg = sumfitness / (popsize - 1)

# 编码：实数变量 → 二进制染色体
def decode():
    for i in range(1, popsize):
        # 初始化染色体为0
        for j in range(lchrom * p):
            population[i].chrom[j] = 0

        # 每个变量编码为lchrom位二进制
        for j in range(p):
            # 将实数映射到0~1023（10位二进制最大值）
            v = int((population[i].x[j] - min_val[j]) * 1023.0 / (max_val[j] - min_val[j]))
            for m in range(lchrom):
                num1 = lchrom * j + m  # 索引从0开始
                if v % 2 == 1:
                    population[i].chrom[num1] = 1
                v = v // 2

# 复制个体
def cop(newpop, oldpop):
    # 复制变量和适应度
    for j in range(p):
        newpop.x[j] = oldpop.x[j]
        newpop.fitness = oldpop.fitness
    # 复制染色体
    for i in range(lchrom * p):
        newpop.chrom[i] = oldpop.chrom[i]

# 按适应度降序排序种群
def sort_population():
    # 冒泡排序
    for j in range(1, popsize - 1):
        for i in range(1, popsize - j):
            if population[i].fitness < population[i + 1].fitness:
                # 交换两个个体
                midst = population[i]
                population[i] = population[i + 1]
                population[i + 1] = midst

# 随机翻转（判断交叉/变异是否执行）
def flip(pp):
    return 1 if random.random() < pp else 0

# 选择操作（轮盘赌法）
def select():
    r_val = random.uniform(0, sumfitness)
    partsum = 0.0
    for i in range(1, popsize):
        partsum += population[i].fitness
        if partsum >= r_val:
            return i
    return popsize - 1

# 交叉变异操作
def crossmutation():
    nv = lchrom * p
    # 第一步：选择个体到旧种群
    for i in range(1, popsize):
        a = select()
        cop(oldpopu[i], population[a])

    # 第二步：复制旧种群到新种群
    for i in range(1, popsize):
        cop(population[i], oldpopu[i])

    # 第三步：交叉+变异
    for i in range(1, popsize // 2):
        # 随机选两个个体
        m = random.randint(1, popsize - 1)
        z = random.randint(1, popsize - 1)

        # 交叉操作
        if flip(pcross):
            bit = random.randint(0, nv - 1)  # 交叉点
            for j in range(bit, nv):
                # 交换染色体位
                t = population[m].chrom[j]
                population[m].chrom[j] = population[z].chrom[j]
                population[z].chrom[j] = t

        # 变异操作
        if flip(pmutation):
            bit = random.randint(0, nv - 1)  # 变异位
            population[m].chrom[bit] = 1 - population[m].chrom[bit]
            population[z].chrom[bit] = 1 - population[z].chrom[bit]

# 解码：二进制染色体 → 实数变量
def undecode():
    global sumfitness
    sumfitness = 0.0
    for i in range(1, popsize):
        for j in range(p):
            accum = 0.0
            powerof2 = 1.0
            # 二进制转十进制
            for m in range(lchrom):
                num2 = j * lchrom + m  # 索引从0开始
                if population[i].chrom[num2] == 1:
                    accum += powerof2
                powerof2 *= 2.0
            # 映射回原变量范围
            population[i].x[j] = accum * (max_val[j] - min_val[j]) / 1023.0 + min_val[j]
            print(f"{population[i].x[j]:.6f} ", end="")
        print()
        # 重新计算适应度
        population[i].fitness = f(population[i])
        sumfitness += population[i].fitness

# 主函数
def main():
    # 解析命令行参数（可选：调整种群大小、迭代次数等）
    parser = argparse.ArgumentParser(description="Genetic Algorithm (GA) for multi-variable optimization")
    parser.add_argument("--popsize", type=int, default=151, help="Population size (default: 151)")
    parser.add_argument("--maxgen", type=int, default=25, help="Max iteration generations (default: 25)")
    parser.add_argument("--pcross", type=float, default=0.6, help="Crossover probability (default: 0.6)")
    parser.add_argument("--pmutation", type=float, default=0.02, help="Mutation probability (default: 0.02)")
    args = parser.parse_args()

    # 覆盖全局参数（如果命令行指定）
    global popsize, maxgen, pcross, pmutation
    popsize = args.popsize
    maxgen = args.maxgen
    pcross = args.pcross
    pmutation = args.pmutation

    # 初始化随机种子
    random.seed(time())

    # GA核心流程
    initialize()       # 初始化种群
    change()           # 适应度平移
    sort_population()  # 种群排序
    power(population[1])# 局部优化最优个体
    decode()           # 编码

    cop(port, population[1])  # 保存初始最优个体

    # 迭代进化
    for gen in range(1, maxgen + 1):
        print(f"\n=== Generation {gen} ===")
        crossmutation()  # 交叉变异
        undecode()       # 解码+重新计算适应度
        sort_population()# 排序
        cop(population[popsize - 1], port)  # 保留精英
        change()         # 适应度平移

    # 最终优化
    sort_population()
    power(population[1])

    # 输出最优解
    print("\n=== Best solution ===")
    for q in range(p):
        print(f"{population[1].x[q]:.6f}\t", end="")
    g = f2(population[1])
    print(f"\nFitness f2: {g:.6f}")

    # 测试固定点
    population[1].x[0] = 1.4891
    population[1].x[1] = 0.2942
    population[1].x[2] = 13.0
    population[1].x[3] = 1.1591
    g = f2(population[1])
    print(f"Test point f2: {g:.6f}")

if __name__ == "__main__":
    main()