#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <ctime>
#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#define NTP_PORT 123
#define NTP_PACKET_SIZE 48
#define NTP_TIMESTAMP_DELTA 2208988800ull

struct ntp_packet
{
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t root_delay;
    uint32_t root_dispersion;
    uint32_t ref_id;
    uint32_t ref_timestamp[2];
    uint32_t orig_timestamp[2];
    uint32_t recv_timestamp[2];
    uint32_t trans_timestamp[2];
};

void time_to_ntp(uint32_t* ntp_timestamp, time_t t)
{
    uint64_t ntp_time = (uint64_t)t + NTP_TIMESTAMP_DELTA;
    ntp_timestamp[0] = htonl((uint32_t)(ntp_time >> 32));
    ntp_timestamp[1] = htonl((uint32_t)(ntp_time & 0xFFFFFFFF));
}
bool parse_time_string(const std::string& time_str, struct tm& tm_time)
{
    if (sscanf(time_str.c_str(), "%d-%d-%d %d:%d:%d", &tm_time.tm_year, &tm_time.tm_mon, &tm_time.tm_mday, &tm_time.tm_hour, &tm_time.tm_min, &tm_time.tm_sec) != 6)
    {
        return false;
    }
    tm_time.tm_year -= 1900;
    tm_time.tm_mon -= 1;
    tm_time.tm_isdst = -1;
    return true;
}

int main(int argc, char* argv[]) {
    // 输入格式："YYYY-MM-DD HH:MM:SS"
    // 例如："2019-05-01 11:45:14"
    if (argc != 2)
    {
        return 1;
    }
    std::string time_string = argv[1];
    struct tm tm_time = { 0 };
    if (!parse_time_string(time_string, tm_time))
    {
        std::cerr << "错误: 时间格式不正确，请使用 \"YYYY-MM-DD HH:MM:SS\" 格式" << std::endl;
        return 1;
    }
    time_t fixed_time = mktime(&tm_time);
    if (fixed_time == -1)
    {
        std::cerr << "错误: 无效的时间" << std::endl;
        return 1;
    }
    std::cout << "设置NTP服务器时间为: " << time_string << std::endl;
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup失败" << std::endl;
        return 1;
    }
    SOCKET server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket == INVALID_SOCKET)
    {
        std::cerr << "创建socket失败: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    int optval = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval)) == SOCKET_ERROR)
    {
        std::cerr << "设置socket选项失败: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(NTP_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        std::cerr << "绑定端口失败: " << WSAGetLastError() << std::endl;
        std::cerr << "注意: 需要管理员权限绑定到端口123" << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "NTP服务器启动，监听端口 " << NTP_PORT << std::endl;
    while (true)
    {
        ntp_packet packet;
        sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
        int bytes_received = recvfrom(server_socket, (char*)&packet, sizeof(packet), 0, (sockaddr*)&client_addr, &client_addr_len);
        if (bytes_received == SOCKET_ERROR)
        {
            std::cerr << "接收数据失败: " << WSAGetLastError() << std::endl;
            continue;
        }
        if (bytes_received >= sizeof(packet.li_vn_mode))
        {
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            std::cout << "收到NTP请求来自: " << client_ip << std::endl;
            memset(&packet, 0, sizeof(packet));
            packet.li_vn_mode = 0x1C;
            packet.stratum = 1;
            packet.poll = 4;
            packet.precision = 0xFA;
            packet.ref_id = htonl(0x4C4F434C);
            time_t current_time = time(nullptr);

            time_to_ntp(packet.ref_timestamp, fixed_time);
            time_to_ntp(packet.recv_timestamp, current_time);
            time_to_ntp(packet.trans_timestamp, fixed_time);
            if (sendto(server_socket, (char*)&packet, sizeof(packet), 0, (sockaddr*)&client_addr, client_addr_len) == SOCKET_ERROR)
            {
                std::cerr << "发送响应失败: " << WSAGetLastError() << std::endl;
            }
            else
            {
                std::cout << "已发送NTP响应" << std::endl;
            }
        }
    }
    closesocket(server_socket);
    WSACleanup();
    return 0;
}