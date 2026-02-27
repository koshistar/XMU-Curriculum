#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<opencv2/opencv.hpp>

#define PI 3.1415926
using namespace std;
using namespace cv;

double originalSignal(double t, double f1, double f2)
{
	return sin(2 * PI * f1 * t) + 0.5 * sin(2 * PI * f2 * t);
}
vector<double> sampleSignal(double fs, double duration, double f1, double f2)
{
	vector<double> res;
	double dt = 1.0 / fs;
	for (int i = 0; i <= duration * fs; i++)
	{
		res.push_back(originalSignal(i * dt, f1, f2));
	}
	return res;
}
double reconstructSignal(double t, const vector<double>& samples, double fs)
{
	double reconstructed = 0.0;
	double T = 1.0 / fs;
	int N = samples.size();
	for (int n = 0; n < N; n++)
	{
		double nT = n * T;
		if (fabs(t - nT) < 1e-10)
		{
			return samples[n];
		}
		reconstructed += samples[n] * sin(PI * (t - nT) / T) / (PI * (t - nT) / T);
	}
	return reconstructed;
}
double calculateError(const vector<double>& original, const vector<double>& reconstructed)
{
	double error = 0.0;
	int N = min(original.size(), reconstructed.size());
	for (int i = 0; i < N; i++)
	{
		error += pow(original[i] - reconstructed[i], 2);
	}
	return sqrt(error / N);
}
void plotSignals(const vector<double>& time, const vector<double> original, const vector<double>& reconstructed, const vector<double>& sample_times, const vector<double>& samples, const string& title, double fs, double nyquist_frequency)
{
	int width = 800;
	int height = 400;
	Mat image(height, width, CV_8UC3, Scalar(255, 255, 255));

	double min_time = *min_element(time.begin(), time.end());
	double max_time = *max_element(time.begin(), time.end());
	double min_val = -2.0;
	double max_val = 2.0;

	auto mapX = [&](double t) {return (t - min_time) / (max_time - min_time) * (width - 100) + 50; };
	auto mapY = [&](double y) {return height - 50 - (y - min_val) / (max_val - min_val) * (height - 100); };

	//坐标轴
	line(image, Point(50, height - 50), Point(width - 50, height - 50), Scalar(0, 0, 0), 2);
	line(image, Point(50, height - 50), Point(50, 50), Scalar(0, 0, 0), 2);
	//网格
	for (double t = min_time; t <= max_time; t += 0.1) {
		int x = mapX(t);
		line(image, Point(x, height - 50), Point(x, 50), Scalar(200, 200, 200), 1);
	}
	for (double y = min_val; y <= max_val; y += 0.5) {
		int y_pix = mapY(y);
		line(image, Point(50, y_pix), Point(width - 50, y_pix), Scalar(200, 200, 200), 1);
	}

	//蓝色原始信号
	for (size_t i = 1; i < time.size(); i++) {
		Point p1(mapX(time[i - 1]), mapY(original[i - 1]));
		Point p2(mapX(time[i]), mapY(original[i]));
		line(image, p1, p2, Scalar(255, 0, 0), 2);
	}
	//红色重建信号
	for (size_t i = 1; i < time.size(); i++) {
		Point p1(mapX(time[i - 1]), mapY(reconstructed[i - 1]));
		Point p2(mapX(time[i]), mapY(reconstructed[i]));
		line(image, p1, p2, Scalar(0, 0, 255), 2);
	}
	//绿色采样点
	for (size_t i = 0; i < sample_times.size(); i++) {
		Point center(mapX(sample_times[i]), mapY(samples[i]));
		circle(image, center, 4, Scalar(0, 255, 0), -1);
	}

	string full_title = title + " (fs = " + to_string(fs) + "Hz, Nyquist = " + to_string(nyquist_frequency) + "Hz)";
	putText(image, full_title, Point(100, 30), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 0), 2);
	putText(image, "Time (s)", Point(width / 2 - 30, height - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
	putText(image, "Amplitude", Point(10, height / 2), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);

	//图例
	rectangle(image, Point(width - 180, 60), Point(width - 20, 130), Scalar(200, 200, 200), -1);
	rectangle(image, Point(width - 180, 60), Point(width - 20, 130), Scalar(0, 0, 0), 1);
	putText(image, "Legend:", Point(width - 170, 80), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
	line(image, Point(width - 160, 95), Point(width - 140, 95), Scalar(255, 0, 0), 2);
	putText(image, "Original", Point(width - 130, 100), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
	line(image, Point(width - 160, 110), Point(width - 140, 110), Scalar(0, 0, 255), 2);
	putText(image, "Reconstructed", Point(width - 130, 115), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);
	circle(image, Point(width - 150, 125), 4, Scalar(0, 255, 0), -1);
	putText(image, "Samples", Point(width - 130, 130), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 0), 1);

	imshow(title, image);
	waitKey(100);
	string filename = "nyquist_fs_" + to_string(int(fs)) + "Hz.png";
	imwrite(filename, image);
}
int main()
{
	// 信号参数
	double f1 = 5.0;
	double f2 = 8.0;
	double f_max = max(f1, f2);
	double nyquist_freq = 2 * f_max;

	double duration = 1.0;
	double dt_continuous = 0.001;

	cout << "Signal frequency: " << f1 << "Hz and " << f2 << "Hz." << endl;
	cout << "Max frequency: " << f_max << "Hz." << endl;
	cout << "Nyquit frequency: " << nyquist_freq << "Hz." << endl;

	vector<double> testFrequency = {
		30.0,20.0,16.0,12.0,8.0 //大于/等于/小于2倍
	};
	vector<double> time_continuous;
	vector<double> signal_continuous;
	for (double t = 0; t <= duration; t += dt_continuous)
	{
		time_continuous.push_back(t);
		signal_continuous.push_back(originalSignal(t, f1, f2));
	}

	namedWindow("Nyquist sampling", WINDOW_AUTOSIZE);
	for (int i = 0; i < testFrequency.size(); i++)
	{
		double fs = testFrequency[i];
		cout << "Test " << i+1 << ", frequency = " << fs << endl;

		// 采样
		vector<double> samples = sampleSignal(fs, duration, f1, f2);
		vector<double> sample_times;
		for (int j = 0; j < samples.size(); j++)
		{
			sample_times.push_back(j * (1.0 / fs));
		}

		// 重建
		vector<double> signal_reconstructed;
		for (double t = 0; t <= duration; t += dt_continuous)
		{
			signal_reconstructed.push_back(reconstructSignal(t, samples, fs));
		}

		double error = calculateError(signal_continuous, signal_reconstructed);
		cout << "Error: " << error << endl;

		plotSignals(time_continuous, signal_continuous, signal_reconstructed, sample_times, samples, "test" + to_string(i+1), fs, nyquist_freq);
	}
	waitKey(0);
	destroyAllWindows();
	return 0;
}