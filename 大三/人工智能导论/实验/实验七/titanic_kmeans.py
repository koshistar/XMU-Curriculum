"""
  K = 2（对应 survived=0/1 两类）
  
  特征组合对比：
  ① 基础组   [sex, class]          — 最直觉的两个强特征
  ② 社会组   [sex, class, fare]    — 加入票价（经济地位）
  ③ 家庭组   [age, sibsp, parch, alone] — 人口特征
  ④ 全特征组 [sex,age,fare,class,alone] — 五维综合

  评估指标：轮廓系数 (Silhouette)、调整兰德指数 (ARI)、聚类准确率
=============================================================
"""

import os
import sys
import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib import rcParams
from itertools import permutations

from sklearn.preprocessing import LabelEncoder, StandardScaler
from sklearn.metrics import (silhouette_score, adjusted_rand_score,
                              accuracy_score, confusion_matrix,
                              classification_report)

# 导入KMeans
SYS_BASE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(SYS_BASE, 'k_means'))
from k_means import KMeans   # 实验自带实现

# 中文字体
rcParams['font.family'] = 'SimHei'
rcParams['axes.unicode_minus'] = False

OUT_DIR = SYS_BASE

# ═══════════════════════════════════════════════════════════════
# 1. 数据加载与预处理
# ═══════════════════════════════════════════════════════════════
print("=" * 65)
print("  Titanic K-Means 聚类分析")
print("=" * 65)

DATA_PATH = os.path.join(SYS_BASE, 'data', 'Titanic.csv')
df = pd.read_csv(DATA_PATH)

# 缺失值处理
df['age'] = df['age'].fillna(df['age'].median())
df['embarked'] = df['embarked'].fillna(df['embarked'].mode()[0])

# 编码
le = LabelEncoder()
for col in ['sex', 'embarked', 'class', 'who']:
    df[col] = le.fit_transform(df[col].astype(str))
df['alone'] = df['alone'].astype(int)

y_true = df['survived'].values     # 真实标签

print(f"数据已加载: {df.shape[0]} 条样本")

# ═══════════════════════════════════════════════════════════════
# 2. 工具函数
# ═══════════════════════════════════════════════════════════════

def cluster_accuracy(y_true, y_pred, n_clusters=2):
    """
    因为 K-Means 的簇编号与真实标签可能对调，
    枚举所有排列方式，取准确率最高的那种。
    """
    best_acc = 0
    labels = list(range(n_clusters))
    for perm in permutations(labels):
        mapping = {old: new for old, new in enumerate(perm)}
        y_mapped = np.array([mapping[int(l)] for l in y_pred.ravel()])
        acc = accuracy_score(y_true, y_mapped)
        if acc > best_acc:
            best_acc = acc
            best_mapping = mapping
    return best_acc, best_mapping


def run_kmeans(X_raw, y_true, feature_names, group_name, n_clusters=2, max_iter=100, n_runs=10):
    """
    多次运行取最优（规避随机初始化影响），返回评估指标。
    """
    # 标准化
    scaler = StandardScaler()
    X = scaler.fit_transform(X_raw)

    best_sil = -1
    best_ids = None
    best_centroids = None

    for run in range(n_runs):
        km = KMeans(X, n_clusters)
        centroids, ids = km.train(max_iter)
        # 跳过含 NaN 的结果
        if np.any(np.isnan(ids)):
            continue
        try:
            sil = silhouette_score(X, ids.ravel())
        except:
            sil = -1
        if sil > best_sil:
            best_sil = sil
            best_ids = ids.copy()
            best_centroids = centroids.copy()

    # 兜底：如果所有运行都失败，用最后一次结果
    if best_ids is None:
        best_ids = ids
        best_centroids = centroids
        best_sil = -1

    ari  = adjusted_rand_score(y_true, best_ids.ravel())
    acc, mapping = cluster_accuracy(y_true, best_ids, n_clusters)

    print(f"\n【{group_name}】特征: {feature_names}")
    print(f"  轮廓系数 Silhouette : {best_sil:.4f} ")
    print(f"  调整兰德指数 ARI   : {ari:.4f} ")
    print(f"  聚类准确率         : {acc*100:.2f}%")

    return best_centroids, best_ids, best_sil, ari, acc, X


