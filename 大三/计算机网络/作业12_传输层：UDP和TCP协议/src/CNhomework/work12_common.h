#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<Windows.h>
#include<iostream>
#include<chrono>
#include<ctime>
#pragma comment(lib,"ws2_32.lib")

#define ECHO_REQUEST 8
#define ECHO_REPLY 0
#define TIME_REQUEST 13
#define TIME_REPLY 14

struct ICMPHeader
{
	BYTE type;
	BYTE code;
	USHORT checksum;
	USHORT id;
	USHORT sequence;
};
struct ICMPTimestamp
{
	ICMPHeader header;
	ULONG originate;
	ULONG receive;
	ULONG transmit;
};

USHORT CalculateChecksum(const void* data, int length)
{
	ULONG sum = 0;
	const USHORT* ptr = (const USHORT*)data;
	while (length > 1)
	{
		sum += *ptr++;
		length -= 2;
	}
	if (length == 1)
	{
		sum += *(const BYTE*)ptr;
	}
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return (USHORT)(~sum);
}
ULONG GetCurrentTimestamp()
{
	auto now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	return (ULONG)(millis & 0xFFFFFFF);
}
std::string TimestampToString(ULONG timestamp)
{
	auto timePoint = std::chrono::system_clock::time_point(std::chrono::milliseconds(timestamp));
	std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
	char buffer[80];
	ctime_s(buffer, sizeof(buffer), &time);
	buffer[strlen(buffer) - 1] = '\0';
	return std::string(buffer);
}