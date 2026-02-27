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
typedef struct tcp_header
{
    u_short sport;
    u_short dport;
    u_int seq;
    u_int ack;
    u_char th_off;
    u_char flags;
    u_short win;
    u_short sum;
    u_short urp;
}tcp_header;
struct FTPSession
{
	string client_ip;
	string server_ip;
	string username;
	string password;
	string status;//"PENDING" "SUCCESS" "FAILED"
	time_t start_time;
};

FILE* fp;
FILE* ftp_log_fp;
const int STAT_INTERVAL = 60;
//FTP会话管理
map<string, FTPSession> ftp_sessions;
// 统计变量
map<string, long long> src_mac_stats;
map<string, long long> dst_mac_stats;
map<string, long long> src_ip_stats;
map<string, long long> dst_ip_stats;
time_t last_stat_time;

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
	FILE* stat_fp = fopen("traffic_statistics_ftp.csv", "a");
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
/*解析FTP协议*/
void parse_ftp_protocol(const u_char* payload, int payload_len, const string& src_ip, const string& dst_ip, const string& src_mac, const string& dst_mac, u_short src_port, u_short dst_port)
{
	string data((char*)payload, payload_len);
	string session_key;
	if (dst_port == 21) { // 客户端到服务器
		session_key = src_ip + ":" + to_string(src_port);
	}
	else if (src_port == 21) { // 服务器到客户端
		session_key = dst_ip + ":" + to_string(dst_port);
	}
	else {
		return; // 不是FTP控制连接
	}
	// 处理FTP命令（客户端->服务器）
	if (dst_port == 21) {
		if (data.find("USER ") == 0) {
			string username = data.substr(5);
			size_t pos = username.find_first_of("\r\n");
			if (pos != string::npos) {
				username = username.substr(0, pos);
			}
			ftp_sessions[session_key] = {
				src_ip, dst_ip, username, "", "PENDING", time(nullptr)
			};
			cout << "检测到FTP登录尝试 - 用户名: " << username << endl;
		}
		else if (data.find("PASS ") == 0) {
			string password = data.substr(5);
			size_t pos = password.find_first_of("\r\n");
			if (pos != string::npos) {
				password = password.substr(0, pos);
			}
			if (ftp_sessions.find(session_key) != ftp_sessions.end()) {
				ftp_sessions[session_key].password = password;
				cout << "检测到FTP密码 - 会话: " << session_key << endl;
			}
		}
	}
	// 处理FTP响应（服务器->客户端）
	else if (src_port == 21) {
		if (ftp_sessions.find(session_key) != ftp_sessions.end()) {
			FTPSession& session = ftp_sessions[session_key];
			if (data.find("230") == 0) {
				session.status = "SUCCEED";
				string timestamp = get_current_time();
				fprintf(ftp_log_fp, "%s,%s,%s,%s,%s,%s,%s,%s\n", timestamp.c_str(), src_mac.c_str(), src_ip.c_str(), dst_mac.c_str(), dst_ip.c_str(), session.username.c_str(), session.password.c_str(), "SUCCEED");
				cout << "FTP登录成功: " << session.username << endl;

				ftp_sessions.erase(session_key);
			}
			else if (data.find("530") == 0) {
				session.status = "FAILED";
				string timestamp = get_current_time();
				fprintf(ftp_log_fp, "%s,%s,%s,%s,%s,%s,%s,%s\n", timestamp.c_str(), src_mac.c_str(), src_ip.c_str(), dst_mac.c_str(), dst_ip.c_str(), session.username.c_str(), session.password.c_str(), "FAILED");
				cout << "FTP登录失败: " << session.username << endl;

				ftp_sessions.erase(session_key);
			}
		}
	}
}
void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data)
{
	char timestr[40];
	u_char* payload;
	ip_header* ih;
	mac_header* mh;
	time_t local_tv_sec;

	local_tv_sec = header->ts.tv_sec;
	strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", localtime(&local_tv_sec));

	mh = (mac_header*)pkt_data;
	//ih = (ip_header*)(pkt_data + sizeof(mac_header));
	if (mh->type[0] == 0x08 && mh->type[1] == 0x00) {
		ip_header* ih = (ip_header*)(pkt_data + 14);

		string src_mac = mac_to_string(mh->src_addr);
		string dst_mac = mac_to_string(mh->dest_addr);
		string src_ip = ip_to_string(ih->saddr);
		string dst_ip = ip_to_string(ih->daddr);

		printf("%s,", timestr);
		fprintf(fp, "%s,", timestr);
		printf("%s,%s,%s,%s,%d\n", src_mac.c_str(), src_ip.c_str(), dst_mac.c_str(), dst_ip.c_str(), header->len);
		fprintf(fp, "%s,%s,%s,%s,%d\n", src_mac.c_str(), src_ip.c_str(), dst_mac.c_str(), dst_ip.c_str(), header->len);

		src_mac_stats[src_mac] += header->len;
		dst_mac_stats[dst_mac] += header->len;
		src_ip_stats[src_ip] += header->len;
		dst_ip_stats[dst_ip] += header->len;

		// 检查是否为TCP协议
		if (ih->proto == 6) {
			tcp_header* th = (tcp_header*)(pkt_data + 14 + (ih->ver_ihl & 0x0F) * 4);
			u_short src_port = ntohs(th->sport);
			u_short dst_port = ntohs(th->dport);

			int ip_header_len = (ih->ver_ihl & 0x0F) * 4;
			int tcp_header_len = (th->th_off >> 4) * 4;
			int payload_len = header->len - 14 - ip_header_len - tcp_header_len;

			if (payload_len > 0) {
				const u_char* payload = pkt_data + 14 + ip_header_len + tcp_header_len;

				// 检查是否为FTP流量（端口21）
				if (src_port == 21 || dst_port == 21) {
					parse_ftp_protocol(payload, payload_len, src_ip, dst_ip, src_mac, dst_mac, src_port, dst_port);
				}
			}
		}
		time_t current_time = time(nullptr);
		if (difftime(current_time, last_stat_time) >= STAT_INTERVAL) {
			write_statistics();
			last_stat_time = current_time;
		}
	}
}

