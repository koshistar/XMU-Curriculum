#include"Lab5_1.h"
#define MAXSIZE 100
int main()
{
	Type L[MAXSIZE] = { 0 };
	int n;
	cout << "输入数组长度:\n";
	cin >> n;
	L[0] = n;
	cout << "依次输入数组元素:(从小到大排序)\n";
	for (int i = 1; i <= n; i++)
	{
		cin >> L[i];
	}
	cout << "输入要查找的值:\n";
	Type key;
	cin >> key;
	int res = BiSearch(L, key, L[0]);
	if (res != -1)
		cout << "\n找到该值，其下标为" << res << endl;
	else
		cout << "\n未找到\n";
	return 0;
}