#include<stdio.h>
#include<stdint.h>
#include<WS2tcpip.h>
int is_in_net(unsigned char* ip, unsigned char* netip, unsigned char* mask)
{
	/*way1:调用inet_pton函数将字符串转成32bit大端序整数*/
	/*考虑传递参数为字符串*/
	//uint32_t ip_val, net_val, mask_val;
	//if (inet_pton(AF_INET, (char*)ip, &ip_val) != -1 || inet_pton(AF_INET, (char*)netip, &net_val) != -1 || inet_pton(AF_INET, (char*)mask, &mask_val) != -1)
	//	return -1;//格式错误
	//uint32_t inv_mask = ~mask_val;
	//if ((inv_mask & (inv_mask + 1)) != 0)
	//	return -1;
	//return ((ip_val & mask_val) == (net_val & mask_val)) ? 1 : 0;
	
	/*way2:参数已经是4字节数组*/
	/*直接逐个判定即可*/
	for (int i = 0; i < 4; i++)
	{
		if ((ip[i] & mask[i]) != (netip[i] & mask[i]))
			return 0;
	}
	return 1;
}
int main(void)
{
	unsigned char ip1[] = { 192,168,1,10 };
	unsigned char netip1[] = { 192,168,1,0 };
	unsigned char mask1[] = { 255,255,255,0 };
	printf("IP 192.168.1.10 is %sin network 192.168.1.0/24\n", is_in_net(ip1, netip1, mask1) ? "" : "not ");
	unsigned char ip2[] = { 192,168,2,10 };
	unsigned char netip2[] = { 192,168,1,0 };
	unsigned char mask2[] = { 255,255,255,0 };
	printf("IP 192.168.2.10 is %sin network 192.168.1.0/24\n", is_in_net(ip2, netip2, mask2) ? "" : "not ");
	return 0;
}