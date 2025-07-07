#include"Lab6_3.h"
#include<time.h>
int main()
{
	SqList L;
	srand(time(0));
	for (int i = 0; i < 10; i++)
	{
		L.key[i] = rand() % 100;
		cout << L.key[i] << " ";
	}
	cout << endl;
	L.length = 10;
	TreeSort(&L);
	for (int i = 0; i < 10; i++)
	{
		cout << L.key[i] << " ";
	}
	return 0;
}