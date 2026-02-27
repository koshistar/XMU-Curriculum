#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include<time.h>
#include<random>

using namespace cv;
using namespace std;

//模式选择
#define BINARY_MODE 0
#define OCTAL_MODE 1

//当前模式
int current_mode = BINARY_MODE;

//八进制颜色映射
//黑、白、红、蓝、绿、紫、黄、青：0、1、2、3、4、5、6、7
const vector<Scalar> octal_colors = {
	Scalar(0,0,0),Scalar(255,255,255),Scalar(0,0,255),Scalar(255,0,0),Scalar(0,255,0),Scalar(255,0,255),Scalar(0,255,255),Scalar(255,255,0)
};

//八进制编码
//message为0-8的整数
Scalar encodeOctal(int msg)
{
	return octal_colors[msg];
}

//二进制编码
//message为0（黑）或1（白）
Scalar encodeBinary(int msg)
{
	return msg == 1 ? Scalar(255, 255, 255) : Scalar(0, 0, 0);
}

//八进制解码
int decodeOctal(Scalar color)
{
	int res = 0;
	double distance = DBL_MAX;
	//根据每种颜色的距离返回相应值
	for (int i = 0; i < 8; i++)
	{
		double d = (color[0] - octal_colors[i][0]) * (color[0] - octal_colors[i][0]) + (color[1] - octal_colors[i][1]) * (color[1] - octal_colors[i][1]) + (color[2] - octal_colors[i][2]) * (color[2] - octal_colors[i][2]);
		if (d < distance)
		{
			distance = d;
			res = i;
		}
	}
	return res;
}

//二进制解码
int decodeBinary(Scalar color)
{
	//根据颜色返回相应值
	double brightness = (color[0] + color[1] + color[2]) / 3.0;
	return brightness > 127.0 ? 1 : 0;
}

//编码
Scalar encode(int msg)
{
	return current_mode == BINARY_MODE ? encodeBinary(msg) : encodeOctal(msg);
}
//解码
int decode(Scalar color)
{
	return current_mode == BINARY_MODE ? decodeBinary(color) : decodeOctal(color);
}

//通过摄像头和显示器进行模拟
//发送消息
void send(int msg)
{
	Scalar color = encode(msg);
	namedWindow("Transmitter", WINDOW_NORMAL);
	setWindowProperty("Transmitter", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
	Mat image(1080, 1920, CV_8UC3, color);
	imshow("Transmitter", image);
	waitKey(1000);//显示1s
	destroyWindow("Transmitter");
}
//接收消息
int receive()
{
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		cerr << "Can not open camera." << endl;
		return -1;
	}
	Mat frame;
	cap >> frame;
	cap.release();

	Rect center_region(frame.cols / 4, frame.rows / 4, frame.cols / 2, frame.rows / 2);
	Mat center = frame(center_region);
	Scalar color = mean(center);
	return decode(color);
}

//通过生成和解析图像文件进行模拟
void send1(int msg, const string& fimename = "transmitted_color.png")
{
	Scalar color = encode(msg);
	Mat image(200, 200, CV_8UC3, color);

	imwrite(fimename, image);
}
int receive1(const string& filename = "transmitted_color.png")
{
	Mat image = imread(filename);
	if (image.empty())
	{
		cerr << "Can not read image from " << filename << endl;
		return -1;
	}
	Scalar color = mean(image);
	return decode(color);
}

int main()
{
	srand(time(0));
	current_mode = BINARY_MODE;
	cout << "Binary mode." << endl;
	int val = rand() % 2;
	//cout << "Send " << val << "." << endl;
	//send(val);
	//cout << "Receive " << receive() << "." << endl;
	cout << "Send " << val << "." << endl;
	send1(val);
	cout << "Receive " << receive1() << "." << endl;
	cout << "Receive " << receive1("AcademyScheduleImage_5.png") << " from AcademyScheduleImage_5.png." << endl;

	cout << endl << "******************" << endl << endl;

	current_mode = OCTAL_MODE;
	cout << "Octal mode." << endl;
	val = rand() % 8;
	//cout << "Send " << val << "." << endl;
	//send(val);
	//cout << "Receive " << receive() << "." << endl;
	cout << "Send " << val << "." << endl;
	send1(val,"transmitted_color1.png");
	cout << "Receive " << receive1("transmitted_color1.png") << "." << endl;
	cout << "Receive " << receive1("AcademyScheduleImage_5.png") << " from AcademyScheduleImage_5.png." << endl;

	return 0;
}