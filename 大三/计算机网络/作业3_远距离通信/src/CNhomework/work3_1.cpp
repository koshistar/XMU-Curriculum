#include<iostream>
#include<cmath>
#include<algorithm>
#include<string.h>

using namespace std;

// 接口声明
int multiplex(unsigned char* c, const int c_size, const unsigned char* a, const int a_len, const unsigned char* b, const int b_len);
int demultiplex(unsigned char* a, const int a_size, unsigned char* b, const int b_size, const unsigned char* c, const int c_len);

// 统计时分多路复用
int multiplex_STDM(unsigned char* c, const int c_size, const unsigned char* a, const int a_len, const unsigned char* b, const int b_len)
{
	int i = 0, j = 0, k = 0;
	while ((i < a_len || j < b_len) && k < c_size - 1) {
		if (i < a_len)
		{
			c[k++] = a[i++];
			c[k++] = 0;
		}
		if (j < b_len)
		{
			c[k++] = b[j++];
			c[k++] = 1;
		}
	}
	return k;
}
int demultiplex_STDM(unsigned char* a, const int a_size, unsigned char* b, const int b_size, const unsigned char* c, const int c_len)
{
	int i = 0, j = 0, k = 0;
	while (k < c_len - 1)
	{
		unsigned char data = c[k++];
		unsigned char source = c[k++];
		if (source == 0 && i < a_size)
		{
			a[i++] = data;
		}
		else if (source == 1 && j < b_size)
		{
			b[j++] = data;
		}
	}
	return i + j;
}
// 同步时分多路复用
int multiplex_TDM(unsigned char* c, const int c_size, const unsigned char* a, const int a_len, const unsigned char* b, const int b_len)
{
	int max_len = max(a_len, b_len);
	int k = 0;
	for (int i = 0; i < max_len && k < c_size - 1; i++)
	{
		c[k++] = i < a_len ? a[i] : 0;
		c[k++] = i < b_len ? b[i] : 0;
	}
	return k;
}
int demultiplex_TDM(unsigned char* a, const int a_size, unsigned char* b, const int b_size, const unsigned char* c, const int c_len)
{
	int i = 0, j = 0;
	for (int k = 0; k < c_len - 1; k += 2)
	{
		if (i < a_size)
		{
			a[i++] = c[k];
		}
		if (j < b_size && k + 1 < c_len)
		{
			b[j++] = c[k + 1];
		}
	}
	return i + j;
}
// 频分多路复用
int multiplex_FDM(unsigned char* c, const int c_size, const unsigned char* a, const int a_len, const unsigned char* b, const int b_len)
{
	int k = 0;
	int max_len = max(a_len, b_len);
	for (int i = 0; i < max_len && k < c_size - 2; i++)
	{
		if (i < a_len)
		{
			c[k++] = a[i];
		}
		else
		{
			k++;
		}
		if (i < b_len)
		{
			c[k++] = b[i];
		}
		else
		{
			k++;
		}
	}
	return k;
}
int demultiplex_FDM(unsigned char* a, const int a_size, unsigned char* b, const int b_size, const unsigned char* c, const int c_len)
{
	int i = 0, j = 0;
	for (int k = 0; k < c_len; k++)
	{
		if (!(k & 1) && i < a_size)
		{
			a[i++] = c[k];
		}
		else if (j < b_size)
		{
			b[j++] = c[k];
		}
	}
	return i + j;
}
// 码分多路复用
// 正交码
#define CDM_CODE_A 0x55 // 01010101
#define CDM_CODE_B 0xAA // 10101010
int multiplex_CDM(unsigned char* c, const int c_size, const unsigned char* a, const int a_len, const unsigned char* b, const int b_len)
{
	int max_len = max(a_len, b_len);
	int k = 0;
	for (int i = 0; i < max_len && k < c_size; i++)
	{
		unsigned char encoded = 0;
		if (i < a_len)
		{
			encoded |= (a[i] != '0') ? CDM_CODE_A : 0;
		}
		if (i < b_len)
		{
			encoded |= (b[i] != '0') ? CDM_CODE_B : 0;
		}
		c[k++] = encoded;
	}
	return k;
}
int demultiplex_CDM(unsigned char* a, const int a_size, unsigned char* b, const int b_size, const unsigned char* c, const int c_len)
{
	int i = 0, j = 0;
	for (int k = 0; k < c_len; k++)
	{
		if (i < a_size)
		{
			a[i++] = (c[k] & CDM_CODE_A) != 0 ? '1' : '0';
		}
		if (j < b_size)
		{
			b[j++] = (c[k] & CDM_CODE_B) != 0 ? '1' : '0';
		}
	}
	return i + j;
}
// 编码
enum Type
{
	STDM,TDM,FDM,CDM
};
Type type;
int multiplex(unsigned char* c, const int c_size, const unsigned char* a, const int a_len, const unsigned char* b, const int b_len)
{
	switch (type)
	{
	case STDM:return multiplex_STDM(c, c_size, a, a_len, b, b_len); break;
	case TDM:return multiplex_TDM(c, c_size, a, a_len, b, b_len); break;
	case FDM:return multiplex_FDM(c, c_size, a, a_len, b, b_len); break;
	case CDM:return multiplex_CDM(c, c_size, a, a_len, b, b_len); break;
	default:
		return -1;
		break;
	}
}
int demultiplex(unsigned char* a, const int a_size, unsigned char* b, const int b_size, const unsigned char* c, const int c_len)
{
	switch (type)
	{
	case STDM:return demultiplex_STDM(a, a_size, b, b_size, c, c_len);
		break;
	case TDM: return demultiplex_TDM(a, a_size, b, b_size, c, c_len);
		break;
	case FDM: return demultiplex_FDM(a, a_size, b, b_size, c, c_len);
		break;
	case CDM: return demultiplex_CDM(a, a_size, b, b_size, c, c_len);
		break;
	default:
		break;
	}
}
int main(int argc, char* argv[])
{
	unsigned char c[20] = { 0 };
	unsigned char a[10] = { 0 };
	unsigned char b[10] = { 0 };
	int a_len_origin = strlen(argv[1]);
	int b_len_origin = strlen(argv[2]);
	int c_len;
	int total;
	cout << "Orginal a: ";
	for (int i = 0; i < a_len_origin; i++)
	{
		cout << argv[1][i] << " ";
	}
	cout << endl << "Orginal b: ";
	for (int i = 0; i < b_len_origin; i++)
	{
		cout << argv[2][i] << " ";
	}
	cout << endl;

	type = STDM;
	memset(c, 0, 20);
	memset(a, 0, 10);
	memset(b, 0, 10);
	cout << "STDM:" << endl;
	c_len = multiplex(c, 20, (unsigned char*)argv[1], strlen(argv[1]), (unsigned char*)argv[2], strlen(argv[2]));
	total = demultiplex(a, 10, b, 10, c, c_len);
	cout << "Encode: ";
	for (int i = 0; i < c_len; i++)
	{
		cout << c[i] << " ";
	}
	cout << "\nDecoded a: ";
	for (int i = 0; i < a_len_origin; i++)
	{
		cout << a[i] << " ";
	}
	cout << "\nDecoded b: ";
	for (int i = 0; i < b_len_origin; i++)
	{
		cout << b[i] << " ";
	}
	cout << endl;

	type = TDM;
	memset(c, 0, 20);
	memset(a, 0, 10);
	memset(b, 0, 10);
	cout << "TDM:" << endl;
	c_len = multiplex(c, 20, (unsigned char*)argv[1], strlen(argv[1]), (unsigned char*)argv[2], strlen(argv[2]));
	total = demultiplex(a, 10, b, 10, c, c_len);
	cout << "Encode: ";
	for (int i = 0; i < c_len; i++)
	{
		cout << c[i] << " ";
	}
	cout << "\nDecoded a: ";
	for (int i = 0; i < a_len_origin; i++)
	{
		cout << a[i] << " ";
	}
	cout << "\nDecoded b: ";
	for (int i = 0; i < b_len_origin; i++)
	{
		cout << b[i] << " ";
	}
	cout << endl;

	type = FDM;
	memset(c, 0, 20);
	memset(a, 0, 10);
	memset(b, 0, 10);
	cout << "FDM:" << endl;
	c_len = multiplex(c, 20, (unsigned char*)argv[1], strlen(argv[1]), (unsigned char*)argv[2], strlen(argv[2]));
	total = demultiplex(a, 10, b, 10, c, c_len);
	cout << "Encode: ";
	for (int i = 0; i < c_len; i++)
	{
		cout << c[i] << " ";
	}
	cout << "\nDecoded a: ";
	for (int i = 0; i < a_len_origin; i++)
	{
		cout << a[i] << " ";
	}
	cout << "\nDecoded b: ";
	for (int i = 0; i < b_len_origin; i++)
	{
		cout << b[i] << " ";
	}
	cout << endl;

	type = CDM;
	memset(c, 0, 20);
	memset(a, 0, 10);
	memset(b, 0, 10);
	cout << "CDM:" << endl;
	c_len = multiplex(c, 20, (unsigned char*)argv[1], strlen(argv[1]), (unsigned char*)argv[2], strlen(argv[2]));
	total = demultiplex(a, 10, b, 10, c, c_len);
	cout << "Encode: ";
	for (int i = 0; i < c_len; i++)
	{
		cout << c[i] << " ";
	}
	cout << "\nDecoded a: ";
	for (int i = 0; i < a_len_origin; i++)
	{
		cout << a[i] << " ";
	}
	cout << "\nDecoded b: ";
	for (int i = 0; i < b_len_origin; i++)
	{
		cout << b[i] << " ";
	}
	cout << endl;
	return 0;
}
