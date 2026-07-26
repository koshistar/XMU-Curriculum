#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

#define maxpop 201
#define maxstring 81
#define n 10
#define pi 3.141592653589793

struct individual {
    double chrom[maxstring];   // 注意：原代码定义为 double，这里保持一致（虽然通常用 int）
    double x[n];
    double fitness;
};

double min_val[n], max_val[n];
struct individual population[maxpop], midst, mid;
int popsize = 151;
int maxgen = 15;
double pcross = 0.6;
double pmutation = 0.02;
double sumfitness;
double avg;
int p;

// 适应度函数（Six-Hump Camel Back 函数的变形，求最大值）
double f(struct individual popu) {
    double func;
    double y[n];

    for (int j = 1; j <= p; j++) {
        int k = (j - 1) * 2 + 1;
        popu.x[j] = popu.chrom[k] + popu.chrom[k + 1];   // 实数编码 + 简单相加
        y[j] = popu.x[j];
    }

    // Six-Hump Camel Back 函数（原为求最小值，这里取 -func + 10000 转为求最大值）
    func = (4 - 2.1 * y[1] * y[1] + y[1] * y[1] * y[1] * y[1] / 3.0) * y[1] * y[1];
    func = func + y[1] * y[2] + (4 * y[2] * y[2] - 4) * y[2] * y[2];
    func = -func + 10000.0;   // 转为最大化问题
    return func;
}

// 局部爬山搜索（power）
void power(struct individual& popu) {   // 使用引用
    double g = f(popu);
    for (int j = 1; j <= p; j++) {
        popu.x[j] -= 0.01;
        double t1 = f(popu);
        if (popu.x[j] < min_val[j]) {
            popu.x[j] += 0.1;
            continue;
        }
        if (t1 < g) {
            while (t1 < g) {
                g = t1;
                popu.x[j] -= 0.01;
                if (popu.x[j] < min_val[j]) {
                    popu.x[j] += 0.1;
                    break;
                }
                t1 = f(popu);
            }
        }
        else {
            while (t1 > g) {
                t1 = g;
                popu.x[j] += 0.01;
                if (popu.x[j] > max_val[j]) {
                    popu.x[j] -= 0.01;
                    break;
                }
                g = f(popu);
            }
        }
    }
}

// 随机生成（修复原 Rnd）
double ran(double minimum, double maximum) {
    return minimum + (maximum - minimum) * (rand() % 10001) / 10000.0 * 10.0;  // 保持原范围逻辑
}

// 初始化种群（修复：原来未给 x 和 chrom 赋值，导致垃圾值）
void initialize() {
    sumfitness = 0.0;
    cout << "Please input the number of the variable: ";
    cin >> p;
    cout << "Please input minimum and maximum of each variable:\n";
    for (int j = 1; j <= p; j++) {
        cin >> min_val[j] >> max_val[j];
    }

    for (int i = 1; i < popsize; i++) {   // 原 popsize-1
        for (int j = 1; j <= p; j++) {
            // 实数编码：随机生成 chrom[奇偶]，x = chrom[k] + chrom[k+1]
            population[i].chrom[(j - 1) * 2 + 1] = ran(min_val[j] / 2, max_val[j] / 2);
            population[i].chrom[(j - 1) * 2 + 2] = ran(min_val[j] / 2, max_val[j] / 2);
            population[i].x[j] = population[i].chrom[(j - 1) * 2 + 1] + population[i].chrom[(j - 1) * 2 + 2];
        }
        population[i].fitness = f(population[i]);
        sumfitness += population[i].fitness;
    }
    avg = sumfitness / (popsize - 1);
    cout << "\naverage of the sumfitness is " << fixed << setprecision(4) << avg << endl;
}

// 拷贝个体（修复严重 bug：原代码 newpop.x[j] = oldpop.fitness 是错的）
void cop(struct individual& newpop, const struct individual& oldpop) {
    for (int j = 1; j <= p; j++) {
        newpop.x[j] = oldpop.x[j];          // 修正：拷贝 x 而非 fitness
        newpop.fitness = oldpop.fitness;
    }
    for (int i = 1; i <= 2 * p; i++) {      // 对应 chrom 长度
        newpop.chrom[i] = oldpop.chrom[i];
    }
}

// 排序（适应度从大到小）
void sort_population() {
    for (int j = 1; j <= popsize - 2; j++) {
        for (int i = 1; i < popsize - j; i++) {
            if (population[i].fitness < population[i + 1].fitness) {
                midst = population[i];
                population[i] = population[i + 1];
                population[i + 1] = midst;
            }
        }
    }
}

// 翻转概率
int flip(double pp) {
    double f_val = rand() / (double)RAND_MAX;
    return (f_val < pp) ? 1 : 0;
}

// 选择（轮盘赌简化修复）
int select() {
    double r_val = (rand() % 10001) / 10000.0 * sumfitness;
    double partsum = 0.0;
    for (int i = 1; i < popsize; i++) {
        partsum += population[i].fitness;
        if (partsum >= r_val) return i;
    }
    return popsize - 1;
}

// 交叉与变异（大幅简化并修复原混乱逻辑、越界、未初始化等问题）
void crossmutation() {
    int nv = 2 * p;
    for (int i = 1; i < popsize; i++) {
        int a = select();
        cop(population[i], population[a]);
    }

    for (int i = 1; i < popsize / 2; i++) {
        int m = 1 + rand() % (popsize - 1);
        int z = 1 + rand() % (popsize - 1);

        if (flip(pcross)) {
            int bit = 1 + rand() % nv;
            for (int j = bit; j <= nv; j++) {
                double t = population[m].chrom[j];
                population[m].chrom[j] = population[z].chrom[j];
                population[z].chrom[j] = t;
            }
        }

        if (flip(pmutation)) {
            int bit = 1 + rand() % nv;
            if (bit % 2 == 1) bit++;   // 尽量成对变异
            if (bit > nv) bit = nv;
            population[m].chrom[bit] = (rand() % 2 == 0) ? 0.0 : 1.0;  // 简单二值化变异
            population[z].chrom[bit] = (rand() % 2 == 0) ? 0.0 : 1.0;
        }
    }
}

// 主函数
int main() {
    srand((unsigned)time(nullptr));

    initialize();
    sort_population();
    power(population[1]);
    cop(mid, population[1]);

    int iter = 0;
    const int max_iter = 100;   // 防止无限循环

    do {
        crossmutation();
        sort_population();
        cop(population[popsize - 1], mid);   // 精英保留
        power(population[1]);
        cop(mid, population[1]);

        // 重新计算平均适应度（原代码缺少，容易导致除0或错误）
        sumfitness = 0.0;
        for (int i = 1; i < popsize; i++) {
            sumfitness += population[i].fitness;
        }
        avg = sumfitness / (popsize - 1);

        iter++;
        if (iter % 5 == 0) {
            cout << "迭代 " << iter << " 次   当前最佳适应度: "
                << fixed << setprecision(4) << population[1].fitness << endl;
        }
    } while ((population[1].fitness - avg > 0.1) && (iter < max_iter));

    sort_population();
    power(population[1]);

    cout << "\n计算结束！\n最佳解:\n";
    for (int q = 1; q <= p; q++) {
        cout << population[1].x[q] << "\t";
    }
    double g = -f(population[1]) + 10000.0;   // 还原原始函数值（最小值）
    cout << "\n原始函数值 (最小值): " << fixed << setprecision(4) << g << endl;

    // 再次局部搜索并输出
    power(population[1]);
    g = -f(population[1]) + 10000.0;
    cout << "局部搜索后原始函数值: " << g << endl;

    system("pause");
    return 0;
}