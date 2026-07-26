#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

#define maxpop 160
#define maxstring 81
#define n 10
#define pi 3.141592653589793

struct individual {
    int chrom[maxstring];
    double x[n];
    double fitness;
};

double min_val[n], max_val[n];
double r[n];
double port2;

struct individual population[maxpop];
struct individual oldpopu[maxpop];
struct individual midst;
struct individual middle;
struct individual port;

int popsize = 50;
int lchrom = 10;
double pcross = 0.6;
double pmutation = 0.03;
double sumfitness;
double avg;
int p;   // 变量个数

// 定义适应度函数
double f(struct individual popu) {
    double func;
//检验函数1,求全局极小值
//		func=100*(popu.x[0]*popu.x[0]-popu.x[1])*(popu.x[0]*popu.x[0]-popu.x[1])
//          +(1-popu.x[0])*(1-popu.x[0]);
//		return(-func);
//检验函数2,求全局极小值
//		func=(1+(popu.x[0]+popu.x[1]+1)*(popu.x[0]
//          +popu.x[1]+1)*(19-14*popu.x[0]+3*popu.x[0]*popu.x[0]-14*popu.x[1]
//          +6*popu.x[0]*popu.x[1]+3*popu.x[1]*popu.x[1]))*(30+(2*popu.x[0]-3*popu.x[1])
//              *(2*popu.x[0]-3*popu.x[1])*(18-32*popu.x[0]+12*popu.x[0]*popu.x[0]+48*popu.x[1]
//                  -36*popu.x[0]*popu.x[1]+27*popu.x[1]*popu.x[1]));
//		return(-func);
//检验函数3,求全局极大值
    func = 0.5 - (sin(sqrt(popu.x[0] * popu.x[0] + popu.x[1] * popu.x[1])) *
        sin(sqrt(popu.x[0] * popu.x[0] + popu.x[1] * popu.x[1])) - 0.5) /
        (1.0 + 0.001 * (popu.x[0] * popu.x[0] + popu.x[1] * popu.x[1]) *
            (1.0 + 0.001 * (popu.x[0] * popu.x[0] + popu.x[1] * popu.x[1])));
    return func;
}

// 随机数生成
double ran(double minimum, double maximum) {
    return minimum + (maximum - minimum) * rand() / 32767.0;
}

// 局部爬山搜索（power）
void power(struct individual& popu) {   // 使用引用传递
    double gg = f(popu);
    for (int pq = 0; pq < p; pq++) {
        popu.x[pq] += 0.1;
        if (popu.x[pq] > max_val[pq]) {
            popu.x[pq] -= 0.1;
            continue;
        }
        double ff = f(popu);
        if (gg < ff) {
            while (gg < ff) {
                gg = ff;
                popu.x[pq] += 0.1;
                if (popu.x[pq] > max_val[pq]) {
                    popu.x[pq] -= 0.1;
                    break;
                }
                ff = f(popu);
            }
        }
        else {
            while (gg > ff) {
                ff = gg;
                popu.x[pq] -= 0.1;
                gg = f(popu);
                if (popu.x[pq] < min_val[pq]) {
                    popu.x[pq] += 0.1;
                    break;
                }
            }
        }
    }
}

// 编码（二进制）
void decode() {
    for (int i = 0; i < popsize; i++) {
        for (int j = 0; j < lchrom * p; j++) {
            population[i].chrom[j] = 0;
        }
        for (int j = 0; j < p; j++) {
            int v = (int)((population[i].x[j] - min_val[j]) * 1023.0 / (max_val[j] - min_val[j]));
            for (int m = 0; m < lchrom; m++) {
                int num1 = lchrom * j + m;
                population[i].chrom[num1] = (v % 2 == 1) ? 1 : 0;
                v /= 2;
                if (v == 0) break;
            }
        }
    }
}

// 翻转概率
int flip(double pp) {
    return (rand() / 32767.0 < pp) ? 1 : 0;
}

// 选择操作（轮盘赌简化版）
int select() {
    double r_val = rand() / 32767.0 * sumfitness;
    double partsum = 0.0;
    for (int i = 0; i < popsize; i++) {
        partsum += population[i].fitness;
        if (partsum >= r_val) return i;
    }
    return popsize - 1;
}