# ═══════════════════════════════════════════════════════════════
# 3. 四组特征对比实验
# ═══════════════════════════════════════════════════════════════

EXPERIMENTS = [
    {
        'name'    : '①基础组 [性别+舱位]',
        'cols'    : ['sex', 'class'],
        'names_cn': ['性别', '舱位等级'],
        'x_axis'  : 'sex',
        'y_axis'  : 'class',
    },
    {
        'name'    : '②社会组 [性别+舱位+票价]',
        'cols'    : ['sex', 'class', 'fare'],
        'names_cn': ['性别', '舱位等级', '票价'],
        'x_axis'  : 'sex',
        'y_axis'  : 'fare',
    },
    {
        'name'    : '③家庭组 [年龄+兄弟姐妹+父母子女+独行]',
        'cols'    : ['age', 'sibsp', 'parch', 'alone'],
        'names_cn': ['年龄', '兄弟/配偶数', '父母/子女数', '是否独行'],
        'x_axis'  : 'age',
        'y_axis'  : 'fare',
    },
    {
        'name'    : '④全特征组 [性别+年龄+票价+舱位+独行]',
        'cols'    : ['sex', 'age', 'fare', 'class', 'alone'],
        'names_cn': ['性别', '年龄', '票价', '舱位等级', '是否独行'],
        'x_axis'  : 'sex',
        'y_axis'  : 'fare',
    },
]

results = []
all_kmeans_data = []

for exp in EXPERIMENTS:
    X_raw = df[exp['cols']].values
    centroids, ids, sil, ari, acc, X_scaled = run_kmeans(
        X_raw, y_true, exp['names_cn'], exp['name'])
    results.append({
        'name': exp['name'],
        'sil' : sil,
        'ari' : ari,
        'acc' : acc,
    })
    all_kmeans_data.append({
        'exp'       : exp,
        'centroids' : centroids,
        'ids'       : ids,
        'X_scaled'  : X_scaled,
        'X_raw'     : X_raw,
    })

# ═══════════════════════════════════════════════════════════════
# 4. 汇总对比表
# ═══════════════════════════════════════════════════════════════
print("\n" + "=" * 65)
print("【四组特征 K-Means 效果对比汇总】")
print(f"{'特征组':30s} {'轮廓系数':>10} {'ARI':>8} {'准确率':>10}")
print("-" * 65)
for r in results:
    print(f"{r['name']:30s} {r['sil']:>10.4f} {r['ari']:>8.4f} {r['acc']*100:>9.2f}%")
best_idx = max(range(len(results)), key=lambda i: results[i]['acc'])
print(f"\n★ 最优特征组: {results[best_idx]['name']}  (准确率={results[best_idx]['acc']*100:.2f}%)")

# ═══════════════════════════════════════════════════════════════
# 5. 可视化：2×4 对比图
# ═══════════════════════════════════════════════════════════════
fig, axes = plt.subplots(2, 4, figsize=(20, 9))
fig.suptitle('Titanic K-Means 聚类：四组特征对比（真实标签 vs 聚类结果）', fontsize=14, fontweight='bold')

COLORS_TRUE = ['#EF5350', '#42A5F5']   # 红=遇难, 蓝=幸存
COLORS_PRED = ['#FF8A65', '#66BB6A']   # 橙=簇0,  绿=簇1

