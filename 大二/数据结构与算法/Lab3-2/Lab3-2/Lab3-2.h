#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<queue>
using namespace std;
typedef char VertexType;
typedef int EdgeType;
#define MAXVEX 10

//自定义排序
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

//创建邻接表
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
        ////确保排列顺序
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
        //G->adjlist[vr.first.second].firstedge = e;//双向图
    }
    //排序
    //for (int i = 0; i < G->numVertexes; i++)
    //{
    //    EdgeNode* cur, * pre, * p;
    //    cur = G->adjlist[i].firstedge ? G->adjlist[i].firstedge->next : NULL;
    //    p = pre = G->adjlist[i].firstedge;
    //    while (cur != NULL)
    //    {
    //        //确认头结点
    //        if (cur->adjvex < pre->adjvex)
    //        {
    //            G->adjlist[i].firstedge = cur;
    //        }
    //    }
    //}
}

//销毁邻接表
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

//返回顶点位置
int LocateVex(GrapghAdjList G, VertexType u)
{
    for (int i = 0; i < G.numVertexes; i++)
    {
        if (G.adjlist[i].data == u)
            return i;
    }
    return -1;//无顶点返回-1
}

//返回v的值
VertexType GetVex(GrapghAdjList G, int v)
{
    return G.adjlist[v].data;
}

//对v赋值
void PutVex(GrapghAdjList* G, int v, VertexType value)
{
    for (int i = 0; i < G->numVertexes; i++)
    {
        //先将v的入度修改
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

//返回v的第一个邻接顶点
VertexType FirstAdjVex(GrapghAdjList G, int v)
{
    if (G.adjlist[v].firstedge)
        return G.adjlist[G.adjlist[v].firstedge->adjvex].data;
    return -1;//无邻接结点返回-1
}

//返回v相对于w的下一个邻接顶点
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
    return -1;//无下一个邻接顶点返回-1
}

//添加新顶点
void InsertVex(GrapghAdjList* G, int v, VertexType w)
{
    if (v < G->numVertexes)
    {
        for(int i=0;i<G->numVertexes;i++)
        {
            EdgeNode* temp = G->adjlist[i].firstedge;
            while (temp)
            {
                if (temp->adjvex > v)//将v的入度后移
                    temp->adjvex++;
                temp = temp->next;
            }
        }
        for (int i = G->numVertexes - 1; i >= v; i--)
        {
            if (i + 1 >= MAXVEX)
                exit(0);
            G->adjlist[i + 1] = G->adjlist[i];//结点后移
        }
        G->adjlist[v].data = w;//插入新顶点
        G->adjlist[v].firstedge = NULL;
        G->numVertexes++;
    }
    else
    {
        G->adjlist[G->numVertexes].data = w;//插入新顶点
        G->adjlist[G->numVertexes].firstedge = NULL;
        G->numVertexes++;
    }
}

//删除结点
void DeleteVex(GrapghAdjList* G, int v)
{
    for (int i = 0; i < G->numVertexes; i++)
    {
        if (v != i)//删除结点v的入度
        {
            EdgeNode* cur = G->adjlist[i].firstedge;
            EdgeNode* pre = cur;
            //头结点指向v
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
    //释放v结点
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
        //结点前移
        G->adjlist[i - 1].data = G->adjlist[i].data;
        G->adjlist[i - 1].firstedge = G->adjlist[i].firstedge;
    }
    //释放最后一个结点
    cur = G->adjlist[G->numVertexes - 1].firstedge;
    G->adjlist[G->numVertexes - 1].data = ' ';
    cur = NULL;
    free(cur);
    G->numVertexes--;
}

//插入弧/边
void InsertArc(GrapghAdjList* G, int v, int w, EdgeType value)
{
    EdgeNode *e = new EdgeNode;
    e->adjvex = w;
    e->weight = value;
    e->next = G->adjlist[v].firstedge;
    G->adjlist[v].firstedge = e;
    G->numEdges++;
}

//删除弧/边
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

//深度优先遍历
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
            DFS(G, p->adjvex, visit);//对访问的结点递归调用
        }
        p = p->next;
    }
}
void DFSTraverse(GrapghAdjList G, void (*visit)(VertexType))
{
    int i;
    for (i = 0; i < G.numVertexes; i++)
        visited[i] = false;//初始化访问标记数组
    for (i = 0; i < G.numVertexes; i++)
        if (!visited[i])//未访问的顶点
            DFS(G, i, visit);
}

//广度优先遍历
void BFSTraverse(GrapghAdjList G, void (*visit)(VertexType))
{
    EdgeNode* p;
    queue<VertexType> Q;
    for (int i = 0; i < G.numVertexes; i++)
        visited[i] = false;//初始化访问标记数组
    for (int i = 0; i < G.numVertexes; i++)
        if (!visited[i])//未访问的顶点
        {
            visited[i] = true;
            visit(G.adjlist[i].data);//访问
            Q.push(i);//入队
            while (!Q.empty())
            {
                int j = Q.front();
                Q.pop();//出队
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

//测试用函数
void Print(VertexType value)
{
    cout << value << " ";
}