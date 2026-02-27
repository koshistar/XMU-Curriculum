#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>
#include<mutex>
#include<map>
#include<string>
#include<fstream>
#include<chrono>
#include<atomic>
#include<random>
#include<ctime>
#pragma comment(lib,"ws2_32.lib")

const int PORT_SERVER = 8888;
const int PORT_LICENSE = 6666;
const int max_license = 5;
std::map<std::string, int> license_map;
std::string server_start_time;

std::string time_now()
{
	time_t now = time(nullptr);
	std::tm* localTime = std::localtime(&now);
	char timeStr[80];
	strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);
	return std::string(timeStr);
}

//生成10位许可证序列号
std::string generateSerial()
{
	std::string serial;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 9);
	for (int i = 0; i < 10; i++)
	{
		serial += std::to_string(dis(gen));
	}
	return serial;
}
//手动构造JSON
void saveLicenseData()
{
	std::ofstream file("license_data.json");
	if (!file.is_open())
	{
		std::cerr << time_now() << " 无法写入" << std::endl;
		return;
	}
	file << "{\n\t\"max_license\": " << max_license << ",\n\t\"server_start_time\": \"" << server_start_time << "\",\n\t\"updated_time\": \"" << time_now() << "\",\n\t\"licenses\": [\n";
	bool first = true;
	for (auto& pair : license_map)
	{
		if (!first)
		{
			file << ",\n";
		}
		first = false;
		file << "\t\t{\n\t\t\t\"serial\": \"" << pair.first << "\",\n\t\t\t\"user_count\": " << pair.second << "\n\t\t}";
	}
	file << "\n\t]\n}\n";
	file.close();
}

void clientThread(SOCKET client_socket)
{
	char buffer[1024] = { 0 };
	sockaddr_in client_addr;
	int addr_len = sizeof(client_addr);
	getpeername(client_socket, (sockaddr*)&client_addr, &addr_len);
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

	int bytesRead = recv(client_socket, buffer, 1024, 0);
	if (bytesRead < 0)
	{
		std::cerr << time_now() << "接受来自" << client_ip << "的消息失败" << std::endl;
		return;
	}
	std::string command(buffer);
	if (license_map.find(command) == license_map.end() || license_map[command] < max_license)
	{
		std::string res = "ACCEPT";
		send(client_socket, res.c_str(), res.length() + 1, 0);
		license_map[command]++;
		saveLicenseData();
		std::cout << time_now() << " 客户接入，" << "许可证： " << command << "，IP地址： " << client_ip << "，用户数量：" << license_map[command] << std::endl;
		while (true)
		{
			bytesRead = recv(client_socket, buffer, 1024, 0);
			if (bytesRead > 0)
			{
				std::cout << time_now() << " 接收到许可证为" << command << "，ip地址为" << client_ip << "的用户状态报送" << std::endl;
			}
			else
			{
				std::cout << time_now() << " 许可证为" << command << "，ip为" << client_ip << "的用户退出" << std::endl;
				license_map[command]--;
				saveLicenseData();
				return;
			}
		}
	}
	else
	{
		std::string res = "REJECT";
		std::cout << time_now() << " 许可证为" << command << "，ip地址为" << client_ip << "的客户接入失败" << std::endl;
		send(client_socket, res.c_str(), res.length() + 1, 0);
	}
	closesocket(client_socket);
	std::cout << time_now() << " 客户端" << client_ip << "断开连接" << std::endl;
}
void licenseListener(SOCKET licenseSocket)
{
	SOCKET clientSocket;
	struct sockaddr_in clientAddress;
	int clientAddressLength = sizeof(clientAddress);
	while (true)
	{
		clientSocket = accept(licenseSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cout << "接受许可证连接失败" << std::endl;
			continue;
		}
		char account[80], password[80], type[80];
		if (recv(clientSocket, account, 1024, 0) < 0)
		{
			std::cerr << "接受数据失败" << std::endl;
			closesocket(licenseSocket);
			continue;
		}
		if (recv(clientSocket, password, 1024, 0) < 0)
		{
			std::cerr << "接受数据失败" << std::endl;
			closesocket(licenseSocket);
			continue;
		}
		if (recv(clientSocket, type, 1024, 0) < 0)
		{
			std::cerr << "接受数据失败" << std::endl;
			closesocket(licenseSocket);
			continue;
		}
		std::string license = generateSerial();
		license_map[license] = 0;
		saveLicenseData();
		std::cout << time_now() << " 用户：" << account << "接入，并分配许可证" << license << std::endl;
		send(clientSocket, license.c_str(), license.size() + 1, 0);
		closesocket(clientSocket);
	}
}
void serverListener(SOCKET serverSocket)
{
	SOCKET clientSocket;
	struct sockaddr_in clientAddress;
	int clientAddressLength = sizeof(clientAddress);
	while (true)
	{
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cout << "接受用户连接失败" << std::endl;
			continue;
		}
		std::thread(clientThread, clientSocket).detach();
	}
}
int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "WSA初始化失败" << std::endl;
		return -1;
	}
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		std::cerr << "创建套接字失败" << std::endl;
		return -1;
	}
	SOCKET licenseSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (licenseSocket == INVALID_SOCKET)
	{
		std::cerr << "创建套接字失败" << std::endl;
		return -1;
	}
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(PORT_SERVER);
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		std::cerr << "绑定失败" << std::endl;
		return -1;
	}
	if (listen(serverSocket, 5) == SOCKET_ERROR)
	{
		std::cerr << "监听失败" << std::endl;
		return -1;
	}
	struct sockaddr_in licenseAddress;
	licenseAddress.sin_family = AF_INET;
	licenseAddress.sin_addr.s_addr = INADDR_ANY;
	licenseAddress.sin_port = htons(PORT_LICENSE);
	if (bind(licenseSocket, (struct sockaddr*)&licenseAddress, sizeof(licenseAddress)) == SOCKET_ERROR)
	{
		std::cerr << "绑定失败" << std::endl;
		return -1;
	}
	if (listen(licenseSocket, 5) == SOCKET_ERROR)
	{
		std::cerr << "监听失败" << std::endl;
		return -1;
	}
	std::cout << time_now() << " 许可证服务器启动（端口" << PORT_SERVER << "）" << std::endl;
	server_start_time = time_now();
	saveLicenseData();

	std::thread licenseThread(licenseListener, licenseSocket);
	std::thread serverThread(serverListener, serverSocket);
	licenseThread.join();
	serverThread.join();

	closesocket(licenseSocket);
	closesocket(serverSocket);
	WSACleanup();
	return 0;
}