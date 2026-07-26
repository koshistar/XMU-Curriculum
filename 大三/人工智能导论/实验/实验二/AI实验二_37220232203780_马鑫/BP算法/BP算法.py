import sys
import math
import os
import numpy as np

# ============================================================
#  工具函数：矩阵 / 向量的动态分配与释放
# ============================================================

def matrix_allocate(n_rows, n_cols):
    """分配 n_rows x n_cols 的二维 double 数组"""
    return np.zeros((n_rows, n_cols), dtype=np.float64)

def vector_allocate(n_cols):
    """分配长度为 n_cols 的一维 double 数组"""
    return np.zeros(n_cols, dtype=np.float64)

def matrix_free(mat):
    """释放矩阵（Python 中由 GC 自动管理，此处仅作占位兼容）"""
    pass

# ============================================================
#  核心网络结构体
# ============================================================

class BPSavedParams:
    """保存每次运行时动态分配的网络层矩阵"""
    def __init__(self):
        self.out0 = None   # 输入层
        self.out1 = None   # 隐含层
        self.delta1 = None # 隐含层误差修正量
        self.delw1 = None  # 输入-隐含层权值变化量
        self.w1 = None     # 输入-隐含层权值
        self.out2 = None   # 输出层
        self.delta2 = None # 输出层误差修正量
        self.delw2 = None  # 隐含-输出层权值变化量
        self.w2 = None     # 隐含-输出层权值
        self.target = None # 目标输出值
        self.pattern_id = None  # 模式识别标志

# ============================================================
#  主程序
# ============================================================

