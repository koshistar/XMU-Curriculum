#include"lab4_2.h"
//a b c d e f g h k
// 0 1
// 0 2
// 0 3
// 0 4
// 0 5
// 0 6
// 1 2
// 1 6
// 2 7
// 3 7
// 4 8
// 5 6
// 5 8
//
int main()
{
	GraphAdjList G;
	CreateAdjList(&G);
	int v1, v2, k;
	cout << "�����u,v,k,�жϴ�u��v�Ƿ���ڳ���Ϊk�ļ�·��\n";
	cin >> v1 >> v2 >> k;
	if (exist_path_len(G, v1, v2, k))
		cout << "����\n";
	else
		cout << "������\n";
	return 0;
}