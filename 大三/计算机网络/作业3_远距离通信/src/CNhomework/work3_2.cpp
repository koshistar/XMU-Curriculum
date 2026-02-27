#include<iostream>
#include<time.h>
#include<cmath>

using namespace std;

// 接口声明
int generate_cover_signal(double* cover, const int size); // unsigned double无效
int simulate_digital_modulation_signal(unsigned char* message, const int size);
int simulate_analog_modulation_signal(double* message, const int size);
int modulate_digital_frequency(double* cover, const int cover_len, const unsigned char* message, const int msg_len);
int modulate_analog_frequency(double* cover, const int cover_len, const double* message, const int msg_len);
int modulate_digital_amplitude(double* cover, const int cover_len, const unsigned char* message, const int msg_len);
int modulate_analog_amplitude(double* cover, const int cover_len, const double* message, const int msg_len);
int modulate_digital_phase(double* cover, const int cover_len, const unsigned char* message, const int msg_len);
int modulate_analog_phase(double* cover, const int cover_len, const double* message, const int msg_len);

#define PI 3.14159265358979323846
#define SAMPLE_RATE 44100 //采样率44.1kHz
#define CARRIER_FREQUENCY 1000 //载波频率 1000Hz
#define MESSAGE_FREQUENCY 100 //调制信号频率 100Hz

// 生成载波信号
int generate_cover_signal(double* cover, const int size)
{
	if (cover == 0 || size <= 0)
		return -1;
	
	for (int i = 0; i < size; i++)
	{
		cover[i] = sin(2 * PI * CARRIER_FREQUENCY * i / SAMPLE_RATE);
	}
	return size;
}

// 生成数字调制信号
int simulate_digital_modulation_signal(unsigned char* message, const int size)
{
	if (message == NULL || size <= 0)
		return -1;

	for (int i = 0; i < size; i++)
	{
		message[i] = rand() % 2;
	}
	return size;
}

// 生成模拟调制信号
int simulate_analog_modulation_signal(double* message, const int size)
{
	if (message == NULL || size <= 0)
		return -1;

	for (int i = 0; i < size; i++)
	{
		message[i] = sin(2 * PI * MESSAGE_FREQUENCY * i / SAMPLE_RATE);
	}
	return size;
}

// 数字调频
int modulate_digital_frequency(double* cover, const int cover_len, const unsigned char* message, const int msg_len)
{
	if (cover == NULL || message == NULL || cover_len <= 0 || msg_len <= 0)
		return -1;
	
	int samples_per_bit = cover_len / msg_len;
	double frequence_0 = CARRIER_FREQUENCY * 0.8;
	double frequence_1 = CARRIER_FREQUENCY * 1.2;
	for (int i = 0; i < cover_len; i++)
	{
		int bit_index = i / samples_per_bit;
		if (bit_index >= msg_len)
			break;
		double frequency = (message[bit_index] == 0) ? frequence_0 : frequence_1;
		cover[i] = sin(2 * PI * frequency * i / SAMPLE_RATE);
	}
	return cover_len;
}

// 模拟调频
int modulate_analog_frequency(double* cover, const int cover_len, const double* message, const int msg_len)
{
	if (cover == NULL || message == NULL || cover_len <= 0 || msg_len <= 0)
		return -1;
	const double modulation_index = 5.0; //调制指数
	double max_frequency_deviation = modulation_index * MESSAGE_FREQUENCY;
	for (int i = 0; i < cover_len; i++)
	{
		int msg_index = (i * msg_len) / cover_len;
		if (msg_index >= msg_len)
			msg_index = msg_len - 1;
		double instantaneous_frequency = CARRIER_FREQUENCY + max_frequency_deviation * message[msg_index];
		cover[i] = sin(2 * PI * instantaneous_frequency * i / SAMPLE_RATE);
	}
	return cover_len;
}

// 数字调幅
int modulate_digital_amplitude(double* cover, const int cover_len, const unsigned char* message, const int msg_len)
{
	if (cover == NULL || message == NULL || cover_len <= 0 || msg_len <= 0)
		return -1;
	int samples_per_bit = cover_len / msg_len;
	double amplitude_0 = 0.2;
	double amplitude_1 = 1.0;
	for (int i = 0; i < cover_len; i++)
	{
		int bit_index = i / samples_per_bit;
		if (bit_index >= msg_len)
			break;
		double amplitude = (message[bit_index] == 0) ? amplitude_0 : amplitude_1;
		cover[i] = amplitude * sin(2 * PI * CARRIER_FREQUENCY * i / SAMPLE_RATE);
	}
	return cover_len;
}

