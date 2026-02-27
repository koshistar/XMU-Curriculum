#include<iostream>
#include<cstdio>
#include<cstring>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<Windows.h>
#include<sys/stat.h>
#pragma comment(lib,"ws2_32.lib")
#define TCP_PORT 8888
#define UDP_PORT 6666
#define BUFFER_SIZE 8192
#define END_MARK "TRANSFER_END"
long long get_file_size(const char* path)
{
	struct _stat file_stat;
	if (_stat(path, &file_stat) == 0)
	{
		return file_stat.st_size;
	}
	return -1;
}
double format_speed(long long bytes, double seconds)
{
	return (bytes / (1024.0 * 1024.0)) / seconds;
}
void tcp_receiver(const char* save_path)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStart failed" << std::endl;
		return;
	}
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Socket creation failed" << std::endl;
		WSACleanup();
		return;
	}
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(TCP_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		std::cerr << "Bind failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	if (listen(sock, 5) == SOCKET_ERROR)
	{
		std::cerr << "Listen failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	SOCKET connect_sock = accept(sock, (sockaddr*)&client_addr, &client_addr_len);
	if (connect_sock == INVALID_SOCKET)
	{
		std::cerr << "Accept failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	HANDLE hFile = CreateFileA(save_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cerr << "File creation failed" << std::endl;
		closesocket(connect_sock);
		closesocket(sock);
		WSACleanup();
		return;
	}
	char buffer[BUFFER_SIZE];
	DWORD byts_written;
	long long total_recv = 0;
	DWORD start_time = GetTickCount();
	while (true)
	{
		int recv_len = recv(connect_sock, buffer, BUFFER_SIZE, 0);
		if (recv_len <= 0)
			break;
		total_recv += recv_len;
		WriteFile(hFile, buffer, recv_len, &byts_written, NULL);
	}
	DWORD end_time = GetTickCount();
	double duration = (end_time - start_time) / 1000.0;
	double speed = format_speed(total_recv, duration);
	std::cout << "TCP 传输完成\n总接受字节： " << total_recv << "B\n传输时间： " << duration << "s\n有效速率： " << speed << "MB/s\n";
	CloseHandle(hFile);
	closesocket(connect_sock);
	closesocket(sock);
	WSACleanup();
}
void tcp_sender(const char* server_ip,const char* file_path)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed" << std::endl;
		return;
	}
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Socket creation failed" << std::endl;
		WSACleanup();
		return;
	}
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(TCP_PORT);
	if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
	{
		std::cerr << "IP Adress invalid" << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		std::cerr << "Connection failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	HANDLE hFile = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cerr << "open file failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	char buffer[BUFFER_SIZE];
	DWORD bytes_read;
	long long file_size = get_file_size(file_path);
	DWORD start_time = GetTickCount();
	while (ReadFile(hFile, buffer, BUFFER_SIZE, &bytes_read, NULL) && bytes_read > 0)
	{
		send(sock, buffer, bytes_read, 0);
	}
	DWORD end_time = GetTickCount();
	double duration = (end_time - start_time) / 1000.0;
	double speed = format_speed(file_size, duration);
	std::cout << "TCP发送完成\n文件大小： " << file_size << "B\n传输时间： " << duration << "s\n有效速率： " << speed << "MB/s\n";
	CloseHandle(hFile);
	closesocket(sock);
	WSACleanup();
}
void udp_receiver(const char* save_path)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStart failed" << std::endl;
		return;
	}
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Socket creation failed" << std::endl;
		WSACleanup();
		return;
	}
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(UDP_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		std::cerr << "Bind failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	//不需要监听和接收
	HANDLE hFile = CreateFileA(save_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cerr << "File creation failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	char buffer[BUFFER_SIZE];
	sockaddr_in sender_addr;
	int sender_len = sizeof(sender_addr);
	DWORD byts_written;
	long long total_recv = 0;
	DWORD start_time = GetTickCount();
	while (true)
	{
		int recv_len = recvfrom(sock, buffer, BUFFER_SIZE, 0, (sockaddr*)&sender_addr, &sender_len);
		if (recv_len <= 0)
			break;
		if (strncmp(buffer, END_MARK, strlen(END_MARK)) == 0)
			break;
		total_recv += recv_len;
		WriteFile(hFile, buffer, recv_len, &byts_written, NULL);
	}
	DWORD end_time = GetTickCount();
	double duration = (end_time - start_time) / 1000.0;
	double speed = format_speed(total_recv, duration);
	std::cout << "UDP 传输完成\n总接受字节： " << total_recv << "B\n传输时间： " << duration << "s\n有效速率： " << speed << "MB/s\n";
	CloseHandle(hFile);
	closesocket(sock);
	WSACleanup();
}
void udp_sender(const char* server_ip, const char* file_path)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSAStartup failed" << std::endl;
		return;
	}
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Socket creation failed" << std::endl;
		WSACleanup();
		return;
	}
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(UDP_PORT);
	if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0)
	{
		std::cerr << "IP Adress invalid" << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	HANDLE hFile = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cerr << "open file failed" << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	char buffer[BUFFER_SIZE];
	DWORD bytes_read;
	long long file_size = get_file_size(file_path);
	DWORD start_time = GetTickCount();
	while (ReadFile(hFile, buffer, BUFFER_SIZE, &bytes_read, NULL) && bytes_read > 0)
	{
		sendto(sock, buffer, bytes_read, 0, (sockaddr*)&server_addr, sizeof(server_addr));
		//Sleep(1);//防止丢包
	}
	sendto(sock, END_MARK, strlen(END_MARK), 0, (sockaddr*)&server_addr, sizeof(server_addr));
	DWORD end_time = GetTickCount();
	double duration = (end_time - start_time) / 1000.0;
	double speed = format_speed(file_size, duration);
	std::cout << "TCP发送完成\n文件大小： " << file_size << "B\n传输时间： " << duration << "s\n有效速率： " << speed << "MB/s\n";
	CloseHandle(hFile);
	closesocket(sock);
	WSACleanup();
}
int main(int argc,char *argv[])
{
	if (argc < 3)
	{
		std::cout << "1 <文件路径>.启动TCP服务器\n2 <服务端IP> <文件路径>.启动TCP客户端\n3 <文件路径>.启动UDP接收方\n4 <目标IP> <文件路径>.启动UDP发送方\n";
		return 1;
	}
	std::string mode = argv[1];
	if (mode == "1")
	{
		tcp_receiver(argv[2]);
	}
	else if (mode == "2")
	{
		if (argc < 4)
		{
			std::cout << "2 <服务端IP> <文件路径>.启动TCP客户端\n";
			return 1;
		}
		tcp_sender(argv[2], argv[3]);
	}
	else if (mode == "3")
	{
		udp_receiver(argv[2]);
	}
	else if (mode == "4")
	{
		if (argc < 4)
		{
			std::cout << "4 <目标IP> <文件路径>.启动UDP接收方\n";
			return 1;
		}
		udp_sender(argv[2], argv[3]);
	}
	else
	{
		std::cout << "mode error" << std::endl;
		return 1;
	}
	return 0;
}
