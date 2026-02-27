#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;
struct Fragment
{
	int length;
	int offset;
	Fragment(int length, int offset) :length(length), offset(offset) {}
};

vector<Fragment> fragmentPacket(int packetLength, const vector<int>& pathMTUs)
{
	const int IP_HEADER_SIZE = 20;
	vector<Fragment> fragments;
	int dataLength = packetLength - IP_HEADER_SIZE;

	int currentOffset = 0;
	int remainingData = dataLength;
	for (size_t i = 0; i < pathMTUs.size(); i++)
	{
		int currentMTU = pathMTUs[i];
		int maxDataPerFragment = currentMTU - IP_HEADER_SIZE;
		maxDataPerFragment = (maxDataPerFragment / 8) * 8;

		bool allFragmentsValid = true;
		for (const Fragment& fragment : fragments)
		{
			if (fragment.length > currentMTU)
			{
				allFragmentsValid = false;
				break;
			}
		}

		if (allFragmentsValid && remainingData <= 0)
			break;
		if (!allFragmentsValid || remainingData > 0)
		{
			vector<Fragment> newFragments;
			currentOffset = 0;
			remainingData = dataLength;
			while (remainingData > 0)
			{
				int fragmentDataSize = min(remainingData, maxDataPerFragment);
				newFragments.push_back(Fragment(fragmentDataSize, currentOffset));
				
				currentOffset += fragmentDataSize / 8;
				remainingData -= fragmentDataSize;
			}
			fragments = newFragments;
		}
	}
	return fragments;
}

void printResult(const vector<Fragment>& fragments)
{
	for (size_t i = 0; i < fragments.size(); i++)
	{
		cout << "分片" << i + 1 << " : (" << fragments[i].length << "," << fragments[i].offset << ")" << endl;
	}
	cout << endl;
}
int main()
{
	vector<int> path1 = { 1500,576,1500 };
	vector<Fragment> fragments1 = fragmentPacket(1500, path1);
	cout << "1500B报文，路径MTU[1500,576,1500]\n" << endl;
	printResult(fragments1);
	vector<int> path2 = { 1500,576,1500 };
	vector<Fragment> fragments2 = fragmentPacket(500, path2);
	cout << "500B报文，路径MTU[1500,576,1500]\n" << endl;
	printResult(fragments2);
	vector<int> path3 = { 1500,1000,576 };
	vector<Fragment> fragments3 = fragmentPacket(4000, path3);
	cout << "4000B报文，路径MTU[1500,1000,576]\n" << endl;
	printResult(fragments3);
	vector<int> path4 = { 576,1000,1500 };
	vector<Fragment> fragments4 = fragmentPacket(1200, path4);
	cout << "1200B报文，路径MTU[576,1000,1500]\n" << endl;
	printResult(fragments4);
	return 0;
}