// 模拟调幅
int modulate_analog_amplitude(double* cover, const int cover_len, const double* message, const int msg_len)
{
	if (cover == NULL || message == NULL || cover_len <= 0 || msg_len <= 0)
		return -1;
	const double modulation_depth = 0.8;//调制深度
	for (int i = 0; i < cover_len; i++)
	{
		int msg_index = (i * msg_len) / cover_len;
		if (msg_index >= msg_len)
		{
			msg_index = msg_len - 1;
		}
		double amplitude = 1.0 + modulation_depth * message[msg_index];
		cover[i] = amplitude * sin(2 * PI * CARRIER_FREQUENCY * i / SAMPLE_RATE);
	}
	return cover_len;
}

// 数字调相
int modulate_digital_phase(double* cover, const int cover_len, const unsigned char* message, const int msg_len)
{
	if (cover == NULL || message == NULL || cover_len <= 0 || msg_len <= 0)
		return -1;

	int samples_per_bit = cover_len / msg_len;
	double phase_0 = 0;
	double phase_1 = PI;

	for (int i = 0; i < cover_len; i++)
	{
		int bit_index = i / samples_per_bit;
		if (bit_index >= msg_len)
			break;
		double phase = (message[bit_index] == 0) ? phase_0 : phase_1;
		cover[i] = sin(2 * PI * CARRIER_FREQUENCY * i / SAMPLE_RATE + phase);
	}
	return cover_len;
}

// 模拟调相
int modulate_analog_phase(double* cover, const int cover_len, const double* message, const int msg_len)
{
	if (cover == NULL || message == NULL || cover_len <= 0 || msg_len <= 0)
		return -1;
	
	double modulation_index = 2.0;
	for (int i = 0; i < cover_len; i++)
	{
		int msg_index = (i * msg_len) / cover_len;
		if (msg_index >= msg_len)
		{
			msg_index = msg_len - 1;
		}
		double phase = modulation_index * message[msg_index];
		cover[i] = sin(2 * PI * CARRIER_FREQUENCY * i / SAMPLE_RATE + phase);
	}
	return cover_len;
}

int main()
{
	srand(time(0));

	const int digital_msg_len = 100;
	unsigned char digital_msg[digital_msg_len];
	const int analog_msg_len = 100;
	double analog_msg[analog_msg_len];
	double cover[1000];
	simulate_digital_modulation_signal(digital_msg, digital_msg_len);
	simulate_analog_modulation_signal(analog_msg, analog_msg_len);

	const int show_num = 50;//显示前多少个采样

	cout << "Digital message: ";
	for (int i = 0; i < show_num; i++)
	{
		cout << (int)digital_msg[i] << " ";
	}
	cout << endl;

	cout << "Modulate digital frequency: ";
	generate_cover_signal(cover, 1000);
	modulate_digital_frequency(cover, 1000, (const unsigned char*)digital_msg, digital_msg_len);
	for (int i = 0; i < show_num; i++)
	{
		cout << cover[i] << " ";
	}
	cout << endl;

	memset(cover, 0, 1000);
	cout << "Modulate digital Amplitude: ";
	generate_cover_signal(cover, 1000);
	modulate_digital_amplitude(cover, 1000, (const unsigned char*)digital_msg, digital_msg_len);
	for (int i = 0; i < show_num; i++)
	{
		cout << cover[i] << " ";
	}
	cout << endl;

	memset(cover, 0, 1000);
	cout << "Modulate digital phase: ";
	generate_cover_signal(cover, 1000);
	modulate_digital_phase(cover, 1000, (const unsigned char*)digital_msg, digital_msg_len);
	for (int i = 0; i < show_num; i++)
	{
		cout << cover[i] << " ";
	}
	cout << endl;

	memset(cover, 0, 1000);
	cout << "Modulate analog frequency: ";
	generate_cover_signal(cover, 1000);
	modulate_analog_frequency(cover, 1000, (const double*)analog_msg, analog_msg_len);
	for (int i = 0; i < show_num; i++)
	{
		cout << cover[i] << " ";
	}
	cout << endl;

	memset(cover, 0, 1000);
	cout << "Modulate analog amplitude: ";
	generate_cover_signal(cover, 1000);
	modulate_analog_amplitude(cover, 1000, (const double*)analog_msg, analog_msg_len);
	for (int i = 0; i < show_num; i++)
	{
		cout << cover[i] << " ";
	}
	cout << endl;

	memset(cover, 0, 1000);
	cout << "Modulate analog phase: ";
	generate_cover_signal(cover, 1000);
	modulate_analog_phase(cover, 1000, (const double*)analog_msg, analog_msg_len);
	for (int i = 0; i < show_num; i++)
	{
		cout << cover[i] << " ";
	}
	cout << endl;
	return 0;
}