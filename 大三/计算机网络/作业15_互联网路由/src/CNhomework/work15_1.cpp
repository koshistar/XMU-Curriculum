#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<algorithm>
using namespace std;
struct RouteEntry
{
	string destination;
	int distance;
	string nextHop;
	RouteEntry(const string& dest, int dist, const string& hop) :destination(dest), distance(dist), nextHop(hop) {}
	void print() const
	{
		cout << "网络： " << destination << "，跳数： " << distance << "，下一跳： " << nextHop << endl;
	}
};
class RIPRouter
{
private:
	string name;
	vector<RouteEntry> routingTable;
	const int MAX_HOP = 16;
public:
	RIPRouter(const string& routerName) :name(routerName) {}
	void addRoute(const string& destination, int distance, const string& nextHop)
	{
		for (auto& route : routingTable)
		{
			if (route.destination == destination)
			{
				route.distance = distance;
				route.nextHop = nextHop;
				return;
			}
		}
		routingTable.emplace_back(destination, distance, nextHop);
	}
	void processRIPUpdate(const string& neighborName, const vector<RouteEntry>& neighborTable)
	{
		for (const auto& neighborRoute : neighborTable)
		{
			int newDistance = neighborRoute.distance + 1;
			if (newDistance >= MAX_HOP)
				continue;
			bool found = false;
			for (auto& currentRoute : routingTable)
			{
				if (currentRoute.destination == neighborRoute.destination)
				{
					found = true;
					if (currentRoute.nextHop == neighborName)
					{
						if (currentRoute.distance != newDistance)
						{
							currentRoute.distance = newDistance;
						}
					}
					else
					{
						if (newDistance < currentRoute.distance)
						{
							currentRoute.distance = newDistance;
							currentRoute.nextHop = neighborName;
						}
					}
					break;
				}
			}
			if (!found)
			{
				addRoute(neighborRoute.destination, newDistance, neighborName);
			}
		}
	}
	void print() const
	{
		cout << "---" << name << "---" << endl;
		for (const auto& route : routingTable)
		{
			route.print();
		}
	}
	vector<RouteEntry> getRoutingTable() const
	{
		return routingTable;
	}
};
int main()
{
	RIPRouter r1("R2");
	r1.addRoute("N1", 1, "R1");
	r1.addRoute("N2", 0, "直接发送");
	r1.addRoute("N3", 6, "R4");
	r1.addRoute("N4", 8, "R5");
	vector<RouteEntry> r2Table;
	r2Table.emplace_back("N2", 2, "R5");
	r2Table.emplace_back("N3", 3, "R6");
	r2Table.emplace_back("N4", 7, "R7");
	r2Table.emplace_back("N5", 3, "R8");
	
	r1.print();
	cout << "---R3---" << endl;
	for (const auto& route : r2Table)
	{
		route.print();
	}
	r1.processRIPUpdate("R3", r2Table);
	r1.print();
	return 0;
}