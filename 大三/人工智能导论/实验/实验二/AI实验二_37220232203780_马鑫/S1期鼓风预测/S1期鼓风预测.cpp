#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <locale>

using namespace std;

#define InputNo 3           // 输入节点个数
#define OutputnNo 3         // 输出节点个数
#define PatternNo 358       // 模式个数（训练样本数）
#define ErrorLevelValue 0.005 // 满意的误差水平
#define wmaxValue 0.1       // 初始权值最大范围
#define HiddenNo 10         // 隐含层节点个数
#define IterationsNo 1000000 // 最大迭代次数
#define etaValue 0.1        // 默认的学习速率
#define alphaValue 0.1      // 默认的势态因子（动量项）

int main()
{
    setlocale(LC_ALL, "chs");

    int nReportErrors = 100; // 误差报告频率
    char MonitorError = 0;   // 表示为真
    double error1 = 0.0;     // 最新的误差平方之和
    double temp;             // 临时变量

    double** out1 = new double* [PatternNo];
    double** delta1 = new double* [PatternNo];
    double** out2 = new double* [PatternNo];
    double** delta2 = new double* [PatternNo];
    double** x = new double* [PatternNo];
    double** y = new double* [PatternNo];
    double** out0 = new double* [PatternNo + 2];
    double** target = new double* [PatternNo + 2];

    for (int i = 0; i < PatternNo; i++) {
        out1[i] = new double[HiddenNo]; delta1[i] = new double[HiddenNo];
        out2[i] = new double[OutputnNo]; delta2[i] = new double[OutputnNo];
        x[i] = new double[InputNo]; y[i] = new double[OutputnNo];
    }
    for (int i = 0; i < PatternNo + 2; i++) {
        out0[i] = new double[InputNo]; target[i] = new double[OutputnNo];
    }

    double** delw1 = new double* [HiddenNo + 1];
    double** w1 = new double* [HiddenNo + 1];
    for (int i = 0; i <= HiddenNo; i++) {
        delw1[i] = new double[InputNo + 1]; w1[i] = new double[InputNo + 1];
    }

    double** delw2 = new double* [OutputnNo];
    double** w2 = new double* [OutputnNo];
    for (int i = 0; i < OutputnNo; i++) {
        delw2[i] = new double[HiddenNo + 1]; w2[i] = new double[HiddenNo + 1];
    }

    double ss[InputNo], average[InputNo], sss[InputNo], fangcha[InputNo];
    double ssy[OutputnNo], averagey[OutputnNo], sssy[OutputnNo], fangchay[OutputnNo];
    double min_val, max_val;
    int h, i, j, p, qq;
    int nPatterns = PatternNo, nInputNodes = InputNo;
    int nHiddenNodes = HiddenNo, nOutputNodes = OutputnNo;
    int nIterations = IterationsNo;
    double scale = 32767.0, errorLimit = ErrorLevelValue;
    double wmax = wmaxValue, eta = etaValue, alpha = alphaValue;

    // ==================== 1. 将节点数写到数据文件中去 ====================
    ofstream my_file3("节点数.dat", ios::out);
    my_file3 << nInputNodes << "\n" << nHiddenNodes << "\n" << nOutputNodes << "\n";
    my_file3.close();

    // ==================== 2. 神经网络的训练阶段：读取历史数据 ====================
    ifstream my_file1("S1期鼓风操作数据.dat");
    if (!my_file1) {
        cout << "错误：找不到文件 'S1期鼓风操作数据.dat'，请将其放在与exe同级的目录下！" << endl;
        system("pause");
        return -1;
    }

    for (i = 0; i <= nPatterns - 1; i++) {
        for (j = 0; j <= nInputNodes + nOutputNodes - 1; j++) { // 原代码为 nInputNodes+2，实际即加OutputnNo
            if (j >= nInputNodes) {
                my_file1 >> y[i][j - nInputNodes];
            }
            else {
                my_file1 >> x[i][j];
            }
        }
    }
    my_file1.close();

    // ==================== 3. 数据标准化处理 ====================
    // 3.1 对输入值标准化 (Z-score -> [0.1, 0.9])
    for (i = 0; i <= nInputNodes - 1; i++) {
        ss[i] = 0;
        for (j = 0; j <= nPatterns - 1; j++) ss[i] += x[j][i];
        average[i] = ss[i] / nPatterns;
    }
    for (i = 0; i <= nInputNodes - 1; i++) {
        sss[i] = 0;
        for (j = 0; j <= nPatterns - 1; j++) sss[i] += (x[j][i] - average[i]) * (x[j][i] - average[i]);
        fangcha[i] = sqrt(sss[i] / (nPatterns - 1));
    }
    for (i = 0; i <= nInputNodes - 1; i++) {
        min_val = 1000.0; max_val = -1000.0;
        for (j = 0; j <= nPatterns - 1; j++) {
            out0[j][i] = (x[j][i] - average[i]) / fangcha[i];
            if (out0[j][i] <= min_val) min_val = out0[j][i];
            if (out0[j][i] >= max_val) max_val = out0[j][i];
        }
        out0[nPatterns][i] = min_val;       // 存放最小值
        out0[nPatterns + 1][i] = max_val;   // 存放最大值
    }
    for (i = 0; i <= nInputNodes - 1; i++)
        for (j = 0; j <= nPatterns - 1; j++)
            out0[j][i] = (out0[j][i] - out0[nPatterns][i]) / (out0[nPatterns + 1][i] - out0[nPatterns][i]) * 0.8 + 0.1;

    // 将输入标准化参数写入文件
    ofstream my_file5("输入自标准化.dat", ios::out);
    for (i = 0; i <= nInputNodes - 1; i++)
        my_file5 << average[i] << "\n" << fangcha[i] << "\n" << out0[nPatterns][i] << "\n" << out0[nPatterns + 1][i] << "\n";
    my_file5.close();

    // 3.2 对输出值标准化 (Z-score -> [0.1, 0.9])
    for (i = 0; i <= nOutputNodes - 1; i++) {
        ssy[i] = 0;
        for (j = 0; j <= nPatterns - 1; j++) ssy[i] += y[j][i];
        averagey[i] = ssy[i] / nPatterns;
        cout << "输出值平均值为:" << averagey[i] << " ";
    }
    cout << endl;
    for (i = 0; i <= nOutputNodes - 1; i++) {
        sssy[i] = 0;
        for (j = 0; j <= nPatterns - 1; j++) sssy[i] += (y[j][i] - averagey[i]) * (y[j][i] - averagey[i]);
        fangchay[i] = sqrt(sssy[i] / (nPatterns - 1));
    }
    for (i = 0; i <= nOutputNodes - 1; i++) {
        min_val = 1000.0; max_val = -1000.0;
        for (j = 0; j <= nPatterns - 1; j++) {
            target[j][i] = (y[j][i] - averagey[i]) / fangchay[i];
            if (target[j][i] <= min_val) min_val = target[j][i];
            if (target[j][i] >= max_val) max_val = target[j][i];
        }
        target[nPatterns][i] = min_val;
        target[nPatterns + 1][i] = max_val;
    }
    for (i = 0; i <= nOutputNodes - 1; i++)
        for (j = 0; j <= nPatterns - 1; j++)
            target[j][i] = (target[j][i] - target[nPatterns][i]) / (target[nPatterns + 1][i] - target[nPatterns][i]) * 0.8 + 0.1;

    ofstream my_file6("输出自标准化.dat", ios::out);
    for (i = 0; i <= nOutputNodes - 1; i++)
        my_file6 << averagey[i] << "\n" << fangchay[i] << "\n" << target[nPatterns][i] << "\n" << target[nPatterns + 1][i] << "\n";
    my_file6.close();

    // ==================== 4. 权值初始化 ====================
    double frand;
    for (i = 0; i <= nHiddenNodes; i++)
        for (j = 0; j <= nInputNodes; j++) {
            frand = rand(); w1[i][j] = wmax * (1.0 - 2 * frand / scale); delw1[i][j] = 0.0;
        }
    for (i = 0; i <= nOutputNodes - 1; i++)
        for (j = 0; j <= nHiddenNodes; j++) {
            frand = rand(); w2[i][j] = wmax * (1.0 - 2 * frand / scale); delw2[i][j] = 0.0;
        }

    // ==================== 5. 开始迭代循环（核心BP算法） ====================
    cout << "正在计算......" << endl;
    for (qq = 0; qq <= nIterations; qq++) {
        for (p = 0; p <= nPatterns - 1; p++) {
            for (h = 0; h <= nHiddenNodes - 1; h++) {
                double sum = w1[h][nInputNodes]; // 加上偏置
                for (i = 0; i <= nInputNodes - 1; i++) sum += w1[h][i] * out0[p][i];
                out1[p][h] = 1.0 / (1.0 + exp(-sum));
            }
            for (j = 0; j <= nOutputNodes - 1; j++) {
                double sum = w2[j][nHiddenNodes]; // 加上偏置
                for (h = 0; h <= nHiddenNodes - 1; h++) sum += w2[j][h] * out1[p][h];
                out2[p][j] = 1.0 / (1.0 + exp(-sum));
            }
            for (j = 0; j <= nOutputNodes - 1; j++)
                delta2[p][j] = (target[p][j] - out2[p][j]) * out2[p][j] * (1.0 - out2[p][j]);
            for (h = 0; h <= nHiddenNodes - 1; h++) {
                double sum = 0.0;
                for (j = 0; j <= nOutputNodes - 1; j++) sum += delta2[p][j] * w2[j][h];
                delta1[p][h] = sum * out1[p][h] * (1.0 - out1[p][h]);
            }
        }
        // 批量更新权值
        for (j = 0; j <= nOutputNodes - 1; j++) {
            double dw, sum = 0.0;
            for (p = 0; p <= nPatterns - 1; p++) sum += delta2[p][j];
            dw = eta * sum + alpha * delw2[j][nHiddenNodes]; w2[j][nHiddenNodes] += dw; delw2[j][nHiddenNodes] = dw;
            for (h = 0; h <= nHiddenNodes - 1; h++) {
                sum = 0.0;
                for (p = 0; p <= nPatterns - 1; p++) sum += delta2[p][j] * out1[p][h];
                dw = eta * sum + alpha * delw2[j][h]; w2[j][h] += dw; delw2[j][h] = dw;
            }
        }
        for (h = 0; h <= nHiddenNodes - 1; h++) {
            double dw, sum = 0.0;
            for (p = 0; p <= nPatterns - 1; p++) sum += delta1[p][h];
            dw = eta * sum + alpha * delw1[h][nInputNodes]; w1[h][nInputNodes] += dw; delw1[h][nInputNodes] = dw;
            for (i = 0; i <= nInputNodes - 1; i++) {
                sum = 0.0;
                for (p = 0; p <= nPatterns - 1; p++) sum += delta1[p][h] * out0[p][i];
                dw = eta * sum + alpha * delw1[h][i]; w1[h][i] += dw; delw1[h][i] = dw;
            }
        }
        // 误差评估
        if (MonitorError || (qq % nReportErrors == 0)) {
            error1 = 0.0;
            for (p = 0; p <= nPatterns - 1; p++) for (j = 0; j <= nOutputNodes - 1; j++) { temp = target[p][j] - out2[p][j]; error1 += temp * temp; }
            error1 /= (nPatterns * nOutputNodes);
            MonitorError = 0;
            if (error1 < errorLimit) break;
        }
    }
    cout << "标准化后的平均误差限为:" << error1 << endl;

    // 记录调节参数
    ofstream my_file7("调节参数.dat", ios::app);
    my_file7 << "模式个数:" << nPatterns << "\n输入节点个数:" << nInputNodes << "\n输出层节点个数:" << nOutputNodes
        << "\n隐含层节点个数:" << nHiddenNodes << "\n最大权值:" << wmax << "\n学习速率:" << eta
        << "\n势态因子:" << alpha << "\n误差平方限为:" << errorLimit << "\n最终误差平方之和为:" << error1
        << "\n迭代次数为:" << qq << "\n\n";
    my_file7.close();

    // ==================== 6. 将结果反标准化（还原为真实物理值） ====================
    for (i = 0; i <= nOutputNodes - 1; i++) {
        for (j = 0; j <= nPatterns - 1; j++) {
            target[j][i] = ((target[j][i] - 0.1) / 0.8 * (target[nPatterns + 1][i] - target[nPatterns][i]) + target[nPatterns][i]) * fangchay[i] + averagey[i];
            out2[j][i] = ((out2[j][i] - 0.1) / 0.8 * (target[nPatterns + 1][i] - target[nPatterns][i]) + target[nPatterns][i]) * fangchay[i] + averagey[i];
        }
    }

    // ==================== 7. 输出预测结果及真实对比 ====================
    ofstream my_file("鼓风优化参数.dat", ios::out);
    error1 = 0.0;
    for (p = 0; p <= nPatterns - 1; p++) {
        for (j = 0; j <= nOutputNodes - 1; j++) {
            my_file << p << " " << out2[p][j] << " " << target[p][j] << " " << (target[p][j] - out2[p][j]) / target[p][j] * 100 << "\n";
            temp = target[p][j] - out2[p][j]; error1 += temp * temp;
        }
    }
    error1 /= (nPatterns * nOutputNodes);
    my_file.close();

    // ==================== 8. 保存最终权值 ====================
    ofstream my_file2("权值.dat", ios::out);
    for (j = 0; j <= nInputNodes; j++) my_file2 << 0 << "\n"; // 分隔符0
    for (i = 0; i <= nHiddenNodes; i++) for (j = 0; j <= nInputNodes; j++) my_file2 << w1[i][j] << "\n";
    for (i = 0; i <= nOutputNodes - 1; i++) for (j = 0; j <= nHiddenNodes; j++) my_file2 << w2[i][j] << "\n";
    my_file2.close();

    cout << "训练已经结束" << endl;
    cout << "循环次数为:" << qq << endl;
    cout << "最终原始平均误差为:" << error1 << endl;

    // 释放动态内存
    for (int i = 0; i < PatternNo; i++) { delete[] out1[i]; delete[] delta1[i]; delete[] out2[i]; delete[] delta2[i]; delete[] x[i]; delete[] y[i]; }
    for (int i = 0; i < PatternNo + 2; i++) { delete[] out0[i]; delete[] target[i]; }
    for (int i = 0; i <= HiddenNo; i++) { delete[] delw1[i]; delete[] w1[i]; }
    for (int i = 0; i < OutputnNo; i++) { delete[] delw2[i]; delete[] w2[i]; }
    delete[] out1; delete[] delta1; delete[] out2; delete[] delta2; delete[] x; delete[] y; delete[] out0; delete[] target; delete[] delw1; delete[] w1; delete[] delw2; delete[] w2;

    system("pause");
    return 0;
}
