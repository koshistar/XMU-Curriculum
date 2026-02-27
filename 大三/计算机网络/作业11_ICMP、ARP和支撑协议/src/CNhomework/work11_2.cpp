#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <map>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68
#define DHCP_MESSAGE_TYPE_DISCOVER 1
#define DHCP_MESSAGE_TYPE_OFFER 2
#define DHCP_MESSAGE_TYPE_REQUEST 3
#define DHCP_MESSAGE_TYPE_ACK 5

struct dhcp_message {
    uint8_t op;
    uint8_t htype;
    uint8_t hlen;
    uint8_t hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint32_t ciaddr;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t giaddr;
    uint8_t chaddr[16];
    uint8_t sname[64];
    uint8_t file[128];
    uint32_t magic_cookie;
    uint8_t options[308];
};
class DHCPServer {
private:
    SOCKET server_socket;
    sockaddr_in server_addr;
    std::string server_ip;
    std::string subnet_mask;
    std::string gateway;
    std::map<std::string, std::string> ip_allocation;
public:
    DHCPServer(const std::string& ip = "192.168.1.1", const std::string& mask = "255.255.255.0", const std::string& gw = "192.168.1.1") : server_ip(ip), subnet_mask(mask), gateway(gw)
    {
        // 预分配固定IP地址
        ip_allocation["00-11-22-33-44-55"] = "192.168.1.2";
        ip_allocation["AA-BB-CC-DD-EE-FF"] = "192.168.1.3";
    }
    bool initialize() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
            return false;
        }
        server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (server_socket == INVALID_SOCKET)
        {
            std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return false;
        }
        // 设置套接字选项，允许广播
        BOOL broadcast = TRUE;
        if (setsockopt(server_socket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) == SOCKET_ERROR)
        {
            std::cerr << "Setsockopt failed: " << WSAGetLastError() << std::endl;
            closesocket(server_socket);
            WSACleanup();
            return false;
        }
        // 绑定到DHCP服务器端口
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(DHCP_SERVER_PORT);
        server_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
        {
            std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
            closesocket(server_socket);
            WSACleanup();
            return false;
        }

        std::cout << "DHCP Server initialized successfully!" << std::endl;
        std::cout << "Server IP: " << server_ip << std::endl;
        std::cout << "Subnet Mask: " << subnet_mask << std::endl;
        std::cout << "Gateway: " << gateway << std::endl;
        std::cout << "Pre-allocated IPs:" << std::endl;
        for (const auto& allocation : ip_allocation) {
            std::cout << "  " << allocation.first << " -> " << allocation.second << std::endl;
        }
        return true;
    }
    std::string mac_to_string(const uint8_t* chaddr)
    {
        char mac[18];
        sprintf_s(mac, sizeof(mac), "%02X-%02X-%02X-%02X-%02X-%02X", chaddr[0], chaddr[1], chaddr[2], chaddr[3], chaddr[4], chaddr[5]);
        return std::string(mac);
    }
    uint32_t ip_to_int(const std::string& ip)
    {
        in_addr addr;
        inet_pton(AF_INET, ip.c_str(), &addr);
        return addr.s_addr;
    }
    std::string int_to_ip(uint32_t ip)
    {
        char ip_str[INET_ADDRSTRLEN];
        in_addr addr;
        addr.s_addr = ip;
        inet_ntop(AF_INET, &addr, ip_str, INET_ADDRSTRLEN);
        return std::string(ip_str);
    }
    std::string get_allocated_ip(const std::string& mac_address)
    {
        auto it = ip_allocation.find(mac_address);
        if (it != ip_allocation.end()) {
            return it->second;
        }
        return "";
    }
    void add_dhcp_option(uint8_t* options, int& offset, uint8_t code, const uint8_t* data, uint8_t length)
    {
        options[offset++] = code;
        options[offset++] = length;
        memcpy(&options[offset], data, length);
        offset += length;
    }

    void send_dhcp_offer(const dhcp_message* discover, const std::string& client_ip)
    {
        dhcp_message offer;
        memset(&offer, 0, sizeof(offer));
        // 填充DHCP Offer消息
        offer.op = 2; // 回复
        offer.htype = discover->htype;
        offer.hlen = discover->hlen;
        offer.xid = discover->xid;
        offer.yiaddr = ip_to_int(client_ip);
        offer.siaddr = ip_to_int(server_ip);
        memcpy(offer.chaddr, discover->chaddr, sizeof(offer.chaddr));
        offer.magic_cookie = htonl(0x63825363); // DHCP魔术cookie
        // 设置选项
        int option_offset = 0;
        uint8_t message_type = DHCP_MESSAGE_TYPE_OFFER;
        add_dhcp_option(offer.options, option_offset, 53, &message_type, 1);

        uint32_t subnet_mask_int = ip_to_int(subnet_mask);
        add_dhcp_option(offer.options, option_offset, 1, (uint8_t*)&subnet_mask_int, 4);

        uint32_t router = ip_to_int(gateway);
        add_dhcp_option(offer.options, option_offset, 3, (uint8_t*)&router, 4);

        uint32_t lease_time = htonl(86400); // 24小时租期
        add_dhcp_option(offer.options, option_offset, 51, (uint8_t*)&lease_time, 4);

        uint32_t server_id = ip_to_int(server_ip);
        add_dhcp_option(offer.options, option_offset, 54, (uint8_t*)&server_id, 4);

        offer.options[option_offset++] = 255; // 结束标记

        // 发送Offer
        sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(DHCP_CLIENT_PORT);
        client_addr.sin_addr.s_addr = INADDR_BROADCAST;
        sendto(server_socket, (char*)&offer, sizeof(offer), 0, (sockaddr*)&client_addr, sizeof(client_addr));
        std::cout << "Sent DHCP OFFER for IP: " << client_ip << std::endl;
    }

    void send_dhcp_ack(const dhcp_message* request, const std::string& client_ip) {
        dhcp_message ack;
        memset(&ack, 0, sizeof(ack));
        // 填充DHCP ACK消息
        ack.op = 2; // 回复
        ack.htype = request->htype;
        ack.hlen = request->hlen;
        ack.xid = request->xid;
        ack.yiaddr = ip_to_int(client_ip);
        ack.siaddr = ip_to_int(server_ip);
        memcpy(ack.chaddr, request->chaddr, sizeof(ack.chaddr));
        ack.magic_cookie = htonl(0x63825363); // DHCP魔术cookie
        // 设置选项
        int option_offset = 0;
        uint8_t message_type = DHCP_MESSAGE_TYPE_ACK;
        add_dhcp_option(ack.options, option_offset, 53, &message_type, 1);

        uint32_t subnet_mask_int = ip_to_int(subnet_mask);
        add_dhcp_option(ack.options, option_offset, 1, (uint8_t*)&subnet_mask_int, 4);

        uint32_t router = ip_to_int(gateway);
        add_dhcp_option(ack.options, option_offset, 3, (uint8_t*)&router, 4);

        uint32_t lease_time = htonl(86400); // 24小时租期
        add_dhcp_option(ack.options, option_offset, 51, (uint8_t*)&lease_time, 4);

        uint32_t server_id = ip_to_int(server_ip);
        add_dhcp_option(ack.options, option_offset, 54, (uint8_t*)&server_id, 4);

        ack.options[option_offset++] = 255; // 结束标记
        // 发送ACK
        sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(DHCP_CLIENT_PORT);
        client_addr.sin_addr.s_addr = INADDR_BROADCAST;
        sendto(server_socket, (char*)&ack, sizeof(ack), 0, (sockaddr*)&client_addr, sizeof(client_addr));

        std::cout << "Sent DHCP ACK for IP: " << client_ip << std::endl;
    }

    void process_dhcp_message()
    {
        dhcp_message message;
        sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);
        // 接收DHCP消息
        int bytes_received = recvfrom(server_socket, (char*)&message, sizeof(message), 0,
            (sockaddr*)&client_addr, &client_addr_len);
        if (bytes_received > 0)
        {
            if (message.magic_cookie != htonl(0x63825363))
            {
                return;
            }
            std::string mac_address = mac_to_string(message.chaddr);
            std::cout << "Received DHCP message from MAC: " << mac_address << std::endl;
            // 解析选项字段获取消息类型
            uint8_t message_type = 0;
            for (int i = 0; i < sizeof(message.options) - 2; i++)
            {
                if (message.options[i] == 53)
                {
                    message_type = message.options[i + 2];
                    break;
                }
            }

            std::string allocated_ip = get_allocated_ip(mac_address);
            if (message_type == DHCP_MESSAGE_TYPE_DISCOVER)
            {
                std::cout << "DHCP DISCOVER received" << std::endl;
                if (!allocated_ip.empty())
                {
                    send_dhcp_offer(&message, allocated_ip);
                }
            }
            else if (message_type == DHCP_MESSAGE_TYPE_REQUEST)
            {
                std::cout << "DHCP REQUEST received" << std::endl;
                if (!allocated_ip.empty())
                {
                    send_dhcp_ack(&message, allocated_ip);
                }
            }
        }
    }

    void start()
    {
        std::cout << "DHCP Server started. Listening for requests..." << std::endl;
        while (true)
        {
            process_dhcp_message();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    ~DHCPServer()
    {
        if (server_socket != INVALID_SOCKET)
        {
            closesocket(server_socket);
        }
        WSACleanup();
    }
};

int main() {
    DHCPServer server("192.168.1.1", "255.255.255.0", "192.168.1.1");
    if (server.initialize())
    {
        server.start();
    }
    else
    {
        std::cerr << "Failed to initialize DHCP server!" << std::endl;
        return 1;
    }
    return 0;
}