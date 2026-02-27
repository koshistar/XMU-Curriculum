#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<set>
#include<queue>
#include<sstream>
#include<algorithm>
using namespace std;
struct Edge
{
	string target;
	int weight;
	Edge(const string& t, int w) :target(t), weight(w) {}
};

class Graph
{
private:
	map<string, vector<Edge>>adjList;
	set<string> nodes;
public:
	void addEdge(const string& src, const string& dest, int weight)
	{
		adjList[src].emplace_back(dest, weight);
		adjList[dest].emplace_back(src, weight);//无向图
		nodes.insert(src);
		nodes.insert(dest);
	}
	vector<string> getNodes() const
	{
		return vector<string>(nodes.begin(), nodes.end());
	}
	vector<Edge> getEdges(const string& node) const
	{
		auto it = adjList.find(node);
		if (it != adjList.end())
		{
			return it->second;
		}
		return vector<Edge>();
	}
	void printGraph() const
	{
		for (const auto& entry : adjList)
		{
			cout << entry.first << "->";
			for (const auto& edge : entry.second)
			{
				cout << "(" << edge.target << ", " << edge.weight << ")";
			}
			cout << endl;
		}
	}
};

map<string, pair<int, string>> dijkstra(const Graph& graph, const string& start)
{
	map<string, pair<int, string>> distances;
	priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;
	for (const auto& node : graph.getNodes())
		distances[node] = { INT_MAX,"" };
	distances[start] = { 0,"" };
	pq.push({ 0,start });
	while (!pq.empty())
	{
		int currentDist = pq.top().first;
		string currentNode = pq.top().second;
		pq.pop();
		if (currentDist > distances[currentNode].first)
			continue;
		for (const auto& edge : graph.getEdges(currentNode))
		{
			string neighbor = edge.target;
			int newDist = currentDist + edge.weight;
			if (newDist < distances[neighbor].first)
			{
				distances[neighbor] = { newDist,currentNode };
				pq.push({ newDist,neighbor });
			}
		}
	}
	return distances;
}
string getPath(const string& start, const string& end, const map<string, pair<int, string>>& distances)
{
	if (distances.at(end).first == INT_MAX)
		return "None";

	vector<string> path;
	string current = end;
	while (current != start)
	{
		path.push_back(current);
		current = distances.at(current).second;
	}
	path.push_back(start);
	reverse(path.begin(), path.end());
	stringstream ss;
	for (size_t i = 0; i < path.size(); ++i)
	{
		if (i > 0)
			ss << " -> ";
		ss << path[i];
	}
	return ss.str();
}
void printPath(const Graph& graph, const string& start, const map<string, pair<int, string>>& distances)
{
	cout << "从节点\'" << start << "\'出发的最短路径： " << endl;
	for (const auto& node : graph.getNodes())
	{
		if (node == start)
			continue;
		int dist = distances.at(node).first;
		if (dist == INT_MAX)
		{
			cout << start << "->" << node << ": 不可达" << endl;
		}
		else
		{
			cout << start << "->" << node << ": 距离=" << dist << ", 路径： " << getPath(start, node, distances) << endl;
		}
	}
}
vector<tuple<string, string, int>> getInput()
{
	vector<tuple<string, string, int>> edges;
	string line;
	cout << "输入三元组：(r1,r2,weight)" << endl;
	while (true)
	{
		getline(cin, line);
		if (line.empty())
			break;
		stringstream ss(line);
		string node1, node2;
		int weight;
		if (ss >> node1 >> node2 >> weight)
		{
			edges.emplace_back(node1, node2, weight);
		}
		else
		{
			cout << "输入格式错误" << endl;
		}
	}
	return edges;
}
int main()
{
	Graph graph;
	auto edges = getInput();
	for (const auto& edge:edges)
	{
		graph.addEdge(get<0>(edge), get<1>(edge), get<2>(edge));
	}
	string startNode;
	cout << "输入起点节点： ";
	cin >> startNode;
	auto distances = dijkstra(graph, startNode);
	printPath(graph, startNode, distances);
	return 0;
}