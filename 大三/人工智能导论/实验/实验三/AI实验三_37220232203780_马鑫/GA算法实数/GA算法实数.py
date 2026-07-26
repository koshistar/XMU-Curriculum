import random
import math

# 全局参数定义
maxpop = 201
maxstring = 81
n = 10
pi = 3.141592653589793
popsize = 151  # 种群规模
maxgen = 15    # 原参数
pcross = 0.6   # 交叉概率
pmutation = 0.02  # 变异概率
sumfitness = 0.0  # 总适应度
avg = 0.0         # 平均适应度
p = 0             # 变量个数
min_val = []      # 每个变量的最小值
max_val = []      # 每个变量的最大值
population = []   # 种群（存储Individual实例）
midst = None      # 临时个体（交换用）
mid = None        # 精英个体存储


# 个体类
class Individual:
    def __init__(self):
        self.chrom = [0.0] * maxstring  # 染色体（实数编码）
        self.x = [0.0] * n              # 变量值
        self.fitness = 0.0              # 适应度


# 适应度函数：Six-Hump Camel Back函数（求最小值→转最大值问题）
def calculate_fitness(popu):
    func = 0.0
    y = popu.x.copy()
    
    # Six-Hump Camel Back函数（核心是双变量，p=2时生效）
    func = (4 - 2.1 * y[0]**2 + y[0]**4 / 3.0) * y[0]**2
    func += y[0] * y[1] + (4 * y[1]**2 - 4) * y[1]**2
    func = -func + 10000.0  # 最小值问题转最大值问题（方便GA求解）
    return func


# 爬山法（局部优化，power函数）
def hill_climbing(popu):
    g = calculate_fitness(popu)
    for j in range(p):
        popu.x[j] -= 0.01
        t1 = calculate_fitness(popu)
        
        # 变量值越下界，重置并跳过
        if popu.x[j] < min_val[j]:
            popu.x[j] += 0.1
            continue
        
        # 向下搜索更优解
        if t1 < g:
            while t1 < g:
                g = t1
                popu.x[j] -= 0.01
                if popu.x[j] < min_val[j]:
                    popu.x[j] += 0.1
                    break
                t1 = calculate_fitness(popu)
        # 向上搜索更优解
        else:
            while t1 > g:
                t1 = g
                popu.x[j] += 0.01
                if popu.x[j] > max_val[j]:
                    popu.x[j] -= 0.01
                    break
                g = calculate_fitness(popu)


# 随机数生成函数：生成[minimum, maximum]范围内的随机数
def ran(minimum, maximum):
    return minimum + (maximum - minimum) * (random.randint(0, 10000) / 10000.0) * 10.0


# 初始化种群
def initialize():
    global sumfitness, avg, population, min_val, max_val, p
    sumfitness = 0.0
    # 输入变量个数
    p = int(input("Please input the number of the variable: "))
    # 输入每个变量的最小值和最大值
    print("Please input minimum and maximum of each variable:")
    min_val = []
    max_val = []
    for j in range(p):
        min_max = input().split()
        min_val.append(float(min_max[0]))
        max_val.append(float(min_max[1]))
    
    # 初始化种群个体
    population = []
    for i in range(popsize - 1):  # 种群规模popsize-1（原C++逻辑）
        ind = Individual()
        for j in range(p):
            # 实数编码：染色体分两部分，变量值=两部分之和
            chrom_idx1 = j * 2
            chrom_idx2 = j * 2 + 1
            ind.chrom[chrom_idx1] = ran(min_val[j]/2, max_val[j]/2)
            ind.chrom[chrom_idx2] = ran(min_val[j]/2, max_val[j]/2)
            ind.x[j] = ind.chrom[chrom_idx1] + ind.chrom[chrom_idx2]
        # 计算适应度
        ind.fitness = calculate_fitness(ind)
        sumfitness += ind.fitness
        population.append(ind)
    
    # 计算平均适应度
    avg = sumfitness / (popsize - 1)
    print(f"\naverage of the sumfitness is {avg:.4f}")


# 复制个体（新个体=旧个体）
def copy_individual(newpop, oldpop):
    # 复制变量值和适应度
    for j in range(p):
        newpop.x[j] = oldpop.x[j]
    newpop.fitness = oldpop.fitness
    # 复制染色体
    for i in range(2 * p):
        newpop.chrom[i] = oldpop.chrom[i]


