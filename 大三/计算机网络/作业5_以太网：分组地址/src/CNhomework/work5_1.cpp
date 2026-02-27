#include<iostream>
#include<cstring>
#include<iomanip>

using namespace std;
#define MAC_ADDRESS_LENGTH 6
typedef unsigned char MAC_address[MAC_ADDRESS_LENGTH];
typedef struct EthernetFrame
{
	unsigned char preamble[7]; //前同步码
	unsigned char SFD;
	MAC_address destination_address;
	MAC_address source_address;
	unsigned char type[2];
	unsigned char data[1500];
	unsigned char CRC[4];
}EthernetFrame;

MAC_address this_mac_address = { 0x00,0x50,0x56,0xc0,0x00,0x01 };

int max_address_match(const struct EthernetFrame* frame);

const MAC_address broadcast_address = { 0xff,0xff,0xff,0xff,0xff,0xff };//广播MAC地址

int max_address_match(const struct EthernetFrame* frame)
{
	if (frame == nullptr)
		return 0;

	if (memcmp(frame->destination_address, broadcast_address, MAC_ADDRESS_LENGTH) == 0)
	{
		return 1; 
	}
	if (memcmp(frame->destination_address, this_mac_address, MAC_ADDRESS_LENGTH) == 0)
	{
		return 1;
	}
	if (frame->destination_address[0] & 0x01)
	{
		return 1;
	}
	return 0;
}

int main()
{
	EthernetFrame test_frame;
	// 测试广播地址
	memcpy(test_frame.destination_address, broadcast_address, MAC_ADDRESS_LENGTH);
	cout << "Test address: ";
	for (int i = 0; i < MAC_ADDRESS_LENGTH; i++)
	{
		cout << setw(2) << setfill('0') << uppercase << hex << static_cast<int>(test_frame.destination_address[i]) << " ";
		if (i != MAC_ADDRESS_LENGTH - 1)
			cout << ":";
	}
	cout << endl << "Result: " << max_address_match(&test_frame) << endl;
	// 测试本机单播地址
	memcpy(test_frame.destination_address, this_mac_address, MAC_ADDRESS_LENGTH);
	cout << "Test address: ";
	for (int i = 0; i < MAC_ADDRESS_LENGTH; i++)
	{
		cout << setw(2) << setfill('0') << uppercase << hex << static_cast<int>(test_frame.destination_address[i]) << " ";
		if (i != MAC_ADDRESS_LENGTH - 1)
			cout << ":";
	}
	cout << endl << "Result: " << max_address_match(&test_frame) << endl;
	// 测试多播地址
	MAC_address multicast_address = { 0x01,0x00,0x5E,0x12,0x34,0x56 };// 第一位为0x01即可
	memcpy(test_frame.destination_address, multicast_address, MAC_ADDRESS_LENGTH);
	cout << "Test address: ";
	for (int i = 0; i < MAC_ADDRESS_LENGTH; i++)
	{
		cout << setw(2) << setfill('0') << uppercase << hex << static_cast<int>(test_frame.destination_address[i]) << " ";
		if (i != MAC_ADDRESS_LENGTH - 1)
			cout << ":";
	}
	cout << endl << "Result: " << max_address_match(&test_frame) << endl;
	// 测试其他地址
	MAC_address other_address = { 0x00,0x00,0x5E,0x12,0x34,0x56 };
	memcpy(test_frame.destination_address, other_address, MAC_ADDRESS_LENGTH);
	cout << "Test address: ";
	for (int i = 0; i < MAC_ADDRESS_LENGTH; i++)
	{
		cout << setw(2) << setfill('0') << uppercase << hex << static_cast<int>(test_frame.destination_address[i]) << " ";
		if (i != MAC_ADDRESS_LENGTH - 1)
			cout << ":";
	}
	cout << endl << "Result: " << max_address_match(&test_frame) << endl;

	return 0;
}