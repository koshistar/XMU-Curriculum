#include"Lab5_2.h"
int main()
{
	BiTree T = NULL;
	int n;
	cout << "��������Ŀ:\n";
	cin >> n;
	cout << "��������ÿ�����:\n";
	while (n--)
	{
		KeyType key;
		cin >> key;
		InsertT(T, key);
	}
	cout << "����������:\n";
	inOrderRecur(T);
	cout << "\n��Ҫ���ҽ���Ԫ��ֵ:\n";
	KeyType key;
	cin >> key;
	BiTree p = new BiTNode;
	if (SearchT(T, key, p))
	{
		cout << "���ҵ�\nɾ����Ԫ��\n";
		if (DeleteT(&T, key))
		{
			inOrderRecur(T);
		}
		else
		{
			cout << "���·���";
		}
	}
	else
		cout << "δ�ҵ�\n";
	return 0;
}