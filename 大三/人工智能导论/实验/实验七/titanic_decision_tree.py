"""
  sex       : 性别 (male/female)          → 编码为 0/1
  age       : 年龄（含177个缺失值）         → 用中位数填充
  sibsp     : 兄弟姐妹/配偶数
  parch     : 父母/子女数
  fare      : 票价
  embarked  : 登船港口 (S/C/Q)            → 编码为 0/1/2
  class     : 舱位等级 (First/Second/Third)→ 编码为 0/1/2
  who       : 人员类型 (man/woman/child)   → 编码
  alone     : 是否独行 (True/False)        → 编码
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

from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.tree import DecisionTreeClassifier, export_graphviz, export_text
from sklearn.metrics import (accuracy_score, classification_report,
                             confusion_matrix, roc_curve, auc)
from sklearn.preprocessing import LabelEncoder
import graphviz

# 中文字体
rcParams['font.family'] = 'SimHei'
rcParams['axes.unicode_minus'] = False

# graphviz 可执行文件路径
GRAPHVIZ_BIN = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'graphviz_bin', 'Graphviz', 'bin')
os.environ['PATH'] = GRAPHVIZ_BIN + os.pathsep + os.environ.get('PATH', '')

# 输出目录
OUT_DIR = os.path.dirname(os.path.abspath(__file__))

# ═══════════════════════════════════════════════════════════════
# 1. 数据加载与预处理
# ═══════════════════════════════════════════════════════════════
print("=" * 65)
print("  Titanic 决策树分类预测")
print("=" * 65)

DATA_PATH = os.path.join(OUT_DIR, 'data', 'Titanic.csv')
df = pd.read_csv(DATA_PATH)

print(f"\n【原始数据】共 {df.shape[0]} 条, {df.shape[1]} 列")
print(df.head(3).to_string())
print(f"\n缺失值统计:\n{df.isnull().sum()[df.isnull().sum() > 0]}")

# 缺失值处理
df['age'] = df['age'].fillna(df['age'].median())
df['embarked'] = df['embarked'].fillna(df['embarked'].mode()[0])
# 类别编码
le = LabelEncoder()
cat_cols = ['sex', 'embarked', 'class', 'who']
for col in cat_cols:
    df[col] = le.fit_transform(df[col].astype(str))

df['alone'] = df['alone'].astype(int)   # True→1, False→0

print(f"\n【预处理后】缺失值: {df.isnull().sum().sum()}")

# 特征 / 标签
FEATURE_COLS = ['sex', 'age', 'sibsp', 'parch', 'fare','embarked', 'class', 'who', 'alone']
FEATURE_NAMES_CN = ['性别', '年龄', '兄弟/配偶数', '父母/子女数', '票价', '登船港口', '舱位等级', '人员类型', '是否独行']

X = df[FEATURE_COLS].values
y = df['survived'].values

# 划分训练/测试集（7:3）
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.3, random_state=42, stratify=y)

print(f"\n训练集: {len(X_train)} 条  |  测试集: {len(X_test)} 条")

# ═══════════════════════════════════════════════════════════════
# 2. 训练决策树
# ═══════════════════════════════════════════════════════════════
dt = DecisionTreeClassifier(
    criterion='entropy',   # 使用信息熵（ID3思想）
    max_depth=4,           # 限制深度防过拟合，且graphviz图清晰
    min_samples_split=10,  # 分裂最少样本数
    min_samples_leaf=5,    # 叶节点最少样本数
    random_state=42
)
dt.fit(X_train, y_train)

# ═══════════════════════════════════════════════════════════════
# 3. 评估
# ═══════════════════════════════════════════════════════════════
y_pred  = dt.predict(X_test)
y_prob  = dt.predict_proba(X_test)[:, 1]
acc     = accuracy_score(y_test, y_pred)
cm      = confusion_matrix(y_test, y_pred)
tn, fp, fn, tp = cm.ravel()

# 5折交叉验证
cv_scores = cross_val_score(dt, X, y, cv=5, scoring='accuracy')

print(f"  测试集准确率    : {acc*100:.2f}%")
print(f"  5折交叉验证均值 : {cv_scores.mean()*100:.2f}% ± {cv_scores.std()*100:.2f}%")
print(f"  TP={tp}  FP={fp}  FN={fn}  TN={tn}")
print(f"\n分类报告：")
print(classification_report(y_test, y_pred, target_names=['遇难(0)', '幸存(1)']))

# ROC / AUC
fpr, tpr, _ = roc_curve(y_test, y_prob)
roc_auc = auc(fpr, tpr)
print(f"  AUC = {roc_auc:.4f}")

# 特征重要性
importances = dt.feature_importances_
feat_imp = sorted(zip(FEATURE_COLS, FEATURE_NAMES_CN, importances), key=lambda x: x[2], reverse=True)
print(f"\n特征重要性排名（信息增益比例）：")
for eng, cn, imp in feat_imp:
    bar = '█' * int(imp * 40)
    print(f"  {cn:8s}({eng:10s}): {imp:.4f}  {bar}")

# ═══════════════════════════════════════════════════════════════
# 4. graphviz 树形可视化
# ═══════════════════════════════════════════════════════════════
print("\n正在生成决策树可视化图...")

dot_data = export_graphviz(
    dt,
    out_file=None,
    feature_names=FEATURE_COLS,        # 节点特征名
    class_names=['Died', 'Survived'],   # 叶节点类别
    filled=True,                        # 按预测类别着色
    rounded=True,                       # 圆角节点
    special_characters=True,
    impurity=True,                      # 显示 entropy
    proportion=False
)

# 渲染为 PNG
source = graphviz.Source(dot_data)
out_path = os.path.join(OUT_DIR, 'titanic_tree')
source.render(out_path, format='png', cleanup=True)
print(f"树形图已保存: {out_path}.png")

# ═══════════════════════════════════════════════════════════════
# 5. ROC 曲线 + 特征重要性柱状图
# ═══════════════════════════════════════════════════════════════
fig, axes = plt.subplots(1, 2, figsize=(14, 5))

# 5.1 ROC 曲线
axes[0].plot(fpr, tpr, color='darkorange', lw=2, label=f'ROC 曲线 (AUC={roc_auc:.4f})')
axes[0].plot([0,1],[0,1],'--', color='navy', lw=1.2, label='随机猜测')
axes[0].fill_between(fpr, tpr, alpha=0.08, color='darkorange')
youden_i = np.argmax(tpr - fpr)
axes[0].scatter(fpr[youden_i], tpr[youden_i], color='red', s=80, zorder=5, label=f'最优阈值点 ({fpr[youden_i]:.2f},{tpr[youden_i]:.2f})')
axes[0].set_title('决策树 ROC 曲线', fontsize=13)
axes[0].set_xlabel('假阳性率 FPR'); axes[0].set_ylabel('真阳性率 TPR')
axes[0].legend(loc='lower right'); axes[0].grid(alpha=0.3)

# 5.2 特征重要性
names_en  = [e[0] for e in feat_imp]
names_cn  = [e[1] for e in feat_imp]
imps      = [e[2] for e in feat_imp]
colors    = plt.cm.RdYlGn([i/len(imps) for i in range(len(imps))])[::-1]

bars = axes[1].barh(names_cn[::-1], imps[::-1], color=colors[::-1], edgecolor='grey')
axes[1].set_title('特征重要性（信息增益比例）', fontsize=13)
axes[1].set_xlabel('重要性分值')
for bar, imp in zip(bars, imps[::-1]):
    axes[1].text(bar.get_width() + 0.002, bar.get_y() + bar.get_height()/2, f'{imp:.4f}', va='center', fontsize=9)
axes[1].grid(alpha=0.3, axis='x')

plt.tight_layout()
plt.savefig(os.path.join(OUT_DIR, 'titanic_dt_analysis.png'), dpi=150, bbox_inches='tight')
print("ROC & 特征重要性图已保存: titanic_dt_analysis.png")

# ═══════════════════════════════════════════════════════════════
# 6. 不同 max_depth 准确率对比
# ═══════════════════════════════════════════════════════════════
print("\n【不同树深度准确率对比】")
print(f"{'max_depth':>10} {'训练准确率':>12} {'测试准确率':>12} {'AUC':>8}")
print("-" * 46)
for depth in [2, 3, 4, 5, 6, 8, None]:
    clf = DecisionTreeClassifier(criterion='entropy', max_depth=depth, random_state=42)
    clf.fit(X_train, y_train)
    tr_acc = accuracy_score(y_train, clf.predict(X_train))
    te_acc = accuracy_score(y_test,  clf.predict(X_test))
    te_auc = auc(*roc_curve(y_test, clf.predict_proba(X_test)[:,1])[:2])
    depth_str = str(depth) if depth else 'None(无限)'
    flag = ' ← 选用' if depth == 4 else ''
    print(f"{depth_str:>10} {tr_acc*100:>11.2f}% {te_acc*100:>11.2f}% {te_auc:>8.4f}{flag}")
