#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<queue>
using namespace std;
typedef char VertexType;
typedef int EdgeType;
#define MAXVEX 10

//�Զ�������
struct cmp
{
    bool operator()(pair<int, int> const& a, pair<int, int> const& b)const
    {
        if (a.second != b.second)
            return a.second > b.second;
        else
            return a.first < b.first;
    }
};

typedef struct EdgeNode
{
    int adjvex;
    EdgeType weight;
    struct EdgeNode* next;
}EdgeNode;

typedef struct VertexNode
{
    VertexType data;
    EdgeNode* firstedge;
}VertexNode, AdjList[MAXVEX];

typedef struct
{
    AdjList adjlist;
    int numVertexes, numEdges;
}GrapghAdjList;

//�����ڽӱ�
void CreateGraph(GrapghAdjList* G, vector<VertexType> v, map<pair<int, int>, EdgeType,cmp> VR)
{
    G->numVertexes = v.size();
    G->numEdges = VR.size();
    for (int i = 0; i < G->numVertexes; i++)
    {
        G->adjlist[i].data = v[i];
        G->adjlist[i].firstedge = NULL;
    }
    EdgeNode* e;
    for (auto vr : VR)
    {
        e = new EdgeNode;
        e->adjvex = vr.first.second;
        e->weight = vr.second;
        e->next = G->adjlist[vr.first.first].firstedge;
        G->adjlist[vr.first.first].firstedge = e;
        //EdgeNode* cur = G->adjlist[vr.first.first].firstedge;
        //if (cur == NULL || cur->adjvex > e->adjvex)
        //{
        //    e->next = cur;
        //    cur = e;
        //    continue;
        //}
        //EdgeNode* pre = cur;
        //cur = cur->next;
        ////ȷ������˳��
        //while (cur != NULL && cur->adjvex < e->adjvex)
        //{
        //    pre = cur;
        //    cur = cur->next;
        //}
        //e->next = cur;
        //pre->next = e;
        //e = new EdgeNode;
        //e->adjvex = vr.first.first;
        //e->weight = vr.second;
        //e->next = G->adjlist[vr.first.second].firstedge;
        //G->adjlist[vr.first.second].firstedge = e;//˫��ͼ
    }
    //����
    //for (int i = 0; i < G->numVertexes; i++)
    //{
    //    EdgeNode* cur, * pre, * p;
    //    cur = G->adjlist[i].firstedge ? G->adjlist[i].firstedge->next : NULL;
    //    p = pre = G->adjlist[i].firstedge;
    //    while (cur != NULL)
    //    {
    //        //ȷ��ͷ���
    //        if (cur->adjvex < pre->adjvex)
    //        {
    //            G->adjlist[i].firstedge = cur;
    //        }
    //    }
    //}
}

//�����ڽӱ�
void DestroyGraph(GrapghAdjList* G)
{
    for (int i = 0; i < G->numVertexes; i++)
    {
        EdgeNode* cur = G->adjlist[i].firstedge;
        while (cur)
        {
            EdgeNode* temp = cur;
            cur = cur->next;
            temp->next = NULL;
            free(temp);
        }
    }
}

//���ض���λ��
int LocateVex(GrapghAdjList G, VertexType u)
{
    for (int i = 0; i < G.numVertexes; i++)
    {
        if (G.adjlist[i].data == u)
            return i;
    }
    return -1;//�޶��㷵��-1
}

//����v��ֵ
VertexType GetVex(GrapghAdjList G, int v)
{
    return G.adjlist[v].data;
}

//��v��ֵ
void PutVex(GrapghAdjList* G, int v, VertexType value)
{
    for (int i = 0; i < G->numVertexes; i++)
    {
        //�Ƚ�v������޸�
        if (i != v)
        {
            EdgeNode* temp = G->adjlist[i].firstedge;
            while (temp)
            {
                if (temp->adjvex == G->adjlist[v].data)
                {
                    temp->adjvex = value;
                    break;
                }
                temp = temp->next;
            }
        }
    }
    G->adjlist[v].data = value;
}

//����v�ĵ�һ���ڽӶ���
VertexType FirstAdjVex(GrapghAdjList G, int v)
{
    if (G.adjlist[v].firstedge)
        return G.adjlist[G.adjlist[v].firstedge->adjvex].data;
    return -1;//���ڽӽ�㷵��-1
}

//����v�����w����һ���ڽӶ���
VertexType NextAdjVex(GrapghAdjList G, int v, int w)
{
    EdgeNode* temp = G.adjlist[v].firstedge;
    while (temp != NULL && temp->adjvex != w)
    {
        temp = temp->next;
    }
    if (temp != NULL && temp->next != NULL)
    {
        return G.adjlist[temp->next->adjvex].data;
    }
    return -1;//����һ���ڽӶ��㷵��-1
}

