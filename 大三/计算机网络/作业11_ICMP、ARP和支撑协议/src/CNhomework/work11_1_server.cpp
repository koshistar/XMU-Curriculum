#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

struct ICMPHeader {
    BYTE type;
    BYTE code;
    USHORT checksum;
    USHORT id;
    USHORT sequence;
    ULONG timestamp;
};
struct IPHeader {
    BYTE hlen : 4;
    BYTE version : 4;
    BYTE tos;
    USHORT total_len;
    USHORT ident;
    USHORT flags;
    BYTE ttl;
    BYTE protocol;
    USHORT checksum;
    ULONG source_ip;
    ULONG dest_ip;
};

class PingServer {
private:
    SOCKET sock;
public:
    PingServer() : sock(INVALID_SOCKET) {}
    ~PingServer()
    {
        if (sock != INVALID_SOCKET) 
        {
            closesocket(sock);
        }
        WSACleanup();
    }
    bool initialize()
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
        {
            std::cerr << "WSAStartup failed" << std::endl;
            return false;
        }
        sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if (sock == INVALID_SOCKET) 
        {
            std::cerr << "Failed to create raw socket. Run as Administrator!" << std::endl;
            return false;
        }
        int timeout = 5000;
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
            std::cerr << "setsockopt failed: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            sock = INVALID_SOCKET;
            return false;
        }
        
        BOOL broadcast = TRUE;
        if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) == SOCKET_ERROR)
        {
            std::cerr << "SO_BROADCAST failed: " << WSAGetLastError() << std::endl;
        }
        std::cout << "Ping server started successfully. Listening for ICMP Echo Requests..." << std::endl;
        std::cout << "Local IP addresses:" << std::endl;
        std::cout << "Ping server started. Listening for ICMP Echo Requests..." << std::endl;
        
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == 0)
        {
            hostent* host = gethostbyname(hostname);
            if (host != nullptr)
            {
                for (int i = 0; host->h_addr_list[i] != nullptr; i++)
                {
                    std::cout << "  " << inet_ntoa(*(in_addr*)host->h_addr_list[i]) << std::endl;
                }
            }
        }
        return true;
    }

    USHORT calculateChecksum(const void* data, int length) {
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

    void processEchoRequest(const char* recvBuf, int recvLen, sockaddr_in* fromAddr) {
        IPHeader* ipHeader = (IPHeader*)recvBuf;
        int ipHeaderLen = (ipHeader->hlen & 0x0F) * 4;
        if (recvLen < ipHeaderLen + sizeof(ICMPHeader)) {
            return;
        }
        ICMPHeader* recvIcmp = (ICMPHeader*)(recvBuf + ipHeaderLen);

        if (recvIcmp->type != 8) {
            return;
        }

        std::cout << "Received Echo Request from " << inet_ntoa(fromAddr->sin_addr) << " ID=" << ntohs(recvIcmp->id) << " Seq=" << ntohs(recvIcmp->sequence) << std::endl;
        // 准备Echo Reply
        char sendBuf[1024];
        ICMPHeader* replyIcmp = (ICMPHeader*)sendBuf;
        // 复制原始数据
        memcpy(sendBuf, recvBuf + ipHeaderLen, recvLen - ipHeaderLen);
        // 修改为Echo Reply
        replyIcmp->type = 0;
        replyIcmp->code = 0;
        replyIcmp->checksum = 0;
        // 计算新校验和
        replyIcmp->checksum = calculateChecksum(sendBuf, recvLen - ipHeaderLen);
        // 发送回复
        sendto(sock, sendBuf, recvLen - ipHeaderLen, 0,
            (sockaddr*)fromAddr, sizeof(sockaddr_in));
        std::cout << "Sent Echo Reply to " << inet_ntoa(fromAddr->sin_addr) << std::endl;
    }

    void run()
    {
        char recvBuf[1024];
        sockaddr_in fromAddr;
        int fromLen = sizeof(fromAddr);
        while (true)
        {
            int recvLen = recvfrom(sock, recvBuf, sizeof(recvBuf), 0,
                (sockaddr*)&fromAddr, &fromLen);
            if (recvLen == SOCKET_ERROR)
            {
                if (WSAGetLastError() != WSAETIMEDOUT)
                {
                    std::cerr << "recvfrom failed: " << WSAGetLastError() << std::endl;
                    closesocket(sock);
                    WSACleanup();
                    Sleep(1000);
                    if (!initialize()) {
                        std::cerr << "Recovery failed. Exiting." << std::endl;
                        break;
                    }
                }
                continue;
            }
            processEchoRequest(recvBuf, recvLen, &fromAddr);
        }
    }
};

int main() {
    PingServer server;
    if (server.initialize())
    {
        server.run();
    }
    return 0;
}