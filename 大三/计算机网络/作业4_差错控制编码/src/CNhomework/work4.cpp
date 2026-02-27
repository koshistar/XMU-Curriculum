#include<iostream>
#include<time.h>

using namespace std;
// 声明接口
int parity_check(const unsigned char* msg, const int msg_length);

enum PARITYTYPE
{
	odd, even
};
PARITYTYPE parityType;
int parity_check(const unsigned char* msg, const int msg_length)
{
	if (msg == NULL || msg_length <= 0)
		return 0;

	int count1 = 0;
	for (int i = 0; i < msg_length; i++)
	{
		if (msg[i] != 0)
		{
			count1++;
		}
	}

	if (parityType == odd)
	{
		return (count1 & 1) ? 1 : 0;
	}
	else if (parityType == even)
	{
		return (count1 & 1) ? 0 : 1;
	}
}

int main()
{
	srand(time(0));
	parityType = odd;
	//prityType = even;
	const int len = 10;
	unsigned char msg[len];

	cout << "测试消息： ";
	for (int i = 0; i < len; i++)
	{
		msg[i] = rand() % 2;
		cout << (char)(msg[i] + '0') << " ";
	}
	cout << endl;
	cout << "校验结果：" << parity_check(msg, len);
	return 0;
}