// 交叉与变异（已修复大量原代码逻辑错误）
void crossmutation() {
    int nv = lchrom * p;
    double per = 0.08;           // 精英保留比例
    int beg = (int)(popsize * per);

    // 交叉与变异只对后 (1-per) 的个体进行
    for (int i = beg; i < popsize / 2; i++) {   // 简化循环，避免越界
        int m = beg + rand() % (popsize - beg);
        int z = beg + rand() % (popsize - beg);

        if (flip(pcross)) {
            int bit = 1 + rand() % (nv - 1);
            for (int j = bit; j < nv; j++) {
                int t = population[m].chrom[j];
                population[m].chrom[j] = population[z].chrom[j];
                population[z].chrom[j] = t;
            }
        }
        if (flip(pmutation)) {
            int bit = 1 + rand() % (nv - 1);
            population[m].chrom[bit] = 1 - population[m].chrom[bit];
            population[z].chrom[bit] = 1 - population[z].chrom[bit];
        }
    }
}

// 解码（二进制转实数）
void undecode() {
    sumfitness = 0.0;
    for (int i = 0; i < popsize; i++) {
        for (int j = 0; j < p; j++) {
            double accum = 0.0;
            double powerof2 = 1.0;
            for (int m = 0; m < lchrom; m++) {
                int num2 = j * lchrom + m;
                if (population[i].chrom[num2] == 1)
                    accum += powerof2;
                powerof2 *= 2.0;
            }
            population[i].x[j] = accum * (max_val[j] - min_val[j]) / 1023.0 + min_val[j];
        }
        population[i].fitness = f(population[i]);
        sumfitness += population[i].fitness;
    }
}

// 主函数
int main() {
    srand((unsigned)time(nullptr));

    double eps = 0.01;
    int times = 0;

    cout << "Please input the number of the variable: ";
    cin >> p;

    cout << "Please input minimum and maximum of each variable:\n";
    for (int j = 0; j < p; j++) {
        cin >> min_val[j] >> max_val[j];
    }

    // 初始化种群
    sumfitness = 0.0;
    for (int i = 0; i < popsize; i++) {
        for (int j = 0; j < p; j++) {
            population[i].x[j] = ran(min_val[j], max_val[j]);
        }
        population[i].fitness = f(population[i]);
        sumfitness += population[i].fitness;
    }
    avg = sumfitness / popsize;

    // 初始排序（适应度从大到小）
    for (int j = 0; j < popsize - 1; j++) {
        for (int i = 0; i < popsize - 1 - j; i++) {
            if (population[i].fitness < population[i + 1].fitness) {
                midst = population[i];
                population[i] = population[i + 1];
                population[i + 1] = midst;
            }
        }
    }

    cout << "\n正在计算...\n";

    // 主循环（直到平均适应度与最优适应度足够接近）
    while (fabs(avg - population[0].fitness) > eps) {
        power(population[0]);     // 局部搜索
        decode();                 // 编码
        crossmutation();          // 交叉 + 变异
        undecode();               // 解码 + 计算适应度

        // 重新计算 sumfitness 和 avg
        sumfitness = 0.0;
        for (int i = 0; i < popsize; i++) {
            population[i].fitness = f(population[i]);
            sumfitness += population[i].fitness;
        }
        avg = sumfitness / popsize;

        // 重新排序
        for (int j = 0; j < popsize - 1; j++) {
            for (int i = 0; i < popsize - 1 - j; i++) {
                if (population[i].fitness < population[i + 1].fitness) {
                    midst = population[i];
                    population[i] = population[i + 1];
                    population[i + 1] = midst;
                }
            }
        }

        times++;
        cout << "第 " << times << " 次   最大值: "
            << fixed << setprecision(6) << population[0].fitness
            << "   坐标: " << population[0].x[0] << "  " << population[0].x[1] << endl;

        if (times > 200) {   // 防止无限循环
            cout << "达到最大迭代次数，提前终止。\n";
            break;
        }
    }

    cout << "\n计算已结束！\n";
    cout << "该函数的最大值为: " << population[0].fitness << endl;
    cout << "对应坐标为: " << population[0].x[0] << "   " << population[0].x[1] << endl;
    cout << "平均适应度: " << avg << endl;
    cout << "循环次数: " << times << endl;

    system("pause");
    return 0;
}