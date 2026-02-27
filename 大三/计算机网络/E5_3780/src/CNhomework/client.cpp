#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<cstring>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>
#include<atomic>
#include<chrono>
#pragma comment(lib,"ws2_32.lib")

const int PORT_SERVER = 8888;
const int PORT_LICENSE = 6666;
const char* SERVER_IP = "127.0.0.1";
const int TIMEOUT = 30 * 60;
bool isStop = false;

void sendStatusReport(SOCKET client_socket)
{
	while (!isStop)
	{
		std::string report = "status report";
		if (send(client_socket, report.c_str(), report.size() + 1, 0) < 0)
		{
			std::cerr << "状态发送失败" << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::seconds(TIMEOUT));
	}
}
std::string get_license(const char account[], const char password[], const char type[])
{
	WSADATA wsaData;
	SOCKET clientSocket;
	struct sockaddr_in serverAddress;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "初始化失败" << std::endl;
		exit(-1);
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << "创建套接字失败" << std::endl;
		exit(-1);
	}
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT_LICENSE);
	if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0)
	{
		std::cerr << "无效的服务器地址" << std::endl;
		exit(-1);
	}
	if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		std::cerr << "连接服务器失败" << std::endl;
		exit(-1);
	}
	if (send(clientSocket, account, strlen(account) + 1, 0) < 0)
	{
		std::cerr << "发送消息失败" << std::endl;
		exit(-1);
	}
	Sleep(1);
	if (send(clientSocket, password, strlen(account) + 1, 0) < 0)
	{
		std::cerr << "发送消息失败" << std::endl;
		exit(-1);
	}
	Sleep(1); 
	if (send(clientSocket, type, strlen(account) + 1, 0) < 0)
	{
		std::cerr << "发送消息失败" << std::endl;
		exit(-1);
	}
	char buffer[1024] = { 0 };
	if (recv(clientSocket, buffer, 1024, 0) < 0)
	{
		std::cerr << "接受响应失败" << std::endl;
		exit(-1);
	}
	closesocket(clientSocket);
	WSACleanup();
	return (std::string)buffer;
}

void sofewareA()
{
	std::cout << "使用软件A中，按Enter键结束使用" << std::endl;
	getchar();
	getchar();
}
int main()
{
	std::cout << "1.管理员生成许可证" << std::endl;
	std::cout << "2.用户使用软件" << std::endl;
	int choice;
	std::cin >> choice;
	if (choice == 1)
	{
		std::string account, password, type;
		std::cout << "账号： ";
		std::cin >> account;
		std::cout << "密码： ";
		std::cin >> password;
		std::cout << "类型： ";
		std::cin >> type;
		std::string serial = get_license(account.c_str(), password.c_str(), type.c_str());
		std::cout << "结果： " << serial << std::endl;
	}
	WSADATA wsaData;
	SOCKET clientSocket;
	struct sockaddr_in serverAddress;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "初始化失败" << std::endl;
		return -1;
	}
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << "创建套接字失败" << std::endl;
		return -1;
	}
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT_SERVER);
	if (inet_pton(AF_INET, SERVER_IP, &(serverAddress.sin_addr)) <= 0)
	{
		std::cerr << "无效的服务器地址" << std::endl;
		return -1;
	}
	std::string serial;
	std::cout << "输入10位许可证： ";
	while (std::cin >> serial && serial.size() != 10)
	{
		std::cout << "许可证必须为10位";
	}

	if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		std::cerr << "连接服务器失败" << std::endl;
		return -1;
	}
	if (send(clientSocket, serial.c_str(), serial.size() + 1, 0) < 0)
	{
		std::cerr << "发送消息失败" << std::endl;
		return -1;
	}
	char buffer[1024] = { 0 };
	if (recv(clientSocket, buffer, 1024, 0) < 0)
	{
		std::cerr << "接受响应失败" << std::endl;
		return -1;
	}
	std::string s = buffer;
	if (s == "ACCEPT")
	{
		std::cout << "允许使用" << std::endl;
		isStop = false;
		std::thread t(sendStatusReport, clientSocket);
		sofewareA();
		isStop = true;
		t.detach();
 	}
	else if (s == "REJECT")
	{
		std::cout << "当前许可证已满" << std::endl;
		Sleep(10);
	}
	closesocket(clientSocket);
	WSACleanup();
	std::cout << "软件已退出，许可证已释放" << std::endl;
	return 0;
}