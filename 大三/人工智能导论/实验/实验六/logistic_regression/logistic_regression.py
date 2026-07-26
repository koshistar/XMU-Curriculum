import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib import rcParams
import scipy.optimize as opt
from sklearn.metrics import (
    accuracy_score, classification_report, confusion_matrix,
    roc_curve, auc, roc_auc_score
)
import warnings
warnings.filterwarnings('ignore')

# 中文字体支持
rcParams['font.family'] = 'SimHei'
rcParams['axes.unicode_minus'] = False

def sigmoid(z):
    """
    Sigmoid 激活函数：g(z) = 1 / (1 + e^(-z))

    将任意实数映射到 (0, 1)，输出可以解释为概率。
    """
    return 1 / (1 + np.exp(-z))


def map_feature(x1, x2, degree=6):
    """
    将两个原始特征 (x1, x2) 扩展为 degree 阶多项式特征。

    生成所有 x1^i * x2^j (i+j <= degree, i,j >= 0) 的组合，
    含偏置项（全1列），共 1 + 2 + 3 + ... + (degree+1) = 28 列（degree=6）。

    参数
    ----
    x1, x2 : np.ndarray, shape (m,)
    degree  : int, 多项式阶数，默认6

    返回
    ----
    X_poly : np.ndarray, shape (m, num_features)
    names  : list[str], 每列对应的特征名（便于调试）
    """
    feature_list = [np.ones(len(x1))]   # 偏置项 x1^0 * x2^0 = 1
    names = ['1']

    for i in range(1, degree + 1):
        for j in range(0, i + 1):
            feat = (x1 ** (i - j)) * (x2 ** j)
            feature_list.append(feat)
            names.append(f'x1^{i-j} * x2^{j}')

    X_poly = np.column_stack(feature_list)
    return X_poly, names


def cost_reg(theta, X, y, lam=1.0):
    """
    L2 正则化逻辑回归代价函数：

        J(θ) = (1/m) * Σ [-y·log(h) - (1-y)·log(1-h)]
               + (λ/2m) * Σ_{j=1}^{n} θ_j^2

    注意：偏置项 θ_0 不参与正则化（j 从 1 开始）。

    参数
    ----
    theta : np.ndarray, shape (n,)     参数向量
    X     : np.ndarray, shape (m, n)   特征矩阵（含偏置列）
    y     : np.ndarray, shape (m, 1)   标签列向量
    lam   : float                      正则化系数 λ

    返回
    ----
    J : float  代价值
    """
    theta = np.matrix(theta)          # (1, n)
    X     = np.matrix(X)              # (m, n)
    y     = np.matrix(y)              # (m, 1)
    m     = len(X)

    h = sigmoid(X * theta.T)          # (m, 1) 预测概率

    # 交叉熵损失
    cross_entropy = np.multiply(-y, np.log(h)) \
                  - np.multiply((1 - y), np.log(1 - h))

    # 正则化项
    reg = (lam / (2 * m)) * np.sum(np.power(theta[:, 1:], 2))

    J = np.sum(cross_entropy) / m + reg
    return float(J)


def gradient_reg(theta, X, y, lam=1.0):
    """
    L2 正则化逻辑回归梯度：

        ∂J/∂θ_0 = (1/m) * Σ (h - y) * x_0
        ∂J/∂θ_j = (1/m) * Σ (h - y) * x_j + (λ/m)*θ_j   (j ≥ 1)

    参数 / 返回同 cost_reg，但返回 np.ndarray grad。
    """
    theta = np.matrix(theta)
    X     = np.matrix(X)
    y     = np.matrix(y)
    m     = len(X)
    n     = int(theta.ravel().shape[1])

    grad  = np.zeros(n)
    error = sigmoid(X * theta.T) - y

    for i in range(n):
        term = np.multiply(error, X[:, i])
        if i == 0:
            grad[i] = np.sum(term) / m
        else:
            grad[i] = np.sum(term) / m + (lam / m) * float(theta[:, i])

    return grad


def predict(theta, X, threshold=0.5):
    """
    以 threshold 为决策阈值进行硬预测（0/1）。
    """
    prob = sigmoid(np.matrix(X) * np.matrix(theta).T)
    return [1 if p >= threshold else 0 for p in np.array(prob).ravel()]


