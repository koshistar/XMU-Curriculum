#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <chrono>

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

class PingClient {
private:
    SOCKET sock;
    sockaddr_in destAddr;
    USHORT sequence;
    USHORT id;
public:
    PingClient() : sock(INVALID_SOCKET), sequence(0), id(GetCurrentProcessId() & 0xFFFF) {}
    ~PingClient() {
        if (sock != INVALID_SOCKET) 
        {
            closesocket(sock);
        }
        WSACleanup();
    }
    bool initialize(const char* hostname) {
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
        // 设置目标地址
        memset(&destAddr, 0, sizeof(destAddr));
        destAddr.sin_family = AF_INET;
        // 解析主机名
        if (inet_addr(hostname) == INADDR_NONE) 
        {
            hostent* host = gethostbyname(hostname);
            if (host == nullptr) 
            {
                std::cerr << "Could not resolve hostname: " << hostname << std::endl;
                return false;
            }
            destAddr.sin_addr = *((in_addr*)host->h_addr);
        }
        else 
        {
            destAddr.sin_addr.s_addr = inet_addr(hostname);
        }
        std::cout << "Pinging " << inet_ntoa(destAddr.sin_addr) << " with 32 bytes of data:" << std::endl;
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
    bool sendPing() {
        char sendBuf[sizeof(ICMPHeader) + 32];
        ICMPHeader* icmp = (ICMPHeader*)sendBuf;
        // 填充ICMP头
        icmp->type = 8;
        icmp->code = 0;
        icmp->checksum = 0;
        icmp->id = htons(id);
        icmp->sequence = htons(sequence);
        icmp->timestamp = GetTickCount();

        memset(sendBuf + sizeof(ICMPHeader), 'A', 32);
        icmp->checksum = calculateChecksum(sendBuf, sizeof(sendBuf));
        // 发送包
        auto start = std::chrono::steady_clock::now();
        int result = sendto(sock, sendBuf, sizeof(sendBuf), 0,
            (sockaddr*)&destAddr, sizeof(destAddr));

        if (result == SOCKET_ERROR) 
        {
            std::cerr << "sendto failed: " << WSAGetLastError() << std::endl;
            return false;
        }
        // 接收回复
        char recvBuf[1024];
        sockaddr_in fromAddr;
        int fromLen = sizeof(fromAddr);
        // 设置超时
        DWORD timeout = 1000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(DWORD));
        result = recvfrom(sock, recvBuf, sizeof(recvBuf), 0,
            (sockaddr*)&fromAddr, &fromLen);

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        if (result == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAETIMEDOUT) {
                std::cout << "Request timed out." << std::endl;
            }
            else {
                std::cerr << "recvfrom failed: " << WSAGetLastError() << std::endl;
            }
            return false;
        }
        // 解析IP头
        IPHeader* ipHeader = (IPHeader*)recvBuf;
        int ipHeaderLen = (ipHeader->hlen & 0x0F) * 4;
        // 解析ICMP头
        ICMPHeader* recvIcmp = (ICMPHeader*)(recvBuf + ipHeaderLen);

        // 验证回复
        if (recvIcmp->type == 0 && ntohs(recvIcmp->id) == id && ntohs(recvIcmp->sequence) == sequence) {
            std::cout << "Reply from " << inet_ntoa(fromAddr.sin_addr) << ": bytes=32 time=" << duration.count() << "ms TTL=" << (int)ipHeader->ttl << std::endl;
            return true;
        }
        return false;
    }
    void run(int count = 4) {
        for (int i = 0; i < count; i++) 
        {
            if (sendPing()) 
            {
                sequence++;
            }
            Sleep(1000); // 等待1秒
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <hostname/IP>" << std::endl;
        return 1;
    }
    PingClient client;
    if (client.initialize(argv[1])) {
        client.run();
    }
    return 0;
}