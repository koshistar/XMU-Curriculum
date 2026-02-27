// https://blog.csdn.net/weixin_44081384/article/details/126707216
#include<stdint.h>
struct Ethernet_frame_header
{
	uint8_t destination_address[6]; // 目的地址
	uint8_t source_address[6]; // 源地址
	uint16_t ethertype; // 类型
};