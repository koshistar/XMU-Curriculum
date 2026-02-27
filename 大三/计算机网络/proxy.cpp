#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <atomic>

#define SOCKS5_VER 0x05
#define SOCKS4_VER 0x04
#define CMD_CONNECT 0x01
#define SOCKS5_METHOD_NO_AUTH 0x00
#define SOCKS5_REP_SUCCESS 0x00
#define SOCKS5_REP_GENERAL_FAILURE 0x01
#define SOCKS4_REP_SUCCESS 0x5a
#define SOCKS4_REP_FAILED 0x5b
#define ATYP_IPV4 0x01
#define ATYP_DOMAIN 0x03
#define ATYP_IPV6 0x04
int send_all(SOCKET s, const char* buf, int len)
{
    int sent = 0;
    while (sent < len) {
        int n = send(s, buf + sent, len - sent, 0);
        if (n == SOCKET_ERROR) return SOCKET_ERROR;
        sent += n;
    }
    return sent;
}
int recv_exact(SOCKET s, char* buf, int len, int flags = 0)
{
    int recvd = 0;
    while (recvd < len)
    {
        int n = recv(s, buf + recvd, len - recvd, flags);
        if (n <= 0) return n;
        recvd += n;
    }
    return recvd;
}
void relay(SOCKET src, SOCKET dst)
{
    char buffer[4096];
    int n;
    while ((n = recv(src, buffer, sizeof(buffer), 0)) > 0)
    {
        if (send_all(dst, buffer, n) == SOCKET_ERROR) break;
    }
    shutdown(dst, SD_SEND);
    closesocket(src);
}
int handle_socks5(SOCKET client_sock)
{
    unsigned char header[2];
    int n = recv_exact(client_sock, (char*)header, 2);
    if (n != 2 || header[0] != SOCKS5_VER) return -1;
    int nmethods = header[1];
    if (nmethods <= 0) 
        return -1;
    unsigned char methods[256];
    if (recv_exact(client_sock, (char*)methods, nmethods) != nmethods) 
        return -1;
    unsigned char auth_resp[2] = { SOCKS5_VER, SOCKS5_METHOD_NO_AUTH };
    if (send_all(client_sock, (char*)auth_resp, 2) == SOCKET_ERROR) 
        return -1;
    unsigned char req_h[4];
    if (recv_exact(client_sock, (char*)req_h, 4) != 4) 
        return -1;
    if (req_h[0] != SOCKS5_VER || req_h[1] != CMD_CONNECT) 
        return -1;
    unsigned char atyp = req_h[3];
    char target_host[INET6_ADDRSTRLEN] = {0};
    u_short target_port = 0;
    sockaddr_storage dest_addr = {0};
    int dest_addr_len = 0;
    if (atyp == ATYP_IPV4)
    {
        unsigned char addr_port[6];
        if (recv_exact(client_sock, (char*)addr_port, 6) != 6) return -1;
        sockaddr_in *sa = (sockaddr_in*)&dest_addr;
        sa->sin_family = AF_INET;
        memcpy(&sa->sin_addr.S_un.S_addr, addr_port, 4);
        memcpy(&sa->sin_port, addr_port + 4, 2);
        dest_addr_len = sizeof(sockaddr_in);
        inet_ntop(AF_INET, &sa->sin_addr, target_host, sizeof(target_host));
        target_port = ntohs(sa->sin_port);
    }
    else if (atyp == ATYP_DOMAIN)
    {
        unsigned char lenb;
        if (recv_exact(client_sock, (char*)&lenb, 1) != 1) 
            return -1;
        int dlen = lenb;
        if (dlen <= 0 || dlen > 255) 
            return -1;
        char domain[256] = {0};
        if (recv_exact(client_sock, domain, dlen) != dlen) 
            return -1;
        unsigned char portb[2];
        if (recv_exact(client_sock, (char*)portb, 2) != 2) 
            return -1;
        target_port = ntohs(*(u_short*)portb);
        addrinfo hints = {0}, *res = NULL;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        char portstr[6];
        sprintf_s(portstr, sizeof(portstr), "%u", target_port);
        if (getaddrinfo(domain, portstr, &hints, &res) != 0 || res == NULL)
        {
            return -1;
        }
        memcpy(&dest_addr, res->ai_addr, res->ai_addrlen);
        dest_addr_len = (int)res->ai_addrlen;
        inet_ntop(res->ai_family, &((sockaddr_in*)res->ai_addr)->sin_addr, target_host, sizeof(target_host));
        freeaddrinfo(res);
    }
    else
    {
        return -1;
    }
    SOCKET target_sock = socket(dest_addr.ss_family, SOCK_STREAM, IPPROTO_TCP);
    if (target_sock == INVALID_SOCKET) return -1;
    // int timeout = 10000; setsockopt(target_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    sockaddr *sa = (sockaddr*)&dest_addr;
    if (connect(target_sock, sa, dest_addr_len) == SOCKET_ERROR)
    {
        closesocket(target_sock);
        unsigned char rep[10] = { SOCKS5_VER, SOCKS5_REP_GENERAL_FAILURE, 0x00, ATYP_IPV4, 0,0,0,0, 0,0 };
        send_all(client_sock, (char*)rep, sizeof(rep));
        return -1;
    }
    unsigned char resp[10] = { SOCKS5_VER, SOCKS5_REP_SUCCESS, 0x00, ATYP_IPV4, 0,0,0,0, 0,0 };
    send_all(client_sock, (char*)resp, sizeof(resp));
    std::thread t1(relay, client_sock, target_sock);
    std::thread t2(relay, target_sock, client_sock);
    t1.detach();
    t2.detach();
    return 0;
}
int handle_socks4(SOCKET client_sock)
{
    unsigned char head[8];
    int n = recv_exact(client_sock, (char*)head, 8);
    if (n != 8 || head[0] != SOCKS4_VER || head[1] != CMD_CONNECT) 
        return -1;
    u_short target_port = ntohs(*(u_short*)(head + 2));
    unsigned long ip_raw = *(unsigned long*)(head + 4);
    char userid[256] = {0};
    int idx = 0;
    char ch = 0;
    do
    {
        if (recv_exact(client_sock, &ch, 1) != 1) return -1;
        if (idx < (int)sizeof(userid) - 1) userid[idx++] = ch;
    } while (ch != 0);
    sockaddr_in dest = {0};
    dest.sin_family = AF_INET;
    dest.sin_port = htons(target_port);
    if ((ip_raw & 0xFFFFFF00) == 0x00000000 && (ip_raw & 0xFF) != 0x00)
    {
        char domain[256] = {0};
        int di = 0;
        do
        {
            if (recv_exact(client_sock, &ch, 1) != 1) return -1;
            if (di < (int)sizeof(domain) - 1) domain[di++] = ch;
        } while (ch != 0);
        addrinfo hints = {0}, *res = NULL;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        char portstr[6];
        sprintf_s(portstr, sizeof(portstr), "%u", target_port);
        if (getaddrinfo(domain, portstr, &hints, &res) != 0 || res == NULL)
        {
            return -1;
        }
        memcpy(&dest, res->ai_addr, res->ai_addrlen);
        freeaddrinfo(res);
    }
    else
    {
        dest.sin_addr.S_un.S_addr = ip_raw;
    }
    SOCKET target_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (target_sock == INVALID_SOCKET) return -1;
    if (connect(target_sock, (sockaddr*)&dest, sizeof(dest)) == SOCKET_ERROR)
    {
        unsigned char resp[8] = {0x00, SOCKS4_REP_FAILED, 0x00,0x00, 0x00,0x00, 0x00,0x00};
        send_all(client_sock, (char*)resp, sizeof(resp));
        closesocket(target_sock);
        return -1;
    }
    unsigned char resp[8] = {0x00, SOCKS4_REP_SUCCESS, 0x00,0x00, 0x00,0x00, 0x00,0x00};
    send_all(client_sock, (char*)resp, sizeof(resp));
    std::thread t1(relay, client_sock, target_sock);
    std::thread t2(relay, target_sock, client_sock);
    t1.detach();
    t2.detach();
    return 0;
}
void handle_client_socket(SOCKET client_sock)
{
    unsigned char ver;
    int n = recv(client_sock, (char*)&ver, 1, MSG_PEEK);
    if (n != 1)
    {
        closesocket(client_sock);
        return;
    }
    if (ver == SOCKS5_VER)
    {
        if (handle_socks5(client_sock) != 0)
        {
            closesocket(client_sock);
        }
    }
    else if (ver == SOCKS4_VER)
    {
        if (handle_socks4(client_sock) != 0)
        {
            closesocket(client_sock);
        }
    }
    else
    {
        // 未知协议
        closesocket(client_sock);
    }
}

int main()
{
    const int listen_port = 1800;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSA 初始化失败！\n");
        return 1;
    }

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock == INVALID_SOCKET) {
        printf("创建套接字失败: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("192.168.10.1");
    server_addr.sin_port = htons(listen_port);
    if (bind(listen_sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        printf("绑定失败: %d\n", WSAGetLastError());
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }
    if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("监听失败: %d\n", WSAGetLastError());
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }
    printf("SOCKS4/SOCKS5 代理服务器已启动，监听端口: %d\n", listen_port);
    while (true)
    {
        sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
        SOCKET client_sock = accept(listen_sock, (sockaddr*)&client_addr, &client_addr_len);
        if (client_sock == INVALID_SOCKET)
        {
            printf("接受连接失败: %d\n", WSAGetLastError());
            continue;
        }
        char client_ip[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("客户端连接: %s:%d\n", client_ip, ntohs(client_addr.sin_port));
        std::thread t(handle_client_socket, client_sock);
        t.detach();
    }
    closesocket(listen_sock);
    WSACleanup();
    return 0;
}