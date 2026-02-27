#include<stdio.h>
#include<stdint.h>
/*
A类：0.0.0.0-127.255.255.255
B类：128.0.0.0-191.255.255.255
C类：192.0.0.0-223.255.255.255
D类：224.0.0.0-239.255.255.255
E类：240.0.0.0-255.255.255.255
*/
int classwise(unsigned char* ip)
{
	uint8_t first_byte = ip[0];
	if (first_byte < 0)
		return -1;

	if (first_byte <= 127)
		return 0;
	else if (first_byte <= 191)
		return 1;
	else if (first_byte <= 223)
		return 2;
	else if (first_byte <= 239)
		return 3;
	else if (first_byte <= 255)
		return 4;
	else
		return -1;
}
char* class_to_string(int x)
{
	switch (x)
	{
	case 0:return "A";
	case 1:return "B";
	case 2:return "C";
	case 3:return "D";
	case 4:return "E";
	default:return "None";
	}
}
int main()
{
	unsigned char ip0[] = { 10,0,0,1 };
	printf("IP 10.0.0.1 is type %s\n", class_to_string(classwise(ip0)));
	unsigned char ip1[] = { 128,0,0,1 };
	printf("IP 128.0.0.1 is type %s\n", class_to_string(classwise(ip1)));
	unsigned char ip2[] = { 192,168,1,1 };
	printf("IP 192.168.1.1 is type %s\n", class_to_string(classwise(ip2)));
	unsigned char ip3[] = { 224,0,0,1 };
	printf("IP 224.0.0.1 is type %s\n", class_to_string(classwise(ip3)));
	unsigned char ip4[] = { 240,0,0,1 };
	printf("IP 240.0.0.1 is type %s\n", class_to_string(classwise(ip4)));
	unsigned char ip5[] = { 127,0,0,1 };
	printf("IP 127.0.0.1 is type %s\n", class_to_string(classwise(ip5)));
	unsigned char ip6[] = { 172,16,0,1 };
	printf("IP 172.16.0.1 is type %s\n", class_to_string(classwise(ip6)));
	unsigned char ip7[] = { 203,0,113,1 };
	printf("IP 203.0.113.1 is type %s\n", class_to_string(classwise(ip7)));
	unsigned char ip8[] = { 225,225,225,225 };
	printf("IP 225.225.225.225 is type %s\n", class_to_string(classwise(ip8)));
	return 0;
}