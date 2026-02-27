#define _CRT_SECURE_NO_WARNINGS
#define HAVE_REMOTE
#define WIN32
#include<string>
#include<iostream>
#include<ctime>
#include<fstream>
#include<iomanip>
#include<map>
#include <pcap.h>
#include <Packet32.h>
#include <ntddndis.h>
#pragma comment(lib, "Packet")
#pragma comment(lib, "wpcap")
#pragma comment(lib, "WS2_32")
using namespace std;
typedef struct ip_header
{
    u_char ver_ihl;
    u_char tos;
    u_short tlen;
    u_short identification;
    u_short flags_fo;
    u_char ttl;
    u_char proto;
    u_short crc;
    u_char saddr[4];
    u_char daddr[4];
    u_int op_pad;
}ip_header;
typedef struct mac_header
{
    u_char dest_addr[6];
    u_char src_addr[6];
    u_char type[2];
}mac_header;

// 全局统计变量
map<string, long long> src_mac_stats;
map<string, long long> dst_mac_stats;
map<string, long long> src_ip_stats;
map<string, long long> dst_ip_stats;
time_t last_stat_time;
const int STAT_INTERVAL = 60;
FILE* fp;

/*格式化地址*/
void format_addr(u_char* addr, u_char* byte) {
    printf("%02x:%02x:%02x:%02x:%02x:%02x,", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    fprintf(fp, "%02x:%02x:%02x:%02x:%02x:%02x,", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    printf("%d.%d.%d.%d,", byte[0], byte[1], byte[2], byte[3]);
    fprintf(fp, "%d.%d.%d.%d,", byte[0], byte[1], byte[2], byte[3]);
}
/*获取当前时间字符串*/
string get_current_time()
{
    time_t now = time(nullptr);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(time_str);
}
/*格式化MAC地址*/
string mac_to_string(u_char* mac) {
    char buffer[18];
    sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return string(buffer);
}
/*格式化IP地址*/
string ip_to_string(u_char* ip) {
    char buffer[16];
    sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return string(buffer);
}
void write_statistics()
{
    FILE* stat_fp = fopen("traffic_statistics.csv", "a");
    if (!stat_fp)
    {
        cerr << "Cannot open file!" << endl;
        return;
    }
    string timestamp = get_current_time();
    fprintf(stat_fp, "\n统计时间: %s\n", timestamp.c_str());
    fprintf(stat_fp, "源MAC地址统计:\n");
    fprintf(stat_fp, "MAC地址,总数据长度(字节)\n");
    for (const auto& stat : src_mac_stats) {
        fprintf(stat_fp, "%s,%lld\n", stat.first.c_str(), stat.second);
    }
    fprintf(stat_fp, "目标MAC地址统计:\n");
    fprintf(stat_fp, "MAC地址,总数据长度(字节)\n");
    for (const auto& stat : dst_mac_stats) {
        fprintf(stat_fp, "%s,%lld\n", stat.first.c_str(), stat.second);
    }
    fprintf(stat_fp, "源IP地址统计:\n");
    fprintf(stat_fp, "IP地址,总数据长度(字节)\n");
    for (const auto& stat : src_ip_stats) {
        fprintf(stat_fp, "%s,%lld\n", stat.first.c_str(), stat.second);
    }
    fprintf(stat_fp, "目标IP地址统计:\n");
    fprintf(stat_fp, "IP地址,总数据长度(字节)\n");
    for (const auto& stat : dst_ip_stats) {
        fprintf(stat_fp, "%s,%lld\n", stat.first.c_str(), stat.second);
    }
    fclose(stat_fp);
}
/*prototype of the packet handler*/
void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data)
{
    struct tm* ltime;
    char timestr[40];
    ip_header* ih;
    mac_header* mh;
    u_int ip_len;
    time_t local_tv_sec;

    // covert the timestamp to readable format
    local_tv_sec = header->ts.tv_sec;
    strftime(timestr, sizeof timestr, "%Y-%m-%d %H:%M:%S", localtime(&local_tv_sec));

    printf("%s,",timestr);
    fprintf(fp, "%s,", timestr);

    mh = (mac_header*)pkt_data;
    ih = (ip_header*)(pkt_data + 14);
    // 检查是否为IP数据包
    if (mh->type[0] == 0x08 && mh->type[1] == 0x00)
    {
        format_addr(mh->src_addr, ih->saddr);
        format_addr(mh->dest_addr, ih->daddr);
        printf("%d\n", header->len);
        fprintf(fp, "%d\n", header->len);

        string src_mac = mac_to_string(mh->src_addr);
        string dst_mac = mac_to_string(mh->dest_addr);
        string src_ip = ip_to_string(ih->saddr);
        string dst_ip = ip_to_string(ih->daddr);
        src_mac_stats[src_mac] += header->len;
        dst_mac_stats[dst_mac] += header->len;
        src_ip_stats[src_ip] += header->len;
        dst_ip_stats[dst_ip] += header->len;

        time_t current_time = time(nullptr);
        if (difftime(current_time, last_stat_time) >= STAT_INTERVAL)
        {
            write_statistics();
            last_stat_time = current_time;
        }
    }
    else
    {
        printf("非IP数据包,,,,%d\n", header->len);
        fprintf(fp, "非IP数据包,,,,%d\n", header->len);
    }
}
int main()
{
    pcap_if_t* alldevs;
    pcap_if_t* d;
    int i = 0;
    char errbuf[PCAP_ERRBUF_SIZE];
    fp = fopen("network_traffic.csv", "w");
    if (!fp)
        exit(0);
    fprintf(fp, "时间,源 MAC,源 IP,目标 MAC,目标 IP,帧长度\n");

    // 获取网络设备列表
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        cerr << "获取设备列表失败: " << errbuf << endl;
        return -1;
    }
    // 显示设备列表
    cout << "可用的网络设备:" << endl;
    for (d = alldevs; d != NULL; d = d->next) {
        cout << ++i << ". " << d->name;
        if (d->description)
            cout << " (" << d->description << ")";
        else
            cout << "(No description available)";
        cout << endl;
    }
    if (i == 0) {
        cout << "未找到网络设备!" << endl;
        pcap_freealldevs(alldevs);
        return -1;
    }
    // 选择设备
    cout << "选择要监听的设备 (1-" << i << "): ";
    int inum;
    cin >> inum;
    if (inum < 1 || inum > i) {
        cout << "选择无效!" << endl;
        pcap_freealldevs(alldevs);
        return -1;
    }
    // 跳转到选中的设备
    for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

    // 打开网络设备
    pcap_t* adhandle;
    
    if ((adhandle = pcap_open(d->name, 65536, 1, 1000, NULL, errbuf)) == NULL) {
        cerr << "打开设备失败: " << errbuf << endl;
        pcap_freealldevs(alldevs);
        return -1;
    }

    u_int netmask;
    struct bpf_program fcode;
    char packet_filter[] = "ip";
    //预处理
    if (pcap_datalink(adhandle) != DLT_EN10MB) {
        fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
        pcap_freealldevs(alldevs);
        return -1;
    }
    if (d->addresses != NULL)
        netmask = ((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
    else
        netmask = 0xffffff;
    //编译过滤器
    if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0) {
        fprintf(stderr, "\nUnable to compile the packet filter.Check the syntax.\n");
        pcap_freealldevs(alldevs);
        return -1;
    }
    //设置过滤器
    if (pcap_setfilter(adhandle, &fcode) < 0) {
        fprintf(stderr, "\nError setting the filter.\n");
        pcap_freealldevs(alldevs);
        return -1;
    }

    printf("\nlistening on %s...\n", d->description);
    //释放设备列表
    pcap_freealldevs(alldevs);
    // 开始捕获数据包
    pcap_loop(adhandle, 0, packet_handler, NULL);
    
    fclose(fp);
    return 0;
}