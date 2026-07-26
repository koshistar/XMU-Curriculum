import matplotlib
import matplotlib.pyplot as plt
import math
# 变量
x01=-0.2
x02=2.5
lemda=0.1
t=0
u=0
# 用于记录每次迭代的数据
history_u   = []   # 总迭代次数
history_y0  = []   # 当前极小值
history_x1  = []   # 自变量 x1
history_x2  = []   # 自变量 x2

# 解决中文显示问题
matplotlib.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'Arial Unicode MS']
matplotlib.rcParams['axes.unicode_minus'] = False

print("开始计算")
while t<=10:
    u=u+1
    
    #梯度下降法寻优
    #y0=x01*x01+25*x02*x02
    y0=4+4.5*x01-4*x02+x01*x01+2*x02*x02-2*x01*x02+x01*x01*x01*x01-2*x01*x01*x02
	#y0=100*(x01-x02)+(1-x01)*(1-x01)

    #对第一个变量的偏导数
	#pian1=2*x01;
    pian1=4.5+2*x01-2*x02+4*x01*x01*x01-4*x01*x02;
    #pian1=100-2*(1-x01);

    #对第二个变量的偏导数
	#pian2=50*x02;
    pian2=-4+4*x02-2*x01-2*x01*x01;
    #pian2=-100;

    #x1=x01-lemda*(2*x01)/math.sqrt(4*x01*x01+2500*x02*x02);
    x1=x01-lemda*pian1/math.sqrt(pian1*pian1+pian2*pian2);

    #x2=x02-lemda*(25*x02)/math.sqrt(4*x01*x01+2500*x02*x02);
    x2=x02-lemda*pian2/math.sqrt(pian1*pian1+pian2*pian2);

    #y1=x1*x1+25*x2*x2;
    y1=4+4.5*x1-4*x2+x1*x1+2*x2*x2-2*x1*x2+x1*x1*x1*x1-2*x1*x1*x2;
    #y1=100*(x1-x2)+(1-x1)*(1-x1);

    if y1<y0:
        x01=x1;
        x02=x2;

    print(" 总迭代次数为"+str(u)+" 自变量为"+str(x01)+"    "+str(x02)+" 当前极小值为"+str(y0)+" t值为 "+str(t));
    # 记录历史数据
    history_u.append(u)
    history_y0.append(y0)
    history_x1.append(x01)
    history_x2.append(x02)
    t=t+1
print("极小值为： "+str(y0))
print("计算已结束")
fig, axes = plt.subplots(1, 2, figsize=(12, 5))
fig.suptitle("梯度下降优化算法 — 迭代过程", fontsize=14, fontweight='bold')

# 左图：目标函数值随迭代次数的变化
axes[0].plot(history_u, history_y0, 'o-', color='#185FA5', linewidth=2,
             markersize=6, markerfacecolor='white', markeredgewidth=2)
axes[0].set_xlabel("迭代次数 u", fontsize=12)
axes[0].set_ylabel("目标函数值 y0", fontsize=12)
axes[0].set_title("目标函数收敛曲线", fontsize=12)
axes[0].grid(True, linestyle='--', alpha=0.5)
for i, (xu, yv) in enumerate(zip(history_u, history_y0)):
    axes[0].annotate(f"{yv:.3f}", (xu, yv),
                     textcoords="offset points", xytext=(0, 8),
                     ha='center', fontsize=8, color='#185FA5')

# 右图：自变量 x1, x2 随迭代次数的变化轨迹
axes[1].plot(history_u, history_x1, 's-', color='#3B6D11', linewidth=2,
             markersize=6, label='x01', markerfacecolor='white', markeredgewidth=2)
axes[1].plot(history_u, history_x2, '^-', color='#993C1D', linewidth=2,
             markersize=6, label='x02', markerfacecolor='white', markeredgewidth=2)
axes[1].set_xlabel("迭代次数 u", fontsize=12)
axes[1].set_ylabel("自变量取值", fontsize=12)
axes[1].set_title("自变量迭代轨迹", fontsize=12)
axes[1].legend(fontsize=11)
axes[1].grid(True, linestyle='--', alpha=0.5)

plt.tight_layout()
out_path = "gradient_descent_result.png"
plt.savefig(out_path, dpi=150, bbox_inches='tight')
print(f"\n可视化图表已保存至：{out_path}")
plt.show()