# 种群排序：按适应度降序（适应度越高越优）
def sort_population():
    global population
    for j in range(len(population) - 1):
        for i in range(len(population) - j - 1):
            if population[i].fitness < population[i+1].fitness:
                # 交换个体
                population[i], population[i+1] = population[i+1], population[i]


# 概率判断函数：判断是否执行交叉/变异
def flip(pp):
    f_val = random.random()  # 生成[0,1)随机数
    return 1 if f_val < pp else 0


# 轮盘赌选择
def select():
    global sumfitness
    r_val = (random.randint(0, 10000) / 10000.0) * sumfitness
    partsum = 0.0
    for i in range(len(population)):
        partsum += population[i].fitness
        if partsum >= r_val:
            return i
    return len(population) - 1


# 交叉和变异操作
def crossmutation():
    global population
    nv = 2 * p  # 染色体总长度（每个变量对应2个染色体位）
    
    # 1. 选择操作：生成新种群
    new_population = []
    for i in range(len(population)):
        a = select()
        ind = Individual()
        copy_individual(ind, population[a])
        new_population.append(ind)
    population = new_population
    
    # 2. 交叉操作
    for i in range(len(population) // 2):
        # 随机选两个个体
        m = random.randint(0, len(population)-1)
        z = random.randint(0, len(population)-1)
        
        # 交叉：概率pcross
        if flip(pcross):
            bit = random.randint(0, nv-1)  # 交叉起始位
            for j in range(bit, nv):
                # 交换染色体
                population[m].chrom[j], population[z].chrom[j] = population[z].chrom[j], population[m].chrom[j]
        
        # 变异：概率pmutation
        if flip(pmutation):
            bit = random.randint(0, nv-1)
            if bit % 2 == 1:  # 确保是偶位（原C++逻辑）
                bit = (bit + 1) % nv
            # 随机赋值0或1（变异操作）
            population[m].chrom[bit] = 0.0 if random.randint(0,1) == 0 else 1.0
            population[z].chrom[bit] = 0.0 if random.randint(0,1) == 0 else 1.0


# 主函数
def main():
    global mid, sumfitness, avg
    # 初始化随机种子
    random.seed()
    
    # 1. 初始化种群
    initialize()
    # 2. 初始排序
    sort_population()
    # 3. 对最优个体做爬山法优化
    hill_climbing(population[0])
    # 4. 保存精英个体（精英保留）
    mid = Individual()
    copy_individual(mid, population[0])
    
    # 迭代参数
    iter_num = 0
    max_iter = 100  # 最大迭代次数
    
    # 遗传算法主循环
    while True:
        # 交叉变异
        crossmutation()
        # 种群排序
        sort_population()
        # 精英保留：把最优旧个体放回种群末尾
        copy_individual(population[-1], mid)
        # 对新最优个体做爬山法
        hill_climbing(population[0])
        # 更新精英个体
        copy_individual(mid, population[0])
        
        # 重新计算总适应度和平均适应度
        sumfitness = 0.0
        for ind in population:
            sumfitness += ind.fitness
        avg = sumfitness / len(population)
        
        # 迭代计数
        iter_num += 1
        # 每5代打印一次最优适应度
        if iter_num % 5 == 0:
            print(f"迭代 {iter_num} 代   当前最优适应度: {population[0].fitness:.4f}")
        
        # 终止条件：最优适应度与平均适应度差值<0.1 或 迭代次数达上限
        if (population[0].fitness - avg < 0.1) or (iter_num >= max_iter):
            break
    
    # 最终排序+爬山法优化
    sort_population()
    hill_climbing(population[0])
    
    # 输出结果
    print("\n优化结果：")
    print("最优变量值:")
    for q in range(p):
        print(f"{population[0].x[q]:.4f}\t", end="")
    # 还原原函数值（最小值）：原适应度= -原函数值 + 10000 → 原函数值= 10000 - 适应度
    original_func_val = 10000.0 - calculate_fitness(population[0])
    print(f"\n原函数值（最小值）: {original_func_val:.4f}")
    
    # 再次爬山法优化并输出
    hill_climbing(population[0])
    original_func_val_opt = 10000.0 - calculate_fitness(population[0])
    print(f"爬山法优化后原函数值: {original_func_val_opt:.4f}")
    
    # 暂停
    input("\nPress Enter to exit...")


if __name__ == "__main__":
    main()