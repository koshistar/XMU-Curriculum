#include"lab3_1.h"
int main()
{
    cout << "输入顶点数和边数:\n";
    int m, n;
    cin >> m >> n;
    cout << "依次输入顶点信息:\n";
    vector<VertexType> vexs;
    for (int i = 0; i < m; i++)
    {
        VertexType temp;
        cin >> temp;
        vexs.push_back(temp);
    }
    cout << "依次输入边信息\n";
    cout << "边(vi,vj)的下标i,下标j,权w:\n";
    map<pair<int, int>, EdgeType> edges;
    for (int j = 0; j < n; j++)
    {
        int ii, jj;
        EdgeType w;
        cin >> ii >> jj;
        pair<int, int> edge = make_pair(ii, jj);
        cin >> w;
        edges[edge] = w;
    }
    MGraph G;
    cout << "创建邻接矩阵\n";
    CreateGraph(&G, vexs, edges);
    cout << "b所在顶点:" << LocateVex(G, 'b') << endl;
    cout << "结点3的值:" << GetVex(G, 3) << endl;
    cout << "将结点4赋值为'f'\n";
    PutVex(&G, 4, 'f');
    cout << "结点0的第一个邻接结点:" << FirstAdjVex(G, 0) << endl;
    cout << "结点0相对结点1的下一个邻接结点:" << NextAdjVex(G, 0, 1) << endl;
    cout << "在结点3处增加新结点'g'\n";
    InsertVex(&G, 3, 'g');
    cout << "删除结点4\n";
    DeleteVex(&G, 4);
    cout << "插入边\n";
    InsertArc(&G, 2, 3, 5);
    InsertArc(&G, 3, 4, 1);
    InsertArc(&G, 1, 3, 2);
    cout << "删除边\n";
    DeleteArc(&G, 1, 3);
    cout << "深度优先遍历:\n";
    DFSTraverse(G, Print);
    cout << "\n广度优先遍历:\n";
    BFSTraverse(G, Print);
    cout << "\n销毁\n";
    DestroyGraph(&G);
    return 0;
}