int main()
{
	pcap_if_t* alldevs;
	pcap_if_t* d;
	int i = 0;
	pcap_t* adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	char packet_filter[] = "port 21";
	struct bpf_program fcode;

	fp = fopen("network_traffic_ftp.csv", "w");
	if (!fp) 
		exit(0);
	fprintf(fp, "时间,源MAC,源IP,目标MAC,目标IP,帧长度\n");

	ftp_log_fp = fopen("ftp_logins.csv", "w");
	if (!ftp_log_fp)
		exit(0);
	fprintf(ftp_log_fp, "时间,源MAC,源IP,目标MAC,目标IP,登录名,口令,成功与否\n");

	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		fclose(fp);
		fclose(ftp_log_fp);
		exit(1);
	}

	for (d = alldevs; d != NULL; d = d->next) {
		cout << ++i << ". " << d->name;
		if (d->description)
			cout << " (" << d->description << ")";
		else
			cout << "(No description available)";
		cout << endl;
	}

	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		pcap_freealldevs(alldevs);
		fclose(fp);
		fclose(ftp_log_fp);
		return -1;
	}

	int inum;
	printf("Enter the interface number (1-%d):", i);
	cin >> inum;

	if (inum < 1 || inum > i)
	{
		printf("\nAdapter number out of range.\n");
		pcap_freealldevs(alldevs);
		fclose(fp);
		fclose(ftp_log_fp);
		return -1;
	}

	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	if ((adhandle = pcap_open_live(d->name,	65536, 1, 1000,	errbuf )) == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n");
		pcap_freealldevs(alldevs);
		fclose(fp);
		fclose(ftp_log_fp);
		return -1;
	}

	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		pcap_freealldevs(alldevs);
		fclose(fp);
		fclose(ftp_log_fp);
		return -1;
	}

	if (d->addresses != NULL)
		netmask = ((struct sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		netmask = 0xffffff;

	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
	{
		fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		pcap_freealldevs(alldevs);
		fclose(fp);
		fclose(ftp_log_fp);
		return -1;
	}

	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		fprintf(stderr, "\nError setting the filter.\n");
		pcap_freealldevs(alldevs);
		fclose(fp);
		fclose(ftp_log_fp);
		return -1;
	}

	printf("\nlistening on %s...\n", d->description);
	pcap_freealldevs(alldevs);
	pcap_loop(adhandle, 0, packet_handler, NULL);
	write_statistics();
	fclose(fp);
	fclose(ftp_log_fp);
	return 0;
}