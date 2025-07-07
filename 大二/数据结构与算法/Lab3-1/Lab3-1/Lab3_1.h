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

//����ͼ���ڽӾ���
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
            G->arc[i][j] = INFINITE;//��ʼ���ڽӾ���
        }
    for (auto vr : VR)
    {
        G->arc[vr.first.first][vr.first.second] = vr.second;
        G->arc[vr.first.second][vr.first.first] = vr.second;//����ͼ
    }
}

//�����ڽӾ���
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

//���ض���λ��
int LocateVex(MGraph G, VertexType u)
{
    for (int i = 0; i < G.numVertexes; i++)
    {
        if (G.vexs[i] == u)
            return i;
    }
    return -1;//�޶��㷵��-1
}

//����v��ֵ
VertexType GetVex(MGraph G, int v)
{
    return G.vexs[v];
}

//��v��ֵ
void PutVex(MGraph* G, int v, VertexType value)
{
    G->vexs[v] = value;
}

//����v�ĵ�һ���ڽӶ���
VertexType FirstAdjVex(MGraph G, int v)
{
    for (int i = 0; i < G.numVertexes; i++)
    {
        if (G.arc[v][i] != 0 && G.arc[v][i] != INFINITE)
            return G.vexs[i];
    }
    return -1;//���ڽӶ��㷵��-1
}

//����v�����w����һ���ڽӶ���
VertexType NextAdjVex(MGraph G, int v, int w)
{
    for (int i = w + 1; i < G.numVertexes; i++)
    {
        if (G.arc[v][i] != 0 && G.arc[v][i] != INFINITE)
            return G.vexs[i];
    }
    return -1;//����һ���ڽӶ��㷵��-1
}

//����¶���
void InsertVex(MGraph* G, int v, VertexType w)
{
    if (v < G->numVertexes)
    {
        for (int i = G->numVertexes - 1; i >= v; i--)
        {
            G->vexs[i + 1] = G->vexs[i];//�������
            for (int j = 0; j < i; j++)
                if (G->arc[i][j] != INFINITE)
                {
                    G->arc[i + 1][j] = G->arc[i][j];//�ڽӾ������
                    G->arc[i][j] = INFINITE;
                    G->arc[j][i + 1] = G->arc[j][i];
                    G->arc[j][i] = INFINITE;
                }
            G->arc[i + 1][v] = INFINITE;
            G->arc[v][i + 1] = INFINITE;
        }
        G->vexs[v] = w;//�����¶���
        G->arc[G->numVertexes][G->numVertexes] = 0;
        G->numVertexes++;
    }
    else
    {
        G->vexs[G->numVertexes] = w;//�����¶���
        G->numVertexes++;
    }
}

//ɾ�����
void DeleteVex(MGraph* G, int v)
{
    for (int i = 0; i < G->numVertexes; i++)
    {
        if (v != i&&G->arc[v][i]!=INFINITE)
        {
            G->arc[v][i] = INFINITE;
            G->arc[i][v] = INFINITE;//ɾ����
            G->numEdges--;
        }
    }
    for (int i = v + 1; i < G->numVertexes; i++)
    {
        G->vexs[i - 1] = G->vexs[i];//����ǰ��
        for (int j = 0; j < i; j++)
        {
            if (G->arc[i][j] != INFINITE)
            {
                G->arc[i - 1][j] = G->arc[i][j];//�ڽӾ���ǰ��
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

//���뻡/��
void InsertArc(MGraph* G, int v, int w, EdgeType value)
{
    G->arc[v][w] = value;
    G->arc[w][v] = value;//����ͼ
    G->numEdges++;
}

//ɾ����/��
void DeleteArc(MGraph* G, int v, int w)
{
    G->arc[v][w] = INFINITE;
    G->arc[w][v] = INFINITE;//����ͼ
    G->numEdges--;
}

//������ȱ���
bool visited[MAXVEX];
void DFS(MGraph G, int v, void (*visit)(VertexType))
{
    int j;
    visited[v] = true;
    visit(G.vexs[v]);
    for (j = 0; j < G.numVertexes; j++)
    {
        if (!visited[j] && G.arc[v][j] != INFINITE && G.arc[v][j] != 0)//δ�����Ҵ��ڱ�
            DFS(G, j, visit);
    }
}
void DFSTraverse(MGraph G, void (*visit)(VertexType))
{
    int i;
    for (i = 0; i < G.numVertexes; i++)
        visited[i] = false;//��ʼ�����ʱ������
    for (i = 0; i < G.numVertexes; i++)
        if (!visited[i])//δ���ʵĶ���
            DFS(G, i, visit);
}

//������ȱ���
void BFSTraverse(MGraph G, void (*visit)(VertexType))
{
    int i, j;
    queue<VertexType> Q;
    for (int i = 0; i < G.numVertexes; i++)
        visited[i] = false;//��ʼ�����ʱ������
    for (int i = 0; i < G.numVertexes; i++)
        if (!visited[i])//δ���ʵĶ���
        {
            visited[i] = true;
            visit(G.vexs[i]);//����
            Q.push(i);//���
            while (!Q.empty())
            {
                j = Q.front();
                Q.pop();
                for (int j = 0; j < G.numVertexes; j++)
                {
                    if (G.arc[j][i] != 0 && G.arc[j][i] != INFINITE && !visited[j])//δ�����Ҵ��ڱ�
                    {
                        Q.push(j);//���
                        visit(G.vexs[j]);//����
                        visited[j] = true;
                    }

                }
            }
        }
}

//�����ú���
void Print(VertexType value)
{
    cout << value << " ";
}