#include<iostream>
#include<vector>
#include<cmath>
#include<algorithm>
#include<random>
#include<fstream>
#include<opencv2/opencv.hpp>
using namespace std;

#define BUILDING_WIDTH 100
#define BUILDING_LENGTH 80
#define BUILDING_FLOORS 3
#define FLOOR_HEIGHT 3.5f

enum WallType {
	NONE = 0,
	BEARING_WALL, //承重墙
	PARTITION_WALL, //普通隔断墙
	GLASS_WALL //玻璃幕墙
};
const double ATTENUATION[3] = { 12.0,6.0,7.0 }; //衰减值
struct AP
{
	int x, y, floor;
	int channel_2_4GHz;
	int channel_5GHz;
	double tx_power;
	AP(int x, int y, int floor, double power = 20.0) :x(x), y(y), floor(floor), tx_power(power)
	{}
};
struct Point
{
	int x, y, floor;
	Point(int x, int y, int floor) :x(x), y(y), floor(floor)
	{}
};
class BuildingModel
{
private:
	vector<vector<vector<WallType>>> walls; //3D墙体矩阵
public:
	BuildingModel()
	{
		walls.resize(BUILDING_WIDTH, vector<vector<WallType>>(BUILDING_LENGTH, vector<WallType>(BUILDING_FLOORS, NONE)));
		// 墙体布置
		for (int x = 0; x < BUILDING_WIDTH; x++)
		{
			for (int floor = 0; floor < BUILDING_FLOORS; floor++)
			{
				if (x % 15 != 0)
				{
					walls[x][39][floor] = GLASS_WALL;
					walls[x][41][floor] = GLASS_WALL;
				}
				if (x == 0 || x == BUILDING_WIDTH - 1 || (x >= 39 && x <= 41))
				{
					for (int y = 0; y < BUILDING_LENGTH; y++)
					{
						if (y == 0 || y == BUILDING_LENGTH - 1)
						{
							walls[x][y][floor] = BEARING_WALL;
						}
					}
				}
				if (x % 15 == 0 && x > 0 && x < BUILDING_LENGTH)
				{
					for (int y = 0; y < BUILDING_LENGTH; y++)
					{
						if (y < 39 || y>41)
						{
							walls[x][y][floor] = BEARING_WALL;
						}
					}
				}
			}
		}
	}
	double calculateWallAttenuation(const Point& from, const Point& to)
	{
		double attenuation = 0;
		int dx = to.x - from.x;
		int dy = to.y - from.y;
		int steps = max(abs(dx), abs(dy));
		if (steps == 0)
			return 0;
		// 楼层间衰减，每层6dB
		if (from.floor != to.floor)
		{
			attenuation += 6.0 * abs(from.floor - to.floor);
		}
		for (int i = 0; i <= steps; i++)
		{
			int x = from.x + dx * i / steps;
			int y = from.y + dy * i / steps;
			if (x >= 0 && x < BUILDING_WIDTH && y >= 0 && y < BUILDING_LENGTH)
			{
				WallType wall = walls[x][y][from.floor];
				if(wall!=NONE)
					attenuation += ATTENUATION[wall - 1];
			}
		}
		return attenuation;
	}
};

