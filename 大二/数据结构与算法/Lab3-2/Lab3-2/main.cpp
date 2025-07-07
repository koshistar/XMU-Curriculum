#include"Lab3-2.h"
int main()
{
    /*0 4 6 1 0 9 1 2 3 2 0 2 2 3 5 3 4 1*/
    cout << "���붥�����ͱ���:\n";
    int m, n;
    cin >> m >> n;
    cout << "�������붥����Ϣ:\n";
    vector<VertexType> vexs;
    for (int i = 0; i < m; i++)
    {
        VertexType temp;
        cin >> temp;
        vexs.push_back(temp);
    }
    cout << "�����������Ϣ\n";
    cout << "��(vi,vj)���±�i,�±�j,Ȩw:\n";
    map<pair<int, int>, EdgeType, cmp> edges;
    for (int j = 0; j < n; j++)
    {
        int ii, jj;
        EdgeType w;
        cin >> ii >> jj;
        pair<int, int> edge = make_pair(ii, jj);
        cin >> w;
        edges[edge] = w;
    }
    GrapghAdjList G;
    cout << "�����ڽӾ���\n";
    CreateGraph(&G, vexs, edges);
    cout << "b���ڶ���:" << LocateVex(G, 'b') << endl;
    cout << "���3��ֵ:" << GetVex(G, 3) << endl;
    cout << "�����4��ֵΪ'f'\n";
    PutVex(&G, 4, 'f');
    cout << "���1�ĵ�һ���ڽӽ��:" << FirstAdjVex(G, 1) << endl;
    cout << "���1��Խ��0����һ���ڽӽ��:" << NextAdjVex(G, 1, 0) << endl;
    cout << "�ڽ��2�������½��'g'\n";
    InsertVex(&G, 2, 'g');
    cout << "ɾ�����4\n";
    DeleteVex(&G, 4);
    cout << "�����\n";
    InsertArc(&G, 2, 3, 5);
    InsertArc(&G, 3, 4, 1);
    InsertArc(&G, 1, 3, 2);
    InsertArc(&G, 0, 4, 6);
    InsertArc(&G, 0, 1, 5);
    cout << "ɾ����\n";
    DeleteArc(&G, 1, 3);
    cout << "������ȱ���:\n";
    DFSTraverse(G, Print);
    cout << "\n������ȱ���:\n";
    BFSTraverse(G, Print);
    cout << "\n����\n";
    DestroyGraph(&G);
    return 0;
}