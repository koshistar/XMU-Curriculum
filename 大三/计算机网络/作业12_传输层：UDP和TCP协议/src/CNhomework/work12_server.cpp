#include"work12_common.h"
class Server
{
private:
	SOCKET sock;
	USHORT currentId;
public:
	Server() :sock(INVALID_SOCKET), currentId(GetCurrentProcessId() & 0xFFFF) {}
	~Server()
	{
		if (sock != INVALID_SOCKET)
			closesocket(sock);
		WSACleanup();
	}
	bool Initialize()
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			std::cerr << "WSAStartup failed" << std::endl;
			return false;
		}
		sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (sock == INVALID_SOCKET) {
			std::cerr << "Failed to create raw socket. Error: " << WSAGetLastError() << std::endl;
			std::cerr << "Please run as Administrator!" << std::endl;
			return false;
		}

		//ÉèÖÃ³¬Ê±
		DWORD timeout = 5000;
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
		std::cout << "Listening for ICMP timestamp requests..." << std::endl;
		return true;
	}
	void ProcessTimestampRequest(const char* recvBuf, int recvLen, sockaddr_in* fromAddr)
	{
		char* ipHeader = (char*)recvBuf;
		int ipHeaderLen = (ipHeader[0] & 0x0F) * 4;
		if (recvLen < ipHeaderLen + sizeof(ICMPTimestamp))
			return;
		ICMPTimestamp* request = (ICMPTimestamp*)(recvBuf + ipHeaderLen);
		if (request->header.type != TIME_REQUEST)
			return;
		std::cout << "Received timestamp request from " << inet_ntoa(fromAddr->sin_addr) << std::endl;

		ICMPTimestamp reply;
		memset(&reply, 0, sizeof(reply));
		reply.header.type = TIME_REPLY;
		reply.header.code = 0;
		reply.header.id = request->header.id;
		reply.header.sequence = request->header.sequence;
		reply.originate = request->originate;
		reply.receive = GetCurrentTimestamp();
		reply.transmit = reply.receive;
		reply.header.checksum = 0;
		reply.header.checksum = CalculateChecksum(&reply, sizeof(reply));
		int sendResult = sendto(sock, (char*)&reply, sizeof(reply), 0, (sockaddr*)fromAddr, sizeof(sockaddr_in));
		if (sendResult == SOCKET_ERROR) {
			std::cerr << "sendto failed: " << WSAGetLastError() << std::endl;
		}
		else {
			std::cout << "Sent timestamp reply to " << inet_ntoa(fromAddr->sin_addr) << std::endl;
			std::cout << "Originate: " << TimestampToString(reply.originate) << std::endl;
			std::cout << "Receive:   " << TimestampToString(reply.receive) << std::endl;
			std::cout << "Transmit:  " << TimestampToString(reply.transmit) << std::endl;
		}
	}
	void Run()
	{
		char recvBuf[1024];
		sockaddr_in fromAddr;
		int fromLen = sizeof(fromAddr);
		while (1)
		{
			int recvLen = recvfrom(sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&fromAddr, &fromLen);
			if (recvLen == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error != WSAETIMEDOUT)
					std::cerr << "recvfrom failed: " << error << std::endl;
				continue;
			}
			ProcessTimestampRequest(recvBuf, recvLen, &fromAddr);
		}
	}
};
int main()
{
	std::cout << "Server Starting..." << std::endl;
	Server server;
	if (server.Initialize())
	{
		server.Run();
	}
	else
	{
		std::cerr << "Fail to initialize server." << std::endl;
		std::cin.get();
	}
	return 0;
}