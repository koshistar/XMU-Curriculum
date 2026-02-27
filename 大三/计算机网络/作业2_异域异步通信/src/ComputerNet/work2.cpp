#include<iostream>
#include<cmath>
#include<string>

using namespace std;

// 接口函数声明
int re232c_encode(double* volts, int volts_size, const char* msg, int size);
int rs232c_decode(char* msg, int size, const double* volts, int volts_size);

#define IDLE_VOLTAGE -15.0
#define START_VOLTAGE 15.0
#define STOP_VOLTAGE -15.0
#define BIT0_VOLTAGE 15.0
#define BIT1_COLTAGE -15.0

int re232c_encode(double* volts, int volts_size, const char* msg, int size)
{
	if (volts == NULL || msg == NULL || volts_size < size * 11 || size <= 0)
	{
		return -1;
	}

	int index = 0;
	for (int i = 0; i < size; i++)
	{
		char c = msg[i];

		if (index < volts_size)
			volts[index++] = IDLE_VOLTAGE;// 空闲位
		if (index < volts_size)
			volts[index++] = START_VOLTAGE;// 起始位
		for (int bit = 0; bit < 7; bit++)
		{
			if (index >= volts_size)
				break;
			int val = (c >> bit) & 1;// 位小端序
			volts[index++] = (val == 0) ? BIT0_VOLTAGE : BIT1_COLTAGE;
		}// 数据位
		if (index < volts_size)
			volts[index++] = STOP_VOLTAGE;// 停止位
		if (index < volts_size)
			volts[index++] = IDLE_VOLTAGE;// 空闲位
	}
	return index;
}

int rs232c_decode(char* msg, int size, const double* volts, int volts_size)
{
	if (msg == NULL || volts == NULL || size <= 0 || volts_size <= 0)
	{
		return -1;
	}

	int msg_index = 0;
	int volt_index = 0;
	bool in_frame = false;
	int bit_count = 0;
	char c = 0;
	while (volt_index < volts_size && msg_index < size)
	{
		double voltage = volts[volt_index++];
		if (!in_frame && voltage >= 3.0f)
		{
			in_frame = true;
			bit_count = 0;
			c = 0;
			continue;
		}// 检测起始位
		if (in_frame)
		{
			if (bit_count < 7)
			{
				if (voltage >= 3.0)
				{
					//c &= ~(1 << bit_count);// 多余操作，可省去
				}
				else if (voltage <= -3.0)
				{
					c |= (1 << bit_count);
				}
				bit_count++;
			}// 处理数据位
			else if (bit_count == 7)
			{
				if (voltage <= -3.0)
				{
					msg[msg_index++] = c;
					in_frame = false;
				}
				else
				{
					cout << "Error in decode." << endl;// 报错但继续执行
					in_frame = false;
				}
			}// 检测停止位
		}
	}
	return msg_index;
}

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++)
	{
		int msg_size = strlen(argv[i]);
		for (int j = 0; j < msg_size; j++)
		{
			cout << "测试字符" << argv[i][j] << ",ASCII码为" << (int)argv[i][j] << endl;
			cout << "二进制表示为：";
			for (int k = 6; k >= 0; k--)
			{
				cout << (int)((argv[i][j] >> k) & 1);
			}
			cout << endl;
		}
		int volts_size = msg_size * 11;
		double* volts = new double[volts_size];
		int volts_count = re232c_encode(volts, volts_size, argv[i], msg_size);
		cout << "序号 | 电压 | 含义" << endl;
		for (int j = 0; j < volts_count; j++)
		{
			string message;
			int t = j % 11;
			if (t == 0 || t == 10) message = "空闲位";
			else if (t == 1) message = "起始位";
			else if (t >= 2 && t <= 8) message = "数据位";
			else if (t == 9) message = "停止位";
			cout << j << "  " << volts[j] << "  " << message << endl;
		}
		char* decoded_msg = new char[msg_size + 1];
		memset(decoded_msg, 0, msg_size + 1);
		int decoded_count = rs232c_decode(decoded_msg, msg_size, volts, volts_count);
		if (decoded_count > 0)
			cout << "解码成功：" << decoded_msg << "，解码字符数：" << decoded_count << endl;
		else
			cout << "解码失败" << endl;
	}
	return 0;
}