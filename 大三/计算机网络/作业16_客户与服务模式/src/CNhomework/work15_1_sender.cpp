#include<iostream>
#include<cstdio>
#include<cstring>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<Windows.h>
#pragma comment(lib,"ws2_32.lib")

#define MULTICAST_GROUP "224.0.0.100"
#define PORT 8888
#define BUFFER_SIZE 4096
#define END_MARK "EOF"
 //后使用，传入需要发送的文件路径
int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "请输入文件路径" << std::endl;
		return 1;
	}
	const char* file_path = argv[1];
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed" << std::endl;
		return 1;
	}
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Socket creation failed" << std::endl;
		WSACleanup();
		return 1;
	}
	u_char ttl = 16;
	if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl)) == SOCKET_ERROR)
	{
		std::cerr << "setsocketopt IP_MULTICAST_TTL failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	sockaddr_in multicast_addr;
	memset(&multicast_addr, 0, sizeof(multicast_addr));
	multicast_addr.sin_family = AF_INET;
	multicast_addr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, MULTICAST_GROUP, &multicast_addr.sin_addr) <= 0)
	{
		std::cerr << "inet_pton failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	HANDLE hFile = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cerr << "File open failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	char buffer[BUFFER_SIZE];
	DWORD bytes_read;
	while (ReadFile(hFile, buffer, BUFFER_SIZE, &bytes_read, NULL) && bytes_read > 0)
	{
		int sent = sendto(sock, buffer, bytes_read, 0, (sockaddr*)&multicast_addr, sizeof(multicast_addr));
		if (sent == SOCKET_ERROR)
		{
			std::cerr << "sento failed" << std::endl;
			break;
		}
		Sleep(1);
	}
	sendto(sock, END_MARK, strlen(END_MARK), 0, (sockaddr*)&multicast_addr, sizeof(multicast_addr));
	CloseHandle(hFile);
	closesocket(sock);
	WSACleanup();
	return 0;
}