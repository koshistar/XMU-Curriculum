#include <iostream>
#include <fstream>      // 虽然原代码未实际使用文件，但保留以防
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
// #include <conio.h>   // 移除，不再需要（getch/clrscr 等已删除）

using namespace std;

#define maxpop 201
#define maxstring 81
#define n 10
#define pi 3.141592653589793

struct individual {
    int chrom[maxstring];
    double x[n];
    double fitness;
};

double min_val[n], max_val[n];   // 改名避免与 min/max 函数冲突
double r[n];
double port2;

struct individual population[maxpop], oldpopu[maxpop], midst, middle, port;
int popsize = 151;
int lchrom = 10;
int maxgen = 8;                  // 原代码中 maxgen 未实际使用，后续循环用 25
double pcross = 0.6;
double pmutation = 0.02;
double sumfitness;
double avg;
int p;                           // 变量个数

// 适应度函数 f
double f(struct individual popu) {
    double func1, func2, func;
    double y[n];
    double u, dcu, cz, b, wm;
    double cx, tt;

    for (int j = 1; j <= p; j++)
        y[j] = popu.x[j];

    u = pi * y[1] * 1000.0 / 60.0;
    cz = 200.0 / (y[1] * y[1] * (1.0 - y[2] * y[2]) * pi);
    dcu = 980.0 / (1.26 * u * 0.8);
    wm = exp(0.5 * log(cz * cz + (u - dcu / 2.0) * (u - dcu / 2.0)));
    b = 117600.0 / (y[4] * y[3] * 1.26 * 800.0 * wm);
    cx = 0.022 - 0.006 * pi * y[1] / y[3] / b + 0.018 * y[4] * y[4] + 0.02 * 2.0 * pi / (y[3] * (1.0 - y[2]));
    tt = pi * y[1] / y[3];

    r[1] = b / tt - 0.5;
    r[2] = 1.5 - b / tt;

    func1 = 1.0 - cx / y[4] * wm * wm / (u * cz) - 0.1 * y[2] * y[2] * (2.0 - y[2] * y[2]) * cz * cz / (u * dcu);
    func2 = 56.0 * log(u) + 10.0 * log(980.0 * y[1] * (1.0 - y[2]) * y[3] * exp(0.6 * log(b)) / 2.0) - 26.22;
    func = 100.0 * func1 - func2 + 200.0;
    return func;
}

// 另一个适应度函数 f2
double f2(struct individual popu) {
    double func1, func2, func;
    double y[n];
    double u, dcu, cz, b, wm;
    double cx;

    for (int j = 1; j <= p; j++)
        y[j] = popu.x[j];

    u = pi * y[1] * 1000.0 / 60.0;
    cz = 200.0 / (y[1] * y[1] * (1.0 - y[2] * y[2]) * pi);
    dcu = 980.0 / (1.26 * u * 0.8);
    wm = exp(0.5 * log(cz * cz + (u - dcu / 2.0) * (u - dcu / 2.0)));
    b = 117600.0 / (y[4] * y[3] * 1.26 * 800.0 * wm);
    cx = 0.022 - 0.006 * pi * y[1] / y[3] / b + 0.018 * y[4] * y[4] + 0.02 * 2.0 * pi / (y[3] * (1.0 - y[2]));

    func1 = 1.0 - cx / y[4] * wm * wm / (u * cz) - 0.1 * y[2] * y[2] * (2.0 - y[2] * y[2]) * cz * cz / (u * dcu);
    func2 = 56.0 * log(u) + 10.0 * log(980.0 * y[1] * (1.0 - y[2]) * y[3] * exp(0.6 * log(b)) / 2.0);
    func = 100.0 * func1 - func2 + 500.0;
    return func;
}

// 随机数生成（现代替换：使用 rand() + srand(time)）
double ran(double minimum, double maximum) {
    double t;
    do {
        t = (rand() % 10001) / 10000.0 * 20.0;   // 保持原逻辑范围
    } while (t < minimum || t > maximum);
    return t;
}