class APPlanner
{
private:
	BuildingModel building;
	vector<AP> aps;
public:
	double calculateSignalStength(const AP& ap, const Point& point, double frequency)
	{
		double distance = sqrt(pow(ap.x - point.x, 2) + pow(ap.y - point.y, 2) + pow((ap.floor - point.floor) * FLOOR_HEIGHT, 2));
		if (distance == 0)
			return ap.tx_power;
		// 路径损耗模型
		double pathLoss;
		if (frequency == 2.4)
		{
			pathLoss = 40 + 20 * log10(distance) + 20 * log10(frequency);
		}
		else
		{
			pathLoss = 47 + 20 * log10(distance) + 20 * log10(frequency);
		}
		//墙体衰减
		double wallAttenuation = building.calculateWallAttenuation(Point(ap.x, ap.y, ap.floor), point);
		double signalStrength = ap.tx_power - pathLoss - wallAttenuation;
		return signalStrength;
	}
	void deployAps()
	{
		vector<Point> deplymentPoints = {
			// 第一层
			Point(10, 40, 0), Point(30, 40, 0), Point(50, 40, 0),
			Point(70, 40, 0), Point(90, 40, 0),
			// 第二层  
			Point(20, 40, 1), Point(40, 40, 1), Point(60, 40, 1), Point(80, 40, 1),
			// 第三层
			Point(15, 40, 2), Point(35, 40, 2), Point(55, 40, 2),
			Point(75, 40, 2), Point(95, 40, 2)
		};
		vector<int> channels_2_4GHz = { 1,6,11 };
		vector<int> channels_5GHz = { 36,40,44,48,149,153,157,161 };
		for (int i = 0; i < deplymentPoints.size(); i++)
		{
			AP ap(deplymentPoints[i].x, deplymentPoints[i].y, deplymentPoints[i].floor);
			ap.channel_2_4GHz = channels_2_4GHz[i % channels_2_4GHz.size()];
			ap.channel_5GHz = channels_5GHz[i % channels_5GHz.size()];
			aps.push_back(ap);
		}
	}
	void markAPs(cv::Mat& image, int floor)
	{
		for (const AP& ap : aps)
		{
			if (ap.floor == floor)
			{
				int x = ap.x * 5;
				int y = ap.y * 5;
				cv::circle(image, cv::Point(x, y), 10, cv::Scalar(255, 255, 255), 2);
				cv::circle(image, cv::Point(x, y), 8, cv::Scalar(0, 0, 0), -1);
				cv::putText(image, "AP", cv::Point(x - 8, y + 4), cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(255, 255, 255), 1);
			}
		}
	}
	void addHeatmapInfo(cv::Mat& image, const string& title)
	{
		cv::putText(image, title, cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
		vector<string> legends = { "-100dBm", "-80dBm", "-65dBm", "-50dBm", "-30dBm" };
		for (int i = 0; i < legends.size(); i++)
		{
			int x = 10 + i * 80;
			int y = image.rows - 10;
			putText(image, legends[i], cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
		}
		putText(image, "Coverage Threshold: -65dBm", cv::Point(image.cols - 200, 20), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(255, 255, 255), 1);
	}
	void generateCoverHeatmap()
	{
		ofstream file("coverage_heatmap.csv");
		file << "X,Y,Floor,Max_Signal_2_4GHz,Max_Signal_5Hz,Coverage_2_4GHz,Coverage_5GHz" << endl;

		vector<cv::Mat> heatmaps_2_4GHz(BUILDING_FLOORS);
		vector<cv::Mat> heatmaps_5GHz(BUILDING_FLOORS);
		for (int floor = 0; floor < BUILDING_FLOORS; floor++)
		{
			heatmaps_2_4GHz[floor] = cv::Mat::zeros(BUILDING_LENGTH, BUILDING_WIDTH, CV_32FC1);
			heatmaps_5GHz[floor] = cv::Mat::zeros(BUILDING_LENGTH, BUILDING_WIDTH, CV_32FC1);
		}
		for (int floor = 0; floor < BUILDING_FLOORS; floor++)
		{
			for (int x = 0; x < BUILDING_WIDTH; x += 2)
			{
				for (int y = 0; y < BUILDING_LENGTH; y += 2)
				{
					Point point(x, y, floor);
					double maxSignal_2_4 = -1000;
					double maxSignal_5 = -1000;
					for (const AP& ap : aps)
					{
						double signal_2_4 = calculateSignalStength(ap, point, 2.4);
						double signal_5 = calculateSignalStength(ap, point, 5.0);
						maxSignal_2_4 = max(maxSignal_2_4, signal_2_4);
						maxSignal_5 = max(maxSignal_5, signal_5);
					}

					bool coverage_2_4 = maxSignal_2_4 >= -65;
					bool coverage_5 = maxSignal_5 >= -65;
					file << x << "," << y << "," << floor << "," << maxSignal_2_4 << "," << maxSignal_5 << "," << (coverage_2_4 ? 1 : 0) << "," << (coverage_5 ? 1 : 0) << endl;
				
					heatmaps_2_4GHz[floor].at<float>(y, x) = maxSignal_2_4;
					heatmaps_5GHz[floor].at<float>(y, x) = maxSignal_5;
				}
			}
		}
		file.close();

		cv::Mat colormap;
		cv::applyColorMap(cv::Mat::zeros(1, 256, CV_8UC1), colormap, cv::COLORMAP_JET);
		for (int floor = 0; floor < BUILDING_FLOORS; floor++)
		{
			cv::Mat normalized_2_4, color_heatmap_2_4;
			normalize(heatmaps_2_4GHz[floor], normalized_2_4, 0, 255, cv::NORM_MINMAX, CV_8UC1);
			applyColorMap(normalized_2_4, color_heatmap_2_4, cv::COLORMAP_JET);
			cv::Mat normalized_5, color_heatmap_5;
			normalize(heatmaps_5GHz[floor], normalized_5, 0, 255, cv::NORM_MINMAX, CV_8UC1);
			applyColorMap(normalized_5, color_heatmap_5, cv::COLORMAP_JET);

			// 放大图像以便更好观察
			cv::Mat resized_2_4, resized_5;
			resize(color_heatmap_2_4, resized_2_4, cv::Size(), 5, 5, cv::INTER_LINEAR);
			resize(color_heatmap_5, resized_5, cv::Size(), 5, 5, cv::INTER_LINEAR);

			// 添加AP位置标记
			markAPs(resized_2_4, floor);
			markAPs(resized_5, floor);

			// 添加标题和颜色条
			addHeatmapInfo(resized_2_4, "2.4GHz Coverage - Floor " + to_string(floor + 1));
			addHeatmapInfo(resized_5, "5GHz Coverage - Floor " + to_string(floor + 1));

			// 保存图像
			imwrite("heatmap_2_4GHz_floor_" + to_string(floor + 1) + ".png", resized_2_4);
			imwrite("heatmap_5GHz_floor_" + to_string(floor + 1) + ".png", resized_5);

			// 显示图像
			imshow("2.4GHz Floor " + to_string(floor + 1), resized_2_4);
			imshow("5GHz Floor " + to_string(floor + 1), resized_5);
		}
		cv::waitKey(0);
	}

	void analyzeCoverage()
	{
		int totalPoints = 0;
		int coveredPoints_2_4 = 0;
		int coveredPoints_5 = 0;
		// 采样分析覆盖结果
		for (int floor = 0; floor < BUILDING_FLOORS; floor++)
		{
			for (int x = 5; x < BUILDING_WIDTH; x += 10)
			{
				for (int y = 5; y < BUILDING_LENGTH; y += 10)
				{
					totalPoints++;
					Point point(x, y, floor);
					double maxSignal_2_4 = -1000;
					double maxSignal_5 = -1000;
					for (const AP& ap : aps)
					{
						double signal_2_4 = calculateSignalStength(ap, point, 2.4);
						double signal_5 = calculateSignalStength(ap, point, 5.0);
						maxSignal_2_4 = max(maxSignal_2_4, signal_2_4);
						maxSignal_5 = max(maxSignal_5, signal_5);
					}
					if (maxSignal_2_4 >= -65)
						coveredPoints_2_4++;
					if (maxSignal_5 >= -65)
						coveredPoints_5++;
				}
			}
		}
		cout << endl;
		cout << "总采样点: " << totalPoints << endl;
		cout << "2.4GHz覆盖点: " << coveredPoints_2_4 << " (" << (coveredPoints_2_4 * 100.0 / totalPoints) << "%)" << endl;
		cout << "5GHz覆盖点: " << coveredPoints_5 << " (" << (coveredPoints_5 * 100.0 / totalPoints) << "%)" << endl;
	}


	void printAPDeployment()
	{
		for (const AP& ap : aps)
		{
			cout << "AP位置: (" << ap.x << ", " << ap.y << ", " << ap.floor << ")" << endl;
			cout << "2.4GHz信道: " << ap.channel_2_4GHz << endl;
			cout << "5GHz信道: " << ap.channel_5GHz << endl;
		}
	}
};
int main()
{
	APPlanner planner;
	planner.deployAps();
	planner.printAPDeployment();
	planner.analyzeCoverage();
	planner.generateCoverHeatmap();
	return 0;
}