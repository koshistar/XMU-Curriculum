#include"Lab5_1.h"
#define MAXSIZE 100
int main()
{
	Type L[MAXSIZE] = { 0 };
	int n;
	cout << "�������鳤��:\n";
	cin >> n;
	L[0] = n;
	cout << "������������Ԫ��:(��С��������)\n";
	for (int i = 1; i <= n; i++)
	{
		cin >> L[i];
	}
	cout << "����Ҫ���ҵ�ֵ:\n";
	Type key;
	cin >> key;
	int res = BiSearch(L, key, L[0]);
	if (res != -1)
		cout << "\n�ҵ���ֵ�����±�Ϊ" << res << endl;
	else
		cout << "\nδ�ҵ�\n";
	return 0;
}