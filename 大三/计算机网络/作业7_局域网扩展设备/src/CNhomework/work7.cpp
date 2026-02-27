#include<iostream>
#include<map>
#include<vector>
#include<iomanip>
using namespace std;
class Bridge
{
private:
	map<int, int> macTable; // MAC地址，端口号
public:
	void learnMAC(int macAddress, int port)
	{
		if (macAddress == 0xF) // 广播
			return;

		// 如果MAC地址不在表中，或者端口发生了变化，则更新
		if (macTable.find(macAddress) == macTable.end() || macTable[macAddress] != port)
		{
			macTable[macAddress] = port;
			cout << "Learned：MAC Address " << hex << uppercase << macAddress << " in the port " << port << endl;
		}
	}
	int getDestinationPort(int destAddress)
	{
		if (destAddress == 0xF)
			return 3;// 表示广播到所有端口
		if (macTable.find(destAddress) != macTable.end())
		{
			return macTable[destAddress];
		}
		return 3;// 未知地址，泛洪
	}
	int processFrame(int srcAddress, int srcPort, int destAddress)
	{
		learnMAC(srcAddress, srcPort);
		return getDestinationPort(destAddress);
	}

	void displayMACTable()
	{
		cout << "\nCurrent MAC Table:\n";
		cout << "MAC Address\tPort" << endl;
		for (const auto& entry : macTable)
		{
			cout << "\t" << hex << uppercase << entry.first << "\t" << entry.second << endl;
		}
		cout << endl;
	}
	void clearMACTable()
	{
		macTable.clear();
	}
};
struct Frame {
	int srcAddress;
	int srcPort;
	int destAddress;
	Frame(int sAddress, int sPort, int dAddress) :srcAddress(sAddress), srcPort(sPort), destAddress(dAddress)
	{}
};

int main()
{
	Bridge bridge;
	vector<Frame> frames;

	frames.push_back(Frame(0x1, 1, 0x2)); //主机1 端口1 -> 主机2
	frames.push_back(Frame(0x2, 2, 0x1)); //主机2 端口2 -> 主机1
	frames.push_back(Frame(0x3, 1, 0x4)); //主机3 端口1 -> 未知主机4
	frames.push_back(Frame(0x4, 2, 0x3)); //主机4 端口2 -> 主机3
	frames.push_back(Frame(0x5, 1, 0xF)); //主机5 端口1 广播
	frames.push_back(Frame(0x1, 1, 0x3)); //主机1 端口1 -> 主机3

	for (int i = 0; i < frames.size(); i++)
	{
		Frame frame = frames[i];
		cout << "\nframe " << dec << i + 1 << ":" << endl;
		cout << "Source address: " << hex << uppercase << frame.srcAddress << ", source port: " << frame.srcPort << ", destination address: " << frame.destAddress << endl;

		int destPort = bridge.processFrame(frame.srcAddress, frame.srcPort, frame.destAddress);
		cout << "Destination port: " << destPort << endl;

		bridge.displayMACTable();
	}
	return 0;
}