def main():
    # ---------- 默认超参数 ----------
    eta = 0.15            # 学习速率
    alpha = 0.075         # 势态因子（动量项）
    n_report_errors = 100 # 误差报告频率
    error_level = 0.02    # 满意的误差水平
    monitor_error = False # 是否强制监控误差

    # ---------- 解析命令行可选参数 ----------
    args = sys.argv[1:]
    idx = 0
    while idx < len(args):
        arg = args[idx]
        if not arg.startswith('-'):
            break
        opt = arg[1]
        if opt == 'e' and idx + 1 < len(args):
            n_report_errors = int(args[idx + 1])
            idx += 2
        elif opt == 'd' and idx + 1 < len(args):
            error_level = float(args[idx + 1])
            idx += 2
        else:
            break

    # 必须提供运行配置文件名
    if idx >= len(args):
        print("Usage: python {} {-en -df} runfilename".format(sys.argv[0]))
        print("  -e n => report error every n iterations (default 100)")
        print("  -d f => done if mean squared error < f (default 0.02)")
        sys.exit(1)

    run_filename = args[idx]
    progname = sys.argv[0]

    # ---------- 打开运行配置文件 ----------
    if not os.path.exists(run_filename):
        print("{}: can't open file {}".format(progname, run_filename))
        sys.exit(1)

    with open(run_filename, 'r') as fp_run:
        # 读第一行：运行次数
        n_runs = int(fp_run.readline().strip())

        # ---------- 每次运行 ----------
        for r in range(n_runs):
            # 读第二行：所有文件路径 + 网络参数
            tokens = fp_run.readline().strip().split()
            sz_results   = tokens[0]
            sz_error     = tokens[1]
            sz_pattern   = tokens[2]
            sz_weights   = tokens[3]
            sz_weights_out = tokens[4]
            n_patterns     = int(tokens[5])
            n_iterations   = int(tokens[6])
            n_input_nodes  = int(tokens[7])
            n_hidden_nodes = int(tokens[8])
            n_output_nodes = int(tokens[9])
            eta            = float(tokens[10])
            alpha          = float(tokens[11])

            net = BPSavedParams()

            # ---------- 动态存储分配 ----------
            net.out0   = matrix_allocate(n_patterns, n_input_nodes)
            net.out1   = matrix_allocate(n_patterns, n_hidden_nodes)
            net.out2   = matrix_allocate(n_patterns, n_output_nodes)
            net.delta2 = matrix_allocate(n_patterns, n_output_nodes)
            net.delw2  = matrix_allocate(n_output_nodes, n_hidden_nodes + 1)  # 含偏置列
            net.w2     = matrix_allocate(n_output_nodes, n_hidden_nodes + 1)
            net.delta1 = matrix_allocate(n_patterns, n_hidden_nodes)
            net.delw1  = matrix_allocate(n_hidden_nodes, n_input_nodes + 1)   # 含偏置列
            net.w1     = matrix_allocate(n_hidden_nodes, n_input_nodes + 1)
            net.target = matrix_allocate(n_patterns, n_output_nodes)
            net.pattern_id = vector_allocate(n_patterns)

            # ---------- 读初始权矩阵 ----------
            if not os.path.exists(sz_weights):
                print("{}: can't open file {}".format(progname, sz_weights))
                sys.exit(1)

            with open(sz_weights, 'r') as fp_w:
                all_vals = fp_w.read().split()
                ptr = 0
                # 输入-隐含层权值: nHiddenNodes x (nInputNodes+1)
                for h in range(n_hidden_nodes):
                    for i in range(n_input_nodes + 1):
                        net.w1[h][i] = float(all_vals[ptr])
                        net.delw1[h][i] = 0.0
                        ptr += 1
                # 隐含-输出层权值: nOutputNodes x (nHiddenNodes+1)
                for j in range(n_output_nodes):
                    for h in range(n_hidden_nodes + 1):
                        net.w2[j][h] = float(all_vals[ptr])
                        net.delw2[j][h] = 0.0
                        ptr += 1

            # ---------- 读所有训练模式 ----------
            if not os.path.exists(sz_pattern):
                print("{}: can't open file {}".format(progname, sz_pattern))
                sys.exit(1)

            p = 0
            with open(sz_pattern, 'r') as fp_p:
                all_lines = fp_p.read().split()
                val_ptr = 0
                while p < n_patterns:
                    # 检查是否有足够的数据
                    needed = n_input_nodes + n_output_nodes + 1  # 输入 + 目标 + ID
                    if val_ptr + needed > len(all_lines):
                        break
                    # 读输入层
                    for i in range(n_input_nodes):
                        net.out0[p][i] = float(all_lines[val_ptr])
                        val_ptr += 1
                    # 读目标输出
                    for j in range(n_output_nodes):
                        net.target[p][j] = float(all_lines[val_ptr])
                        val_ptr += 1
                    # 读模式识别标志
                    net.pattern_id[p] = float(all_lines[val_ptr])
                    val_ptr += 1
                    p += 1

            if p < n_patterns:
                print("{}: {} out of {} patterns read".format(progname, p, n_patterns))
                n_patterns = p

            # ---------- 打开误差输出文件 ----------
            try:
                fp_error = open(sz_error, 'w')
            except IOError:
                print("{}: can't open file {}".format(progname, sz_error))
                sys.exit(1)

            if n_iterations > 1:
                print("Training...")
            else:
                print("Testing...")

            # ==================== 开始迭代循环 ====================
            error = 0.0
            q = 0
            for q in range(n_iterations):
                for p in range(n_patterns):
                    # ---- 隐含层前向传播 ----
                    for h in range(n_hidden_nodes):
                        s = net.w1[h][n_input_nodes]  # 偏置项
                        for i in range(n_input_nodes):
                            s += net.w1[h][i] * net.out0[p][i]
                        net.out1[p][h] = 1.0 / (1.0 + math.exp(-s))

                    # ---- 输出层前向传播 ----
                    for j in range(n_output_nodes):
                        s = net.w2[j][n_hidden_nodes]  # 偏置项
                        for h in range(n_hidden_nodes):
                            s += net.w2[j][h] * net.out1[p][h]
                        net.out2[p][j] = 1.0 / (1.0 + math.exp(-s))

                    # ---- 输出层误差修正量 ----
                    for j in range(n_output_nodes):
                        net.delta2[p][j] = (net.target[p][j] - net.out2[p][j]) \
                                           * net.out2[p][j] * (1.0 - net.out2[p][j])

                    # ---- 隐含层误差修正量 ----
                    for h in range(n_hidden_nodes):
                        s = 0.0
                        for j in range(n_output_nodes):
                            s += net.delta2[p][j] * net.w2[j][h]
                        net.delta1[p][h] = s * net.out1[p][h] * (1.0 - net.out1[p][h])

                # ---- 调整隐含-输出层的权值（批量累加后更新）----
                for j in range(n_output_nodes):
                    # 偏置项
                    s = 0.0
                    for p in range(n_patterns):
                        s += net.delta2[p][j]
                    dw = eta * s + alpha * net.delw2[j][n_hidden_nodes]
                    net.w2[j][n_hidden_nodes] += dw
                    net.delw2[j][n_hidden_nodes] = dw

                    # 权值项
                    for h in range(n_hidden_nodes):
                        s = 0.0
                        for p in range(n_patterns):
                            s += net.delta2[p][j] * net.out1[p][h]
                        dw = eta * s + alpha * net.delw2[j][h]
                        net.w2[j][h] += dw
                        net.delw2[j][h] = dw

                # ---- 调整输入-隐含层的权值（批量累加后更新）----
                for h in range(n_hidden_nodes):
                    # 偏置项
                    s = 0.0
                    for p in range(n_patterns):
                        s += net.delta1[p][h]
                    dw = eta * s + alpha * net.delw1[h][n_input_nodes]
                    net.w1[h][n_input_nodes] += dw
                    net.delw1[h][n_input_nodes] = dw

                    # 权值项
                    for i in range(n_input_nodes):
                        s = 0.0
                        for p in range(n_patterns):
                            s += net.delta1[p][h] * net.out0[p][i]
                        dw = eta * s + alpha * net.delw1[h][i]
                        net.w1[h][i] += dw
                        net.delw1[h][i] = dw

                # ---- 计算均方根误差 ----
                if monitor_error or (q % n_report_errors == 0):
                    error = 0.0
                    for p in range(n_patterns):
                        for j in range(n_output_nodes):
                            temp = net.target[p][j] - net.out2[p][j]
                            error += temp * temp
                    error /= (n_patterns * n_output_nodes)
                    print("\riteration {}-{} Error {:f}".format(q, n_iterations, error), end="")
                    monitor_error = False

                    if q % n_report_errors == 0:
                        fp_error.write("{} {:f}\n".format(q, error))

                    if error < error_level:
                        break

            # ==================== 迭代循环结束 ====================

            # 计算最终平均误差
            error = 0.0
            for p in range(n_patterns):
                for j in range(n_output_nodes):
                    temp = net.target[p][j] - net.out2[p][j]
                    error += temp * temp
            error /= (n_patterns * n_output_nodes)

            print("\nIteration {}-{} Error {:f}".format(q, n_iterations, error))
            fp_error.write("\n{} {:f}\n".format(q, error))
            fp_error.close()

            # ---------- 输出最终权值文件 ----------
            try:
                fp_wout = open(sz_weights_out, 'w')
            except IOError:
                print("{}: can't write file {}".format(progname, sz_weights_out))
                sys.exit(1)

            items_per_line = 8
            # 输入-隐含层权值
            count = 0
            for h in range(n_hidden_nodes):
                for i in range(n_input_nodes + 1):
                    fp_wout.write("{:g}".format(net.w1[h][i]))
                    count += 1
                    if count % items_per_line == 0:
                        fp_wout.write('\n')
                    else:
                        fp_wout.write(' ')
            # 隐含-输出层权值
            count = 0
            for j in range(n_output_nodes):
                for h in range(n_hidden_nodes + 1):
                    fp_wout.write("{:g}".format(net.w2[j][h]))
                    count += 1
                    if count % items_per_line == 0:
                        fp_wout.write('\n')
                    else:
                        fp_wout.write(' ')
            fp_wout.close()

            # ---------- 输出结果文件 ----------
            try:
                fp_res = open(sz_results, 'w')
            except IOError:
                print("{}: can't write file {}".format(progname, sz_results))
                fp_res = sys.stderr

            for p in range(n_patterns):
                line = "{} ".format(p)
                for j in range(n_output_nodes):
                    line += "{:f} ".format(net.out2[p][j])
                line += " {:>-6.0f}\n".format(net.pattern_id[p])
                fp_res.write(line)
            fp_res.close()

            # ---------- 释放存储（Python 自动回收，此处仅作示意）----------
            matrix_free(net.out0)
            matrix_free(net.out1)
            matrix_free(net.delta1)
            matrix_free(net.delw1)
            matrix_free(net.w1)
            matrix_free(net.out2)
            matrix_free(net.delta2)
            matrix_free(net.delw2)
            matrix_free(net.w2)
            matrix_free(net.target)
            del net.pattern_id


if __name__ == '__main__':
    main()