// 局部搜索（爬山法）
void power(struct individual& popu) {   // 改为引用传递，避免拷贝问题
    double gg = f(popu);
    for (int pq = 1; pq <= p; pq++) {
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
        } else {
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

// 初始化种群
void initialize() {
    sumfitness = 0.0;
    cout << "Please input the number of the variable: ";
    cin >> p;
    cout << "Please input minimum and maximum of each variable:\n";
    for (int j = 1; j <= p; j++) {
        cin >> min_val[j] >> max_val[j];
    }

    for (int i = 1; i < popsize; i++) {   // 原代码 popsize-1
        for (int j = 1; j <= p; j++) {
            population[i].x[j] = ran(min_val[j], max_val[j]);
        }
        population[i].fitness = f(population[i]);
        sumfitness += population[i].fitness;
    }
    avg = sumfitness / (popsize - 1);
    cout << "\naverage of the sumfitness is " << avg << endl;
}

// 适应度平移（避免负值）
void change() {
    double ff = population[1].fitness;
    for (int i = 2; i < popsize; i++) {
        if (ff > population[i].fitness) ff = population[i].fitness;
    }
    sumfitness = 0.0;
    for (int i = 1; i < popsize; i++) {
        population[i].fitness -= ff;
        sumfitness += population[i].fitness;
    }
    avg = sumfitness / (popsize - 1);
}

// 编码（二进制）
void decode() {
    for (int i = 1; i < popsize; i++) {
        for (int j = 1; j <= lchrom * p; j++) population[i].chrom[j] = 0;

        for (int j = 1; j <= p; j++) {
            int v = (int)((population[i].x[j] - min_val[j]) * 1023.0 / (max_val[j] - min_val[j]));
            for (int m = 1; m <= lchrom; m++) {
                int num1 = lchrom * (j - 1) + m;
                if (v % 2 == 1) population[i].chrom[num1] = 1;
                v /= 2;
            }
        }
    }
}

// 拷贝个体（修复原代码 bug：newpop.x[j] 应拷贝 x 而非 fitness）
void cop(struct individual& newpop, const struct individual& oldpop) {
    for (int j = 1; j <= p; j++) {
        newpop.x[j] = oldpop.x[j];          // 修正：原代码错误地拷贝了 fitness
        newpop.fitness = oldpop.fitness;
    }
    for (int i = 1; i <= lchrom * p; i++) { // 应拷贝全部染色体长度
        newpop.chrom[i] = oldpop.chrom[i];
    }
}

// 按适应度降序排序
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

// 随机翻转（交叉/变异概率）
int flip(double pp) {
    return (rand() / (double)RAND_MAX < pp) ? 1 : 0;
}

// 选择个体（轮盘赌）
int select() {
    double r_val = (rand() % 10001) / 10000.0 * sumfitness;
    double partsum = 0.0;
    for (int i = 1; i < popsize; i++) {
        partsum += population[i].fitness;
        if (partsum >= r_val) return i;
    }
    return popsize - 1;
}

// 交叉与变异
void crossmutation() {
    int nv = lchrom * p;
    // 先复制选择后的个体到 oldpopu
    for (int i = 1; i < popsize; i++) {
        int a = select();
        cop(oldpopu[i], population[a]);
    }

    // 复制回 population（原代码有逻辑问题，这里简化处理）
    for (int i = 1; i < popsize; i++) {
        cop(population[i], oldpopu[i]);
    }

    for (int i = 1; i < popsize / 2; i++) {
        int m = rand() % (popsize - 1) + 1;
        int z = rand() % (popsize - 1) + 1;

        if (flip(pcross)) {
            int bit = rand() % nv + 1;
            for (int j = bit; j <= nv; j++) {
                int t = population[m].chrom[j];
                population[m].chrom[j] = population[z].chrom[j];
                population[z].chrom[j] = t;
            }
        }

        if (flip(pmutation)) {
            int bit = rand() % nv + 1;
            population[m].chrom[bit] = 1 - population[m].chrom[bit];
            population[z].chrom[bit] = 1 - population[z].chrom[bit];
        }
    }
}

// 解码（从二进制还原实数）
void undecode() {
    sumfitness = 0.0;
    for (int i = 1; i < popsize; i++) {
        for (int j = 1; j <= p; j++) {
            double accum = 0.0;
            double powerof2 = 1.0;
            for (int m = 1; m <= lchrom; m++) {
                int num2 = (j - 1) * lchrom + m;
                if (population[i].chrom[num2] == 1)
                    accum += powerof2;
                powerof2 *= 2.0;
            }
            population[i].x[j] = accum * (max_val[j] - min_val[j]) / 1023.0 + min_val[j];
            cout << population[i].x[j] << " ";
        }
        cout << endl;
        population[i].fitness = f(population[i]);
        sumfitness += population[i].fitness;
    }
}

// 主函数
int main() {
    srand((unsigned)time(nullptr));   // 初始化随机种子

    initialize();
    change();
    sort_population();
    power(population[1]);
    decode();

    cop(port, population[1]);

    for (int gen = 1; gen <= 25; gen++) {   // 原代码硬编码 25 代
        crossmutation();
        undecode();
        sort_population();
        cop(population[popsize - 1], port);   // 保留精英
        change();
    }

    sort_population();
    power(population[1]);

    // 输出最优解
    cout << "\nBest solution:\n";
    for (int q = 1; q <= p; q++) {
        cout << population[1].x[q] << "\t";
    }
    double g = f2(population[1]);
    cout << "\nFitness f2: " << g << endl;

    // 原代码中硬编码的测试点
    population[1].x[1] = 1.4891;
    population[1].x[2] = 0.2942;
    population[1].x[3] = 13.0;
    population[1].x[4] = 1.1591;
    g = f2(population[1]);
    cout << "Test point f2: " << g << endl;

    system("pause");   // 防止控制台立即关闭（VS2022 推荐方式）
    return 0;
}