import math
import random

# ============================================================
# 原始数据
# ============================================================

x = [
    [0.05,2.32,0.21,0.073,26,340,690,8,18],
    [0.05,2.25,0.19,0.072,30,340,695,8,18],
    [0.05,2.30,0.25,0.080,29,300,700,9,17],
    [0.04,2.18,0.20,0.080,27,350,700,8,16],
    [0.07,2.27,0.25,0.084,30,320,700,9,17],
    [0.05,2.42,0.22,0.084,28,310,700,7,17],
    [0.06,2.23,0.22,0.084,30,320,700,9,17],
    [0.06,2.43,0.26,0.085,27,330,695,8,17],
    [0.05,2.00,0.21,0.073,27,330,695,8,17],
    [0.04,2.55,0.20,0.087,28,360,700,7,15],
    [0.05,2.10,0.17,0.078,28,350,700,9,14],
    [0.04,2.27,0.25,0.071,30,305,700,9,17],
    [0.07,2.38,0.22,0.079,27,350,695,8,16],
    [0.05,2.31,0.32,0.062,28,360,700,7,15],
    [0.05,2.29,0.21,0.082,30,340,695,8,18],
    [0.05,2.26,0.23,0.084,27,320,695,9,16],
    [0.05,2.41,0.23,0.081,26,345,700,9,16],
    [0.06,2.41,0.22,0.083,27,340,690,8,16],
    [0.05,2.27,0.20,0.069,26,370,700,6,18],
    [0.05,2.18,0.20,0.084,27,350,700,8,16],
    [0.06,2.34,0.24,0.079,29,320,700,8,18],
    [0.05,2.33,0.22,0.078,28,350,700,9,14],
    [0.05,2.32,0.22,0.071,27,310,700,9,18],
    [0.07,2.04,0.24,0.068,28,330,690,8,18],
    [0.04,2.60,0.20,0.088,28,330,700,5,17],
    [0.05,2.51,0.24,0.087,27,360,700,9,16],
    [0.04,2.18,0.23,0.083,27,380,695,9,16],
    [0.06,2.21,0.21,0.081,27,350,695,8,16],
    [0.06,2.20,0.22,0.060,27,320,690,8,18],
    [0.07,2.39,0.30,0.085,26,365,695,9,17],
    [0.06,2.03,0.21,0.077,30,300,690,8,16],
    [0.04,2.47,0.28,0.074,30,305,700,9,17],
    [0.05,2.31,0.22,0.074,29,290,695,8,18],
    [0.07,2.41,0.22,0.081,26,320,695,8,15],
    [0.04,2.37,0.37,0.086,28,320,700,9,16],
    [0.06,2.60,0.22,0.087,33,290,695,8,17],
    [0.06,2.13,0.23,0.077,28,330,700,6,18],
    [0.06,2.25,0.22,0.085,29,335,700,9,17],
    [0.06,2.36,0.23,0.080,28,330,700,6,18],
    [0.08,2.33,0.26,0.082,28,325,690,8,20],
    [0.07,2.18,0.26,0.079,29,320,695,7,16],
    [0.07,2.59,0.24,0.089,29,330,690,8,18],
    [0.08,2.50,0.21,0.085,30,220,690,9,17],
    [0.08,2.65,0.25,0.087,26,350,695,8,19],
    [0.05,2.35,0.26,0.082,29,310,695,9,18],
    [0.07,2.02,0.30,0.070,28,360,700,8,16],
    [0.05,2.64,0.24,0.078,28,390,700,9,15],
    [0.07,2.22,0.32,0.080,28,280,700,6,15],
    [0.08,2.44,0.25,0.081,28,300,700,9,17],
    [0.08,2.40,0.29,0.075,29,360,695,8,17],
    [0.04,2.03,0.18,0.080,33,265,695,8,16],
    [0.06,2.44,0.22,0.073,29,320,700,9,17],
    [0.09,2.09,0.20,0.072,28,330,700,6,18],
    [0.07,2.53,0.25,0.085,29,300,700,9,15],
    [0.07,2.54,0.23,0.088,27,380,690,8,17],
    [0.07,2.45,0.24,0.071,28,340,700,7,15],
    [0.08,2.39,0.24,0.086,29,290,695,8,18],
    [0.06,2.58,0.23,0.087,28,355,695,6,16],
    [0.06,2.48,0.23,0.074,29,310,695,8,18],
    [0.06,2.13,0.19,0.073,30,280,695,7,16],
    [0.07,2.39,0.25,0.082,31,280,695,6,17],
    [0.07,2.35,0.20,0.090,30,695,8,17,0],
    [0.10,2.68,0.30,0.087,27,300,700,9,17],
    [0.07,2.40,0.26,0.082,27,310,700,9,18],
    [0.05,2.23,0.21,0.081,29,360,695,8,17],
    [0,0,0,0,0,0,0,0,0],  
]

