#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<queue>
using namespace std;
typedef char VertexType;
typedef int EdgeType;
#define MAXVEX 100
#define INFINITE 65535
typedef struct
{
    VertexType vexs[MAXVEX];
    EdgeType arc[MAXVEX][MAXVEX];
    int numVertexes, numEdges;
}MGraph;

//创建图的邻接矩阵
void CreateGraph(MGraph* G, vector<VertexType> V, map<pair<int, int>, EdgeType> VR)
{
    G->numVertexes = V.size();
    G->numEdges = VR.size();
    for (int i = 0; i < V.size(); i++)
        G->vexs[i] = V[i];
    for (int i = 0; i < G->numVertexes; i++)
        for (int j = 0; j < G->numVertexes; j++)
        {
            if (i == j)
            {
                G->arc[i][j] = 0;
                continue;
            }
            G->arc[i][j] = INFINITE;//初始化邻接矩阵
        }
    for (auto vr : VR)
    {
        G->arc[vr.first.first][vr.first.second] = vr.second;
        G->arc[vr.first.second][vr.first.first] = vr.second;//无向图
    }
}

//销毁邻接矩阵
void DestroyGraph(MGraph* G)
{
     for(int i=0;i<G->numVertexes;i++)
     { 
         G->vexs[i]=' ';
         for(int j=0;j<G->numVertexes;j++)
             {
                 if(i==j)
                     continue;
                 G->arc[i][j] = 0;
             }
     }
     G->numVertexes = 0;
     G->numEdges = 0;
}

//返回顶点位置
int LocateVex(MGraph G, VertexType u)
{
    for (int i = 0; i < G.numVertexes; i++)
    {
        if (G.vexs[i] == u)
            return i;
    }
    return -1;//无顶点返回-1
}

//返回v的值
VertexType GetVex(MGraph G, int v)
{
    return G.vexs[v];
}

//对v赋值
void PutVex(MGraph* G, int v, VertexType value)
{
    G->vexs[v] = value;
}

//返回v的第一个邻接顶点
VertexType FirstAdjVex(MGraph G, int v)
{
    for (int i = 0; i < G.numVertexes; i++)
    {
        if (G.arc[v][i] != 0 && G.arc[v][i] != INFINITE)
            return G.vexs[i];
    }
    return -1;//无邻接顶点返回-1
}

//返回v相对于w的下一个邻接顶点
VertexType NextAdjVex(MGraph G, int v, int w)
{
    for (int i = w + 1; i < G.numVertexes; i++)
    {
        if (G.arc[v][i] != 0 && G.arc[v][i] != INFINITE)
            return G.vexs[i];
    }
    return -1;//无下一个邻接顶点返回-1
}

//添加新顶点
void InsertVex(MGraph* G, int v, VertexType w)
{
    if (v < G->numVertexes)
    {
        for (int i = G->numVertexes - 1; i >= v; i--)
        {
            G->vexs[i + 1] = G->vexs[i];//顶点后移
            for (int j = 0; j < i; j++)
                if (G->arc[i][j] != INFINITE)
                {
                    G->arc[i + 1][j] = G->arc[i][j];//邻接矩阵后移
                    G->arc[i][j] = INFINITE;
                    G->arc[j][i + 1] = G->arc[j][i];
                    G->arc[j][i] = INFINITE;
                }
            G->arc[i + 1][v] = INFINITE;
            G->arc[v][i + 1] = INFINITE;
        }
        G->vexs[v] = w;//插入新顶点
        G->arc[G->numVertexes][G->numVertexes] = 0;
        G->numVertexes++;
    }
    else
    {
        G->vexs[G->numVertexes] = w;//插入新顶点
        G->numVertexes++;
    }
}

//删除结点
void DeleteVex(MGraph* G, int v)
{
    for (int i = 0; i < G->numVertexes; i++)
    {
        if (v != i&&G->arc[v][i]!=INFINITE)
        {
            G->arc[v][i] = INFINITE;
            G->arc[i][v] = INFINITE;//删除弧
            G->numEdges--;
        }
    }
    for (int i = v + 1; i < G->numVertexes; i++)
    {
        G->vexs[i - 1] = G->vexs[i];//顶点前移
        for (int j = 0; j < i; j++)
        {
            if (G->arc[i][j] != INFINITE)
            {
                G->arc[i - 1][j] = G->arc[i][j];//邻接矩阵前移
                G->arc[i][j] = INFINITE;
                G->arc[j][i - 1] = G->arc[j][i];
                G->arc[j][i] = INFINITE;
            }
        }
        G->arc[i-1][i-1]=0;
        G->vexs[i] = ' ';
    }
    G->numVertexes--;
}

//插入弧/边
void InsertArc(MGraph* G, int v, int w, EdgeType value)
{
    G->arc[v][w] = value;
    G->arc[w][v] = value;//无向图
    G->numEdges++;
}

//删除弧/边
void DeleteArc(MGraph* G, int v, int w)
{
    G->arc[v][w] = INFINITE;
    G->arc[w][v] = INFINITE;//无向图
    G->numEdges--;
}

//深度优先遍历
bool visited[MAXVEX];
void DFS(MGraph G, int v, void (*visit)(VertexType))
{
    int j;
    visited[v] = true;
    visit(G.vexs[v]);
    for (j = 0; j < G.numVertexes; j++)
    {
        if (!visited[j] && G.arc[v][j] != INFINITE && G.arc[v][j] != 0)//未访问且存在边
            DFS(G, j, visit);
    }
}
void DFSTraverse(MGraph G, void (*visit)(VertexType))
{
    int i;
    for (i = 0; i < G.numVertexes; i++)
        visited[i] = false;//初始化访问标记数组
    for (i = 0; i < G.numVertexes; i++)
        if (!visited[i])//未访问的顶点
            DFS(G, i, visit);
}

//广度优先遍历
void BFSTraverse(MGraph G, void (*visit)(VertexType))
{
    int i, j;
    queue<VertexType> Q;
    for (int i = 0; i < G.numVertexes; i++)
        visited[i] = false;//初始化访问标记数组
    for (int i = 0; i < G.numVertexes; i++)
        if (!visited[i])//未访问的顶点
        {
            visited[i] = true;
            visit(G.vexs[i]);//访问
            Q.push(i);//入队
            while (!Q.empty())
            {
                j = Q.front();
                Q.pop();
                for (int j = 0; j < G.numVertexes; j++)
                {
                    if (G.arc[j][i] != 0 && G.arc[j][i] != INFINITE && !visited[j])//未访问且存在边
                    {
                        Q.push(j);//入队
                        visit(G.vexs[j]);//访问
                        visited[j] = true;
                    }

                }
            }
        }
}

//测试用函数
void Print(VertexType value)
{
    cout << value << " ";
}