if __name__ == '__main__':

    DATA_PATH = r'./ex2data2.txt'
    OUT_DIR   = r'./'

    data2 = pd.read_csv(DATA_PATH, header=None, names=['Test 1', 'Test 2', 'Accepted'])

    x1_raw = data2['Test 1'].values
    x2_raw = data2['Test 2'].values
    y_raw  = data2['Accepted'].values

    pos = data2[data2['Accepted'] == 1]
    neg = data2[data2['Accepted'] == 0]

    fig0, ax0 = plt.subplots(figsize=(6, 5))
    ax0.scatter(pos['Test 1'], pos['Test 2'], s=60, c='royalblue', marker='o', label='接受 (y=1)', zorder=3)
    ax0.scatter(neg['Test 1'], neg['Test 2'], s=60, c='tomato', marker='x', label='拒绝 (y=0)', zorder=3)
    ax0.set_xlabel('芯片测试1得分 (x1)', fontsize=12)
    ax0.set_ylabel('芯片测试2得分 (x2)', fontsize=12)
    ax0.set_title('芯片测试原始数据散点图\n', fontsize=12)
    ax0.legend()
    ax0.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(OUT_DIR + 'scatter.png', dpi=150, bbox_inches='tight')
    print("\n散点图已保存: scatter.png")

    DEGREE = 6
    X_poly, feat_names = map_feature(x1_raw, x2_raw, degree=DEGREE)
    y_col    = y_raw.reshape(-1, 1)
    m, n_feat = X_poly.shape

    LAM         = 1
    theta_init  = np.zeros(n_feat)

    J0 = cost_reg(theta_init, X_poly, y_col, LAM)

    result = opt.fmin_tnc(
        func    = cost_reg,
        x0      = theta_init,
        fprime  = gradient_reg,
        args    = (X_poly, y_col, LAM),
        messages= 0
    )
    theta_opt = result[0]
    J_opt = cost_reg(theta_opt, X_poly, y_col, LAM)

    theta_mat  = np.matrix(theta_opt)
    y_pred     = predict(theta_mat, X_poly, threshold=0.5)
    y_true     = list(y_raw)

    X_mat = np.matrix(X_poly)
    y_prob = [float(sigmoid(X_mat[i] * theta_mat.T)) for i in range(m)]

    acc = accuracy_score(y_true, y_pred)
    cm  = confusion_matrix(y_true, y_pred)
    tn, fp, fn, tp = cm.ravel()

    print("\n预测结果（训练集，λ=1）")
    print(f"  准确率  : {acc * 100:.2f}%")
    print(f"  TP={tp}  FP={fp}  FN={fn}  TN={tn}")
    print("\n分类报告：")
    print(classification_report(y_true, y_pred, target_names=['拒绝(0)', '接受(1)']))
    print("混淆矩阵：")
    print(cm)
    print()

    fpr, tpr, thresholds = roc_curve(y_true, y_prob)
    roc_auc = auc(fpr, tpr)

    # Youden 指数最优阈值
    youden_idx  = int(np.argmax(tpr - fpr))
    best_thresh = thresholds[youden_idx]
    best_tpr    = tpr[youden_idx]
    best_fpr    = fpr[youden_idx]

    fig1, ax1 = plt.subplots(figsize=(7, 6))
    ax1.plot(fpr, tpr, color='darkorange', lw=2, label=f'ROC 曲线 (AUC = {roc_auc:.4f})')
    ax1.plot([0, 1], [0, 1], color='navy', lw=1.2, linestyle='--', label='随机猜测 (AUC = 0.5)')
    ax1.fill_between(fpr, tpr, alpha=0.08, color='darkorange')

    # 标注最优阈值点
    ax1.scatter(best_fpr, best_tpr, color='red', s=90, zorder=5, label=(f'最优阈值 = {best_thresh:.2f}\n'f'TPR={best_tpr:.3f}, FPR={best_fpr:.3f}'))
    ax1.annotate(f'  τ={best_thresh:.2f}',xy=(best_fpr, best_tpr), xytext=(best_fpr + 0.06, best_tpr - 0.06), fontsize=9, color='red')

    ax1.set_xlim([0.0, 1.0])
    ax1.set_ylim([0.0, 1.05])
    ax1.set_xlabel('假阳性率 FPR = FP / (FP + TN)', fontsize=11)
    ax1.set_ylabel('真阳性率 TPR = TP / (TP + FN)', fontsize=11)
    ax1.set_title(f'芯片检测 ROC 曲线\n正则化逻辑回归（λ={LAM}, degree={DEGREE}）',
                  fontsize=12)
    ax1.legend(loc='lower right', fontsize=9)
    ax1.grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig(OUT_DIR + 'roc_curve.png', dpi=150, bbox_inches='tight')
    print("ROC曲线已保存: roc_curve.png")

    u_vals = np.linspace(-1.2, 1.2, 200)
    v_vals = np.linspace(-1.2, 1.2, 200)
    Z = np.zeros((len(u_vals), len(v_vals)))

    for i, ui in enumerate(u_vals):
        for j, vj in enumerate(v_vals):
            feats, _ = map_feature(np.array([ui]), np.array([vj]), DEGREE)
            Z[i, j]  = float(sigmoid(np.matrix(feats) * theta_mat.T))

    Z = Z.T  # 转置对齐 meshgrid 坐标

    fig2, ax2 = plt.subplots(figsize=(7, 6))
    ax2.contourf(u_vals, v_vals, Z, levels=[0, 0.5, 1], colors=['#FFCCCC', '#CCE5FF'], alpha=0.5)
    ax2.contour(u_vals, v_vals, Z, levels=[0.5], colors=['green'], linewidths=2)
    ax2.scatter(pos['Test 1'], pos['Test 2'], s=60, c='royalblue', marker='o', label='接受 (y=1)', zorder=3)
    ax2.scatter(neg['Test 1'], neg['Test 2'], s=60, c='tomato', marker='x', label='拒绝 (y=0)', zorder=3)
    ax2.set_xlabel('芯片测试1得分 (x1)', fontsize=12)
    ax2.set_ylabel('芯片测试2得分 (x2)', fontsize=12)
    ax2.set_title(f'正则化逻辑回归决策边界\nλ={LAM}, degree={DEGREE}, 准确率={acc*100:.1f}%',
                  fontsize=12)
    ax2.legend()
    ax2.grid(True, alpha=0.3)

    from matplotlib.lines import Line2D
    ax2.legend(handles=[
        plt.scatter([], [], s=60, c='royalblue', marker='o', label='接受 (y=1)'),
        plt.scatter([], [], s=60, c='tomato',    marker='x', label='拒绝 (y=0)'),
        Line2D([0], [0], color='green', lw=2, label='决策边界 (P=0.5)'),
    ], fontsize=10)

    plt.tight_layout()
    plt.savefig(OUT_DIR + 'boundary.png', dpi=150, bbox_inches='tight')
    print("决策边界图已保存: boundary.png")

    print("\n【不同正则化系数 λ 的效果对比】")
    print(f"{'λ':<10} {'准确率':>10} {'AUC':>10} {'代价':>12}  说明")
    print("-" * 60)
    lam_remarks = {
        0:     '无正则',
        0.001: '极弱正则',
        0.01:  '弱正则',
        0.1:   '轻正则',
        1:     '均衡',
        10:    '强正则',
        100:   '过强正则',
    }
    for lam_test in [0, 0.001, 0.01, 0.1, 1, 10, 100]:
        th0 = np.zeros(n_feat)
        res = opt.fmin_tnc(func=cost_reg, x0=th0, fprime=gradient_reg,
                           args=(X_poly, y_col, lam_test), messages=0)
        th  = np.matrix(res[0])
        preds_t = predict(th, X_poly)
        acc_t   = accuracy_score(y_true, preds_t)
        proba_t = [float(sigmoid(X_mat[i] * th.T)) for i in range(m)]
        auc_t   = roc_auc_score(y_true, proba_t)
        cost_t  = cost_reg(res[0], X_poly, y_col, lam_test)
        remark  = lam_remarks.get(lam_test, '')
        print(f"{lam_test:<10} {acc_t*100:>9.2f}% {auc_t:>10.4f} {cost_t:>12.4f}  {remark}")