y = [
    [435],[442],[450],[453],[455],[456],[460],[461],[463],[467],
    [467],[468],[470],[471],[472],[475],[476],[478],[478],[484],
    [485],[486],[487],[488],[489],[491],[492],[493],[496],[496],
    [497],[498],[498],[499],[499],[520],[521],[524],[525],[526],
    [527],[528],[528],[529],[533],[534],[535],[536],[536],[538],
    [539],[539],[540],[543],[543],[546],[548],[548],[550],[551],
    [551],[553],[553],[554],[558],[567]
]

# ============================================================
# 参数定义
# ============================================================

nReportErrors = 100       # 误差报告频率
MonitorError = False       # 表示为真
error1 = 0.0              # 最新的误差平方之和

# 数组声明
out0   = [[0.0]*9   for _ in range(68)]  # 输入层（标准化后）
out1   = [[0.0]*5   for _ in range(66)]  # 隐含层,[模式数][隐含]
delta1 = [[0.0]*5   for _ in range(66)]  # 隐含层中的误差修正量,[模式数][隐含]
delw1  = [[0.0]*10  for _ in range(6)]   # 输入与隐含层之间权值的变化,[隐含+1][输入]
w1     = [[0.0]*10  for _ in range(6)]   # 输入与隐含层之间的权值,[隐含+1][输入]
out2   = [[0.0]*1   for _ in range(66)]  # 输出层
delta2 = [[0.0]*1   for _ in range(66)]  # 输出层中的误差修正量
delw2  = [[0.0]*6   for _ in range(1)]   # 隐含与输出层之间权值的变化,[输出][隐含+1]
w2     = [[0.0]*6   for _ in range(1)]   # 隐含与输出层间的权值,[输出][隐含+1]
target = [[0.0]*1   for _ in range(68)]  # 目标输出值（标准化后）

ss       = [0.0]*9
average  = [0.0]*9
sss      = [0.0]*9
fangcha  = [0.0]*9
ssy      = [0.0]*1
averagey = [0.0]*1
sssy     = [0.0]*1
fangchay = [0.0]*1

min_val = 1000.0
max_val = -1000.0

nPatterns    = 66      # 模式个数
nInputNodes  = 9       # 输入节点个数
nHiddenNodes = 5       # 隐含层节点个数
nOutputNodes = 1       # 输出层节点个数
nIterations  = 500000  # 迭代次数
ErrorLevel   = 0.002   # 满意的误差水平
wmax         = 0.3     # 权值最大范围
scale        = 32767.0 # 随机数缩放
eta          = 0.35    # 默认的学习速率
alpha        = 0.1     # 默认的势态因子

# ============================================================
# 将输入与输出值标准化
# ============================================================

# 1.对输入值标准化
# 求均值
for i in range(nInputNodes):
    ss[i] = 0.0
    for j in range(nPatterns):
        ss[i] = ss[i] + x[j][i]
    average[i] = ss[i] / nPatterns
    print("{:.6f} ".format(average[i]), end="")
print()

