import math
import random
import os

InputNo = 3            # 输入节点个数
OutputnNo = 3          # 输出节点个数
PatternNo = 358        # 模式个数（训练样本数）
ErrorLevelValue = 0.005 # 满意的误差水平
wmaxValue = 0.1        # 初始权值最大范围
HiddenNo = 10          # 隐含层节点个数
IterationsNo = 1000000 # 最大迭代次数
etaValue = 0.1         # 默认的学习速率
alphaValue = 0.1       # 默认的势态因子（动量项）


def main():
    # ---- 基础变量 ----
    nReportErrors = 100   # 误差报告频率
    MonitorError = False   # 表示为真
    error1 = 0.0           # 最新的误差平方之和

    nPatterns = PatternNo
    nInputNodes = InputNo
    nHiddenNodes = HiddenNo
    nOutputNodes = OutputnNo
    nIterations = IterationsNo
    scale = 32767.0
    errorLimit = ErrorLevelValue
    wmax = wmaxValue
    eta = etaValue
    alpha = alphaValue

    # 模式相关: [模式数][节点数]
    out1   = [[0.0] * nHiddenNodes for _ in range(nPatterns)]   # 隐含层输出
    delta1 = [[0.0] * nHiddenNodes for _ in range(nPatterns)]   # 隐含层误差修正量
    out2   = [[0.0] * nOutputNodes for _ in range(nPatterns)]   # 输出层输出
    delta2 = [[0.0] * nOutputNodes for _ in range(nPatterns)]   # 输出层误差修正量
    x      = [[0.0] * nInputNodes  for _ in range(nPatterns)]   # 历史数据输入值
    y      = [[0.0] * nOutputNodes for _ in range(nPatterns)]   # 历史数据输出值
    out0   = [[0.0] * nInputNodes  for _ in range(nPatterns + 2)]
    target = [[0.0] * nOutputNodes for _ in range(nPatterns + 2)]

    # 权值: w1[隐含+1][输入+1], w2[输出][隐含+1]
    w1   = [[0.0] * (nInputNodes + 1)  for _ in range(nHiddenNodes + 1)]
    delw1 = [[0.0] * (nInputNodes + 1) for _ in range(nHiddenNodes + 1)]
    w2   = [[0.0] * (nHiddenNodes + 1) for _ in range(nOutputNodes)]
    delw2 = [[0.0] * (nHiddenNodes + 1) for _ in range(nOutputNodes)]

    # 标准化统计量
    ss       = [0.0] * nInputNodes
    average  = [0.0] * nInputNodes
    sss      = [0.0] * nInputNodes
    fangcha  = [0.0] * nInputNodes
    ssy      = [0.0] * nOutputNodes
    averagey = [0.0] * nOutputNodes
    sssy     = [0.0] * nOutputNodes
    fangchay = [0.0] * nOutputNodes

    # ================================================================
    # 1. 将节点数写到数据文件中去
    # ================================================================
    with open("节点数.dat", "w", encoding="utf-8") as f:
        f.write("{}\n{}\n{}\n".format(nInputNodes, nHiddenNodes, nOutputNodes))

    # ================================================================
    # 2. 神经网络的训练阶段：读取历史数据
    # ================================================================
    data_filename = "S1期鼓风操作数据.dat"
    if not os.path.exists(data_filename):
        print("错误：找不到文件 '{}'，请将其放在与py脚本同级的目录下！".format(data_filename))
        input("按 Enter 键退出...")
        return -1

    with open(data_filename, "r", encoding="utf-8") as f:
        # 将文件中所有数值读取到一个扁平列表
        all_values = f.read().split()

    val_ptr = 0
    for i in range(nPatterns):
        for j in range(nInputNodes + nOutputNodes):
            if val_ptr >= len(all_values):
                print("警告：数据文件中数值不足，只读取了 {} 个样本".format(i))
                nPatterns = i
                break
            val = float(all_values[val_ptr])
            val_ptr += 1
            if j >= nInputNodes:
                y[i][j - nInputNodes] = val
            else:
                x[i][j] = val
        else:
            continue
        break

    # ================================================================
    # 3. 数据标准化处理
    # ================================================================

    # ---- 3.1 对输入值标准化 (Z-score -> [0.1, 0.9]) ----
    # 求均值
    for i in range(nInputNodes):
        ss[i] = 0.0
        for j in range(nPatterns):
            ss[i] += x[j][i]
        average[i] = ss[i] / nPatterns

    # 求标准差
    for i in range(nInputNodes):
        sss[i] = 0.0
        for j in range(nPatterns):
            sss[i] += (x[j][i] - average[i]) * (x[j][i] - average[i])
        fangcha[i] = math.sqrt(sss[i] / (nPatterns - 1))

    # Z-score 变换，并记录每列 min/max
    for i in range(nInputNodes):
        min_val = 1000.0
        max_val = -1000.0
        for j in range(nPatterns):
            out0[j][i] = (x[j][i] - average[i]) / fangcha[i]
            if out0[j][i] <= min_val:
                min_val = out0[j][i]
            if out0[j][i] >= max_val:
                max_val = out0[j][i]
        out0[nPatterns][i] = min_val       # 存放最小值
        out0[nPatterns + 1][i] = max_val   # 存放最大值

    # 映射到 [0.1, 0.9]
    for i in range(nInputNodes):
        for j in range(nPatterns):
            out0[j][i] = (out0[j][i] - out0[nPatterns][i]) / \
                         (out0[nPatterns + 1][i] - out0[nPatterns][i]) * 0.8 + 0.1

    # 将输入标准化参数写入文件
    with open("输入自标准化.dat", "w", encoding="utf-8") as f:
        for i in range(nInputNodes):
            f.write("{}\n{}\n{}\n{}\n".format(
                average[i], fangcha[i],
                out0[nPatterns][i], out0[nPatterns + 1][i]
            ))

    # ---- 3.2 对输出值标准化 (Z-score -> [0.1, 0.9]) ----
    # 求均值
    for i in range(nOutputNodes):
        ssy[i] = 0.0
        for j in range(nPatterns):
            ssy[i] += y[j][i]
        averagey[i] = ssy[i] / nPatterns
        print("输出值平均值为: {}".format(averagey[i]), end=" ")
    print()

    # 求标准差
    for i in range(nOutputNodes):
        sssy[i] = 0.0
        for j in range(nPatterns):
            sssy[i] += (y[j][i] - averagey[i]) * (y[j][i] - averagey[i])
        fangchay[i] = math.sqrt(sssy[i] / (nPatterns - 1))

    # Z-score 变换，并记录每列 min/max
    for i in range(nOutputNodes):
        min_val = 1000.0
        max_val = -1000.0
        for j in range(nPatterns):
            target[j][i] = (y[j][i] - averagey[i]) / fangchay[i]
            if target[j][i] <= min_val:
                min_val = target[j][i]
            if target[j][i] >= max_val:
                max_val = target[j][i]
        target[nPatterns][i] = min_val
        target[nPatterns + 1][i] = max_val

    # 映射到 [0.1, 0.9]
    for i in range(nOutputNodes):
        for j in range(nPatterns):
            target[j][i] = (target[j][i] - target[nPatterns][i]) / \
                           (target[nPatterns + 1][i] - target[nPatterns][i]) * 0.8 + 0.1

    # 将输出标准化参数写入文件
    with open("输出自标准化.dat", "w", encoding="utf-8") as f:
        for i in range(nOutputNodes):
            f.write("{}\n{}\n{}\n{}\n".format(
                averagey[i], fangchay[i],
                target[nPatterns][i], target[nPatterns + 1][i]
            ))

    # ================================================================
    # 4. 权值初始化
    # ================================================================
    for i in range(nHiddenNodes + 1):
        for j in range(nInputNodes + 1):
            frand = random.randint(0, 32767)
            w1[i][j] = wmax * (1.0 - 2.0 * frand / scale)
            delw1[i][j] = 0.0

    for i in range(nOutputNodes):
        for j in range(nHiddenNodes + 1):
            frand = random.randint(0, 32767)
            w2[i][j] = wmax * (1.0 - 2.0 * frand / scale)
            delw2[i][j] = 0.0

    # ================================================================
    # 5. 开始迭代循环
    # ================================================================
    print("正在计算......")
    qq = 0
    for qq in range(nIterations + 1):
        # ---- 前向传播 & 误差计算 ----
        for p in range(nPatterns):
            # 隐含层输出
            for h in range(nHiddenNodes):
                s = w1[h][nInputNodes]  # 偏置项
                for i in range(nInputNodes):
                    s += w1[h][i] * out0[p][i]
                out1[p][h] = 1.0 / (1.0 + math.exp(-s))

            # 输出层输出
            for j in range(nOutputNodes):
                s = w2[j][nHiddenNodes]  # 偏置项
                for h in range(nHiddenNodes):
                    s += w2[j][h] * out1[p][h]
                out2[p][j] = 1.0 / (1.0 + math.exp(-s))

            # 输出层误差
            for j in range(nOutputNodes):
                delta2[p][j] = (target[p][j] - out2[p][j]) * \
                               out2[p][j] * (1.0 - out2[p][j])

            # 隐含层误差
            for h in range(nHiddenNodes):
                s = 0.0
                for j in range(nOutputNodes):
                    s += delta2[p][j] * w2[j][h]
                delta1[p][h] = s * out1[p][h] * (1.0 - out1[p][h])

        # ---- 批量更新权值 ----
        # 调整隐含-输出层间的权值
        for j in range(nOutputNodes):
            # 偏置项
            s = 0.0
            for p in range(nPatterns):
                s += delta2[p][j]
            dw = eta * s + alpha * delw2[j][nHiddenNodes]
            w2[j][nHiddenNodes] += dw
            delw2[j][nHiddenNodes] = dw

            # 权值项
            for h in range(nHiddenNodes):
                s = 0.0
                for p in range(nPatterns):
                    s += delta2[p][j] * out1[p][h]
                dw = eta * s + alpha * delw2[j][h]
                w2[j][h] += dw
                delw2[j][h] = dw

        # 调整输入-隐含层的权值
        for h in range(nHiddenNodes):
            # 偏置项
            s = 0.0
            for p in range(nPatterns):
                s += delta1[p][h]
            dw = eta * s + alpha * delw1[h][nInputNodes]
            w1[h][nInputNodes] += dw
            delw1[h][nInputNodes] = dw

            # 权值项
            for i in range(nInputNodes):
                s = 0.0
                for p in range(nPatterns):
                    s += delta1[p][h] * out0[p][i]
                dw = eta * s + alpha * delw1[h][i]
                w1[h][i] += dw
                delw1[h][i] = dw

        # ---- 误差评估 ----
        if MonitorError or (qq % nReportErrors == 0):
            error1 = 0.0
            for p in range(nPatterns):
                for j in range(nOutputNodes):
                    temp = target[p][j] - out2[p][j]
                    error1 += temp * temp
            error1 /= (nPatterns * nOutputNodes)
            MonitorError = False

            # 打印当前迭代进度
            if qq % nReportErrors == 0:
                print("\riteration {:>8d} / {}  Error {:.8f}".format(qq, nIterations, error1), end="", flush=True)

            if error1 < errorLimit:
                print()
                break
    print()
    print("标准化后的平均误差限为: {}".format(error1))

    # ---- 记录调节参数 ----
    with open("调节参数.dat", "a", encoding="utf-8") as f:
        f.write("模式个数:{}\n".format(nPatterns))
        f.write("输入节点个数:{}\n".format(nInputNodes))
        f.write("输出层节点个数:{}\n".format(nOutputNodes))
        f.write("隐含层节点个数:{}\n".format(nHiddenNodes))
        f.write("最大权值:{}\n".format(wmax))
        f.write("学习速率:{}\n".format(eta))
        f.write("势态因子:{}\n".format(alpha))
        f.write("误差平方限为:{}\n".format(errorLimit))
        f.write("最终误差平方之和为:{}\n".format(error1))
        f.write("迭代次数为:{}\n\n".format(qq))

    # ================================================================
    # 6. 将结果反标准化
    # ================================================================
    for i in range(nOutputNodes):
        for j in range(nPatterns):
            # 反标准化公式：[0.1,0.9] -> Z-score -> 原始值
            target[j][i] = ((target[j][i] - 0.1) / 0.8 *
                           (target[nPatterns + 1][i] - target[nPatterns][i]) +
                           target[nPatterns][i]) * fangchay[i] + averagey[i]
            out2[j][i] = ((out2[j][i] - 0.1) / 0.8 *
                          (target[nPatterns + 1][i] - target[nPatterns][i]) +
                          target[nPatterns][i]) * fangchay[i] + averagey[i]

    # ================================================================
    # 7. 输出预测结果及真实对比
    # ================================================================
    with open("鼓风优化参数.dat", "w", encoding="utf-8") as f:
        error1 = 0.0
        for p in range(nPatterns):
            for j in range(nOutputNodes):
                if abs(target[p][j]) > 1e-9:
                    relative_err = (target[p][j] - out2[p][j]) / target[p][j] * 100
                else:
                    relative_err = 0.0 
                f.write("{} {} {} {}\n".format(p, out2[p][j], target[p][j], relative_err))
                temp = target[p][j] - out2[p][j]
                error1 += temp * temp
        error1 /= (nPatterns * nOutputNodes)

    # ================================================================
    # 8. 保存最终权值
    # ================================================================
    with open("权值.dat", "w", encoding="utf-8") as f:
        # 分隔符0（与C++原代码一致）
        for j in range(nInputNodes + 1):
            f.write("0\n")
        # 输入-隐含层权值
        for i in range(nHiddenNodes + 1):
            for j in range(nInputNodes + 1):
                f.write("{}\n".format(w1[i][j]))
        # 隐含-输出层权值
        for i in range(nOutputNodes):
            for j in range(nHiddenNodes + 1):
                f.write("{}\n".format(w2[i][j]))

    print("训练已经结束")
    print("循环次数为: {}".format(qq))
    print("最终原始平均误差为: {}".format(error1))


if __name__ == "__main__":
    main()
