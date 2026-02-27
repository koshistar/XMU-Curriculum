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
 //先使用，传入保存文件的路径，然后挂起，等待传入
int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "请输入文件路径" << std::endl;
		return 1;
	}
	const char* save_path = argv[1];
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
	bool reuse = true;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) == SOCKET_ERROR)
	{
		std::cerr << "setsocket SO_REUSERADDR failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	sockaddr_in local_addr;
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(PORT);
	local_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR)
	{
		std::cerr << "bild failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	ip_mreq multicast_req;
	inet_pton(AF_INET, MULTICAST_GROUP, &multicast_req.imr_multiaddr);
	multicast_req.imr_interface.s_addr = INADDR_ANY;
	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&multicast_req, sizeof(multicast_req)) == SOCKET_ERROR)
	{
		std::cerr << "setsockopt IP_ADD_MEMBERSHIP failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	HANDLE hFile = CreateFileA(save_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cerr << "File create failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	char buffer[BUFFER_SIZE];
	sockaddr_in sender_addr;
	int sender_len = sizeof(sender_addr);
	DWORD bytes_written;
	while (true)
	{
		int bytes_recv = recvfrom(sock, buffer, BUFFER_SIZE, 0, (sockaddr*)&sender_addr, &sender_len);
		if (bytes_recv == SOCKET_ERROR)
		{
			std::cerr << "recvfrom failed" << std::endl;
			break;
		}
		if (strncmp(buffer, END_MARK, strlen(END_MARK)) == 0)
		{
			std::cout << "Receive end mark, stop" << std::endl;
			break;
		}
		WriteFile(hFile, buffer, bytes_recv, &bytes_written, NULL);
	}
	CloseHandle(hFile);
	closesocket(sock);
	WSACleanup();
	std::cout << "File save to " << save_path << std::endl;
	return 0;
}