# 求方差（标准差）
for i in range(nInputNodes):
    sss[i] = 0.0
    for j in range(nPatterns):
        sss[i] = sss[i] + (x[j][i] - average[i]) * (x[j][i] - average[i])
    fangcha[i] = math.sqrt(sss[i] / (nPatterns - 1))

# 求Z-score标准化后的输入值，并记录每列min/max
for i in range(nInputNodes):
    min_val = 1000.0
    max_val = -1000.0
    for j in range(nPatterns):
        out0[j][i] = (x[j][i] - average[i]) / fangcha[i]
        if out0[j][i] <= min_val:
            min_val = out0[j][i]
        if out0[j][i] >= max_val:
            max_val = out0[j][i]
    out0[nPatterns][i]     = min_val     # 第67行存min
    out0[nPatterns + 1][i] = max_val     # 第68行存max

# 再做 [0.1, 0.9] 归一化
for i in range(nInputNodes):
    for j in range(nPatterns):
        out0[j][i] = (out0[j][i] - out0[nPatterns][i]) / (out0[nPatterns + 1][i] - out0[nPatterns][i]) * 0.8 + 0.1

# 2.对输出值标准化
# 求均值
for i in range(nOutputNodes):
    ssy[i] = 0.0
    for j in range(nPatterns):
        ssy[i] = ssy[i] + y[j][i]
    averagey[i] = ssy[i] / nPatterns
    print("{:.6f} ".format(averagey[i]), end="")
print()

# 求方差（标准差）
for i in range(nOutputNodes):
    sssy[i] = 0.0
    for j in range(nPatterns):
        sssy[i] = sssy[i] + (y[j][i] - averagey[i]) * (y[j][i] - averagey[i])
    fangchay[i] = math.sqrt(sssy[i] / (nPatterns - 1))

# 求Z-score标准化后的输出值，并记录min/max
for i in range(nOutputNodes):
    min_val = 1000.0
    max_val = -1000.0
    for j in range(nPatterns):
        target[j][i] = (y[j][i] - averagey[i]) / fangchay[i]
        if target[j][i] <= min_val:
            min_val = target[j][i]
        if target[j][i] >= max_val:
            max_val = target[j][i]
    target[nPatterns][i]     = min_val
    target[nPatterns + 1][i] = max_val

# 再做 [0.1, 0.9] 归一化
for i in range(nOutputNodes):
    for j in range(nPatterns):
        target[j][i] = (target[j][i] - target[nPatterns][i]) / (target[nPatterns + 1][i] - target[nPatterns][i]) * 0.8 + 0.1

print()

# ============================================================
# 初始化权值（随机生成）
# ============================================================

# 输入-隐含层权值: w1[0..5][0..9]（含偏置项，第10列为偏置）
for i in range(nHiddenNodes + 1):
    for j in range(nInputNodes + 1):
        frand = random.randint(0, 32767)
        w1[i][j] = wmax * (1.0 - 2 * frand / scale)
        delw1[i][j] = 0.0
print()

# 隐含-输出层权值: w2[0][0..5]（含偏置项，第6列为偏置）
for i in range(nOutputNodes):
    for j in range(nHiddenNodes + 1):
        frand = random.randint(0, 32767)
        w2[i][j] = wmax * (1.0 - 2 * frand / scale)
        delw2[i][j] = 0.0
print()

# ============================================================
# 开始迭代循环
# ============================================================

print("正在计算......", end="", flush=True)

qq = 0
for qq in range(nIterations + 1):
    # ---------- 前向传播 + 误差计算 ----------
    for p in range(nPatterns):
        # 隐含层的输出值
        for h in range(nHiddenNodes):
            s = w1[h][nInputNodes]  # 偏置项
            for i in range(nInputNodes):
                s = s + w1[h][i] * out0[p][i]
            out1[p][h] = 1.0 / (1.0 + math.exp(-s))

        # 输出层的输出值
        for j in range(nOutputNodes):
            s = w2[j][nHiddenNodes]  # 偏置项
            for h in range(nHiddenNodes):
                s = s + w2[j][h] * out1[p][h]
            out2[p][j] = 1.0 / (1.0 + math.exp(-s))

        # 输出层的误差
        for j in range(nOutputNodes):
            delta2[p][j] = (target[p][j] - out2[p][j]) * out2[p][j] * (1.0 - out2[p][j])

        # 隐含层的误差
        for h in range(nHiddenNodes):
            s = 0.0
            for j in range(nOutputNodes):
                s = s + delta2[p][j] * w2[j][h]
            delta1[p][h] = s * out1[p][h] * (1.0 - out1[p][h])

    # ---------- 反向传播：调整隐含-输出层间的权值 ----------
    for j in range(nOutputNodes):
        # 偏置项
        s = 0.0
        for p in range(nPatterns):
            s = s + delta2[p][j]
        dw = eta * s + alpha * delw2[j][nHiddenNodes]
        w2[j][nHiddenNodes] += dw
        delw2[j][nHiddenNodes] = dw

        # 权值项
        for h in range(nHiddenNodes):
            s = 0.0
            for p in range(nPatterns):
                s = s + delta2[p][j] * out1[p][h]
            dw = eta * s + alpha * delw2[j][h]
            w2[j][h] = w2[j][h] + dw
            delw2[j][h] = dw

    # ---------- 反向传播：调整输入-隐含层的权值 ----------
    for h in range(nHiddenNodes):
        # 偏置项
        s = 0.0
        for p in range(nPatterns):
            s = s + delta1[p][h]
        dw = eta * s + alpha * delw1[h][nInputNodes]
        w1[h][nInputNodes] += dw
        delw1[h][nInputNodes] = dw

        # 权值项
        for i in range(nInputNodes):
            s = 0.0
            for p in range(nPatterns):
                s = s + delta1[p][h] * out0[p][i]
            dw = eta * s + alpha * delw1[h][i]
            w1[h][i] = w1[h][i] + dw
            delw1[h][i] = dw

    # ---------- 计算均方根误差 ----------
    if MonitorError or (qq % nReportErrors == 0):
        error1 = 0.0
        for p in range(nPatterns):
            for j in range(nOutputNodes):
                temp = target[p][j] - out2[p][j]
                error1 = error1 + temp * temp
        # 所有模式每个节点的平均误差
        error1 = error1 / (nPatterns * nOutputNodes)
        MonitorError = False
        if error1 < ErrorLevel:
            print("\n最终平均误差为:{}".format(error1))
            break

# ============================================================
# 迭代循环结束，输出结果
# ============================================================

# 写入结果文件
my_file = open("BP算法.dat", "w", encoding="utf-8")

error1 = 0.0
for p in range(nPatterns):
    for j in range(nOutputNodes):
        relative_err = (target[p][j] - out2[p][j]) / target[p][j] * 100
        line = "{} {} {} {}\n".format(p, out2[p][j], target[p][j], relative_err)
        my_file.write(line)
        print("out2[{}][{}]={}  target[{}][{}]={}".format(p, j, out2[p][j], p, j, target[p][j]))
        temp = target[p][j] - out2[p][j]
        error1 = error1 + temp * temp

error1 = error1 / (nPatterns * nOutputNodes)

# 输出输入-隐含层权值
for i in range(nHiddenNodes + 1):
    for j in range(nInputNodes + 1):
        print("w1[{}][{}] {}  ".format(i, j, w1[i][j]), end="")
    print()

# 输出隐含-输出层权值
for i in range(nOutputNodes):
    for j in range(nHiddenNodes + 1):
        print("w2[{}][{}] {}  ".format(i, j, w2[i][j]), end="")
    print()

print("循环次数为:{}".format(qq))
print("循环已经结束")
print("最终平均误差为:{}".format(error1))

my_file.close()

# 防止运行结束闪退（交互式环境下可忽略）
try:
    input("\n按 Enter 键退出...")
except EOFError:
    pass