//����¶���
void InsertVex(GrapghAdjList* G, int v, VertexType w)
{
    if (v < G->numVertexes)
    {
        for(int i=0;i<G->numVertexes;i++)
        {
            EdgeNode* temp = G->adjlist[i].firstedge;
            while (temp)
            {
                if (temp->adjvex > v)//��v����Ⱥ���
                    temp->adjvex++;
                temp = temp->next;
            }
        }
        for (int i = G->numVertexes - 1; i >= v; i--)
        {
            if (i + 1 >= MAXVEX)
                exit(0);
            G->adjlist[i + 1] = G->adjlist[i];//������
        }
        G->adjlist[v].data = w;//�����¶���
        G->adjlist[v].firstedge = NULL;
        G->numVertexes++;
    }
    else
    {
        G->adjlist[G->numVertexes].data = w;//�����¶���
        G->adjlist[G->numVertexes].firstedge = NULL;
        G->numVertexes++;
    }
}

//ɾ�����
void DeleteVex(GrapghAdjList* G, int v)
{
    for (int i = 0; i < G->numVertexes; i++)
    {
        if (v != i)//ɾ�����v�����
        {
            EdgeNode* cur = G->adjlist[i].firstedge;
            EdgeNode* pre = cur;
            //ͷ���ָ��v
            if (cur && cur->adjvex == v)
            {
                cur = cur->next;
                G->adjlist[i].firstedge = cur;
                free(pre);
                pre = cur;
                G->numEdges--;
            }
            if (cur)
                cur = cur->next;
            while (cur)
            {
                if (cur->adjvex == v)
                {
                    EdgeNode* temp = cur;
                    pre->next = cur->next;
                    cur = cur->next;
                    free(temp);
                    G->numEdges--;
                    continue;
                }
                if (cur->adjvex > v)
                    cur->adjvex--;
                pre = cur;
                cur = cur->next;
            }
        }
    }
    //�ͷ�v���
    EdgeNode* cur = G->adjlist[v].firstedge;
    while (cur)
    {
        EdgeNode* temp = cur;
        cur = cur->next;
        free(temp);
        G->numEdges--;
    }
    for (int i = v + 1; i < G->numVertexes; i++)
    {
        //���ǰ��
        G->adjlist[i - 1].data = G->adjlist[i].data;
        G->adjlist[i - 1].firstedge = G->adjlist[i].firstedge;
    }
    //�ͷ����һ�����
    cur = G->adjlist[G->numVertexes - 1].firstedge;
    G->adjlist[G->numVertexes - 1].data = ' ';
    cur = NULL;
    free(cur);
    G->numVertexes--;
}

//���뻡/��
void InsertArc(GrapghAdjList* G, int v, int w, EdgeType value)
{
    EdgeNode *e = new EdgeNode;
    e->adjvex = w;
    e->weight = value;
    e->next = G->adjlist[v].firstedge;
    G->adjlist[v].firstedge = e;
    G->numEdges++;
}

//ɾ����/��
void DeleteArc(GrapghAdjList* G, int v, int w)
{
    if (G->adjlist[v].firstedge->adjvex == w)
    {
        EdgeNode* temp = G->adjlist[v].firstedge;
        G->adjlist[v].firstedge = temp->next;
        free(temp);
        G->numEdges--;
        return;
    }
    EdgeNode* cur = G->adjlist[v].firstedge->next;
    EdgeNode* pre = G->adjlist[v].firstedge;
    while(cur)
    {
        if (cur->adjvex == w)
        {
            pre->next = cur->next;
            free(cur);
            G->numEdges--;
            return;
        }
        pre = cur;
        cur = cur->next;
    }

}

//������ȱ���
bool visited[MAXVEX];
void DFS(GrapghAdjList G, int v, void (*visit)(VertexType))
{
    EdgeNode* p;
    visited[v] = true;
    visit(G.adjlist[v].data);
    p = G.adjlist[v].firstedge;
    while (p)
    {
        if (!visited[p->adjvex])
        {
            DFS(G, p->adjvex, visit);//�Է��ʵĽ��ݹ����
        }
        p = p->next;
    }
}
void DFSTraverse(GrapghAdjList G, void (*visit)(VertexType))
{
    int i;
    for (i = 0; i < G.numVertexes; i++)
        visited[i] = false;//��ʼ�����ʱ������
    for (i = 0; i < G.numVertexes; i++)
        if (!visited[i])//δ���ʵĶ���
            DFS(G, i, visit);
}

//������ȱ���
void BFSTraverse(GrapghAdjList G, void (*visit)(VertexType))
{
    EdgeNode* p;
    queue<VertexType> Q;
    for (int i = 0; i < G.numVertexes; i++)
        visited[i] = false;//��ʼ�����ʱ������
    for (int i = 0; i < G.numVertexes; i++)
        if (!visited[i])//δ���ʵĶ���
        {
            visited[i] = true;
            visit(G.adjlist[i].data);//����
            Q.push(i);//���
            while (!Q.empty())
            {
                int j = Q.front();
                Q.pop();//����
                p = G.adjlist[j].firstedge;
                while (p)
                {
                    if (!visited[p->adjvex])
                    {
                        visited[p->adjvex] = true;
                        visit(G.adjlist[p->adjvex].data);
                        Q.push(p->adjvex);
                    }
                    p = p->next;
                }
            }
        }
}

//�����ú���
void Print(VertexType value)
{
    cout << value << " ";
}