for col_idx, (data, r) in enumerate(zip(all_kmeans_data, results)):
    exp   = data['exp']
    ids   = data['ids'].ravel().astype(int)
    X_raw = data['X_raw']
    X_sc  = data['X_scaled']

    # 取前两个特征作为可视化坐标
    xi = 0; yi = 1 if X_raw.shape[1] > 1 else 0

    ax_top = axes[0, col_idx]
    ax_bot = axes[1, col_idx]

    x_label = exp['names_cn'][xi]
    y_label = exp['names_cn'][yi]

    # 上行：真实标签
    for cls, label, color in zip([0,1], ['遇难','幸存'], COLORS_TRUE):
        mask = y_true == cls
        ax_top.scatter(X_raw[mask, xi], X_raw[mask, yi], s=20, c=color, alpha=0.6, label=label)
    ax_top.set_title(f'{exp["name"]}\n（真实标签）', fontsize=8.5)
    ax_top.set_xlabel(x_label, fontsize=8); ax_top.set_ylabel(y_label, fontsize=8)
    ax_top.legend(fontsize=7); ax_top.grid(alpha=0.25)

    # 下行：K-Means 聚类结果
    for k, color in zip([0, 1], COLORS_PRED):
        mask = ids == k
        ax_bot.scatter(X_raw[mask, xi], X_raw[mask, yi], s=20, c=color, alpha=0.6, label=f'簇{k}')

    # 簇中心（反标准化回原始空间）
    scaler = StandardScaler().fit(X_raw)
    c_orig = scaler.inverse_transform(data['centroids'])
    ax_bot.scatter(c_orig[:, xi], c_orig[:, yi], s=150, c='black', marker='*', zorder=5, label='中心点')
    ax_bot.set_title(
        f'K-Means 结果\n准确率={r["acc"]*100:.1f}% ARI={r["ari"]:.3f}',
        fontsize=8.5)
    ax_bot.set_xlabel(x_label, fontsize=8); ax_bot.set_ylabel(y_label, fontsize=8)
    ax_bot.legend(fontsize=7); ax_bot.grid(alpha=0.25)

plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.savefig(os.path.join(OUT_DIR, 'titanic_kmeans_comparison.png'), dpi=150, bbox_inches='tight')
print(f"\n对比可视化图已保存: titanic_kmeans_comparison.png")

# ═══════════════════════════════════════════════════════════════
# 6. 指标柱状图
# ═══════════════════════════════════════════════════════════════
fig2, axes2 = plt.subplots(1, 3, figsize=(15, 5))
fig2.suptitle('四组特征 K-Means 聚类评估指标对比', fontsize=13, fontweight='bold')

short_names = ['①基础', '②社会', '③家庭', '④全特征']
sils  = [r['sil']  for r in results]
aris  = [r['ari']  for r in results]
accs  = [r['acc']*100 for r in results]

palette = ['#42A5F5', '#66BB6A', '#FFA726', '#AB47BC']

for ax, vals, title, ylabel in zip(
        axes2,
        [sils, aris, accs],
        ['轮廓系数 (Silhouette)', '调整兰德指数 (ARI)', '聚类准确率 (%)'],
        ['分值', '分值', '准确率 (%)']):
    bars = ax.bar(short_names, vals, color=palette, edgecolor='grey', width=0.5)
    ax.set_title(title, fontsize=12)
    ax.set_ylabel(ylabel)
    ax.grid(alpha=0.3, axis='y')
    for bar, v in zip(bars, vals):
        ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.005,
                f'{v:.3f}' if ylabel != '准确率 (%)' else f'{v:.1f}%',
                ha='center', fontsize=9)

plt.tight_layout()
plt.savefig(os.path.join(OUT_DIR, 'titanic_kmeans_metrics.png'), dpi=150, bbox_inches='tight')
print("评估指标柱状图已保存: titanic_kmeans_metrics.png")

# ═══════════════════════════════════════════════════════════════
# 7. 最优组详细分析（全特征组）
# ═══════════════════════════════════════════════════════════════
best_data = all_kmeans_data[best_idx]
ids_best  = best_data['ids'].ravel().astype(int)
_, best_mapping = cluster_accuracy(y_true, ids_best, n_clusters=2)
y_mapped  = np.array([best_mapping[l] for l in ids_best])
cm        = confusion_matrix(y_true, y_mapped)

print(f"\n【最优组（{results[best_idx]['name']}）详细分析】")
print(f"  簇标签映射: 簇0→{best_mapping[0]}类, 簇1→{best_mapping[1]}类")
print(f"\n混淆矩阵:")
print(cm)
print(f"\n分类报告（K-Means 与真实标签对照）：")
print(classification_report(y_true, y_mapped, target_names=['遇难(0)', '幸存(1)']))
