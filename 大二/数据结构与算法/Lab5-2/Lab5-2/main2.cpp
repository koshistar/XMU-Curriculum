#include"Lab5_2.h"
int main()
{
	BiTree T = NULL;
	int n;
	cout << "输入结点数目:\n";
	cin >> n;
	cout << "依次输入每个结点:\n";
	while (n--)
	{
		KeyType key;
		cin >> key;
		InsertT(T, key);
	}
	cout << "二叉排序树:\n";
	inOrderRecur(T);
	cout << "\n需要查找结点的元素值:\n";
	KeyType key;
	cin >> key;
	BiTree p = new BiTNode;
	if (SearchT(T, key, p))
	{
		cout << "已找到\n删除该元素\n";
		if (DeleteT(&T, key))
		{
			inOrderRecur(T);
		}
		else
		{
			cout << "无事发生";
		}
	}
	else
		cout << "未找到\n";
	return 0;
}