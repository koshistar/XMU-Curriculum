#include"work12_common.h"
class Client
{
private:
	SOCKET sock;
	sockaddr_in serverAddr;
	USHORT sequence;
	USHORT id;
public:
	Client() :sock(INVALID_SOCKET), sequence(0), id(GetCurrentProcessId() & 0xFFFF) {}
	~Client()
	{
		if (sock != INVALID_SOCKET)
			closesocket(sock);
		WSACleanup();
	}
	bool Initialize(const char* serverHost)
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			std::cerr << "Failed to create raw socket. Run as Administrator." << std::endl;
			return false;
		}
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		if (inet_addr(serverHost) == INADDR_NONE)
		{
			hostent* host = gethostbyname(serverHost);
			if (host == nullptr)
			{
				std::cerr << "Could not resolve hostname: " << serverHost << std::endl;
				return false;
			}
			serverAddr.sin_addr = *((in_addr*)host->h_addr);
		}
		else
		{
			serverAddr.sin_addr.s_addr = inet_addr(serverHost);
		}
		std::cout << "Server: " << inet_ntoa(serverAddr.sin_addr) << std::endl;
		return true;
	}
	bool RequestTimestamp()
	{
		ICMPTimestamp request;
		memset(&request, 0, sizeof(request));

		request.header.type = TIME_REQUEST;
		request.header.code = 0;
		request.header.checksum = 0;
		request.header.id = htons(id);
		request.header.sequence = htons(sequence);
		ULONG originateTime = GetCurrentTimestamp();
		request.originate = originateTime;
		request.receive = 0;
		request.transmit = 0;
		request.header.checksum = CalculateChecksum(&request, sizeof(request));

		auto startTime = std::chrono::steady_clock::now();
		int sendResult = sendto(sock, (char*)&request, sizeof(request), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
		if (sendResult == SOCKET_ERROR) {
			std::cerr << "sendto failed: " << WSAGetLastError() << std::endl;
			return false;
		}
		std::cout << "Originate time: " << TimestampToString(originateTime) << std::endl;

		char recvBuff[1024];
		sockaddr_in fromAddr;
		int fromLen = sizeof(fromAddr);
		
		DWORD timeout = 3000;
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
		int recvLen = recvfrom(sock, recvBuff, sizeof(recvBuff), 0, (sockaddr*)&fromAddr, &fromLen);
		auto endTime = std::chrono::steady_clock::now();
		auto rtt = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
		if (recvLen == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT)
			{
				std::cout << "Request timed out." << std::endl;
			}
			else
			{
				std::cerr << "recvfrom failed: " << WSAGetLastError() << std::endl;
			}
			return false;
		}

		char* ipHeader = recvBuff;
		int ipHeaderLen = (ipHeader[0] & 0x0F) * 4;
		if (recvLen < ipHeaderLen + sizeof(ICMPTimestamp))
		{
			std::cerr << "Received packet too small." << std::endl;
			return false;
		}
		ICMPTimestamp* reply = (ICMPTimestamp*)(recvBuff + ipHeaderLen);
		if (reply->header.type == TIME_REPLY && ntohs(reply->header.id) == id && ntohs(reply->header.sequence) == sequence)
		{
			std::cout << "Server: " << inet_ntoa(fromAddr.sin_addr) << std::endl;
			std::cout << "Round-trip time: " << rtt.count() << " ms" << std::endl;
			std::cout << "Originate time: " << TimestampToString(reply->originate) << std::endl;
			std::cout << "Receive time:   " << TimestampToString(reply->receive) << std::endl;
			std::cout << "Transmit time:  " << TimestampToString(reply->transmit) << std::endl;

			ULONG clientCurrentTime = GetCurrentTimestamp();
			ULONG estimatedServerTime = reply->transmit + (rtt.count() / 2);
			long timeDiff = (long)estimatedServerTime - (long)clientCurrentTime;
			std::cout << "Estimated time difference: " << timeDiff << " ms" << std::endl;
			std::cout << "Client current time: " << TimestampToString(clientCurrentTime) << std::endl;
			std::cout << "Server estimated time: " << TimestampToString(estimatedServerTime) << std::endl;
			sequence++;
			return true;
		}
		else
		{
			std::cerr << "Invalid timestamp reply" << std::endl;
			return false;
		}
	}
	void Run(int count = 1)
	{
		for (int i = 0; i < count; i++)
		{
			std::cout << "Request " << (i + 1) << std::endl;
			if (!RequestTimestamp())
			{
				std::cout << "Failed to get timestamp from server" << std::endl;
			}
			if (i < count - 1)
			{
				Sleep(2000);
			}
		}
	}
};
int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <server_host>" << std::endl;
		std::cout << "Example: " << argv[0] << " 192.168.1.100" << std::endl;
		return 1;
	}

	Client client;
	if (client.Initialize(argv[1]))
	{
		client.Run(3);
	}
	std::cin.get();
	return 0;
}