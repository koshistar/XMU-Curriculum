#pragma once
#include<iostream>
#define m 9
#define n 13
using namespace std;
typedef char VexType;
bool visited[m];
typedef struct EdgeNode
{
    int adjvex;
    struct EdgeNode* next;
}EdgeNode;
typedef struct VertexNode
{
    VexType outdata;
    EdgeNode* firstedge;
}VertexNode, AdjList[m];
typedef struct
{
    AdjList adjList;
    int numVertexes, numEdges;
}GraphAdjList;
void CreateAdjList(GraphAdjList* G)
{
    G->numVertexes = m;
    G->numEdges = n;
    EdgeNode* e;
    cout << "输入顶点信息(m=" << m << "):\n";
    for (int i = 0; i < m; i++)
    {
        cin >> G->adjList[i].outdata;
        G->adjList[i].firstedge = NULL;
        visited[i] = false;
    }
    cout << "依次输入(vi,vj)的下标(n=" << n << "):\n";
    for (int i = 0; i < n; i++)
    {
        e = new EdgeNode;
        int v1, v2;
        cin >> v1 >> v2;
        e->adjvex = v1;
        e->next = G->adjList[v2].firstedge;
        G->adjList[v2].firstedge = e;
        e = new EdgeNode;
        e->adjvex = v2;
        e->next = G->adjList[v1].firstedge;
        G->adjList[v1].firstedge = e;
    }
}
bool exist_path_len(GraphAdjList G, int v1, int v2, int k)
{
    if (v1 == v2 && k == 0)
        return true;
    else if (k > 0)
    {
        visited[v1] = true;
        for (auto p = G.adjList[v1].firstedge; p; p = p->next)
        {
            int x = p->adjvex;
            if (!visited[x])
            {
                if (exist_path_len(G, x, v2, k - 1))
                {
                    cout << x << endl;
                    return true;
                }
            }
        }
        visited[v1] = false;
    }
    return false;
}