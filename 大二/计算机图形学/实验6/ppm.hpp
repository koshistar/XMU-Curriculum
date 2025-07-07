#pragma once
#include <stdio.h>
#include<iostream>
#include<cstdio>
using namespace std;

class PPM
{
public:
	PPM(int width=1280, int height=640)
	{
		_nWidth = width;
		_nHeight = height;
		_nChannel = 3;
		_rgb_data = new unsigned char[3*_nWidth*_nHeight];
	}

	//深拷贝
	PPM(const PPM& other) : _nWidth(other._nWidth), _nHeight(other._nHeight), _nChannel(other._nChannel)
	{
		_rgb_data = new unsigned char[_nWidth * _nHeight * _nChannel];
		memcpy(_rgb_data, other._rgb_data, _nWidth * _nHeight * _nChannel);
	}
	PPM& operator=(const PPM& other)
	{
		if (this == &other) return *this; // 自我赋值检查

		// 先释放旧内存
		delete[] _rgb_data;

		// 拷贝尺寸和数据
		_nWidth = other._nWidth;
		_nHeight = other._nHeight;
		_nChannel = other._nChannel;
		_rgb_data = new unsigned char[_nWidth * _nHeight * _nChannel];
		memcpy(_rgb_data, other._rgb_data, _nWidth * _nHeight * _nChannel);

		return *this;
	}

	~PPM(){
		delete []_rgb_data;
	}

	void SetPixel(int row, int col, unsigned r, unsigned g, unsigned b)
	{
		int idx = 3 * (row * _nWidth + col);
		_rgb_data[idx + 0] = r;
		_rgb_data[idx + 1] = g;
		_rgb_data[idx + 2] = b;
	}

	//增加获得当前颜色值的函数
	void GetPixel(int row, int col, unsigned& r, unsigned& g, unsigned& b)
	{
		int idx = 3 * (row * _nWidth + col);
		g = _rgb_data[idx + 1];
		b = _rgb_data[idx + 2];		r = _rgb_data[idx + 0];

	}

	void Write2File(const char* filename = "output.ppm")
	{
		cout << 123 << endl;
		FILE* fp;
		fopen_s(&fp, filename, "w");
		fprintf(fp, "P3\n%d %d\n255\n", _nWidth, _nHeight);
		int idx = 0;
		for (int j = _nHeight - 1; j >= 0; --j)
		{
			for (int i = 0; i < _nWidth; ++i)
			{
				idx = 3*(j * _nWidth + i);
				fprintf_s(fp, "%d %d %d\n", _rgb_data[idx], _rgb_data[idx + 1], _rgb_data[idx + 2]);
			}
		}

		fclose(fp);
	}

private:
	unsigned char* _rgb_data;
	int _nHeight;
	int _nWidth;
	int _nChannel;
};