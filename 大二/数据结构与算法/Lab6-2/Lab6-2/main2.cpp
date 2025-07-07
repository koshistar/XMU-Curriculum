#include"Lab6_2.h"
int main()
{
	char name[10][25];
	Names(name, 10);
	NameSort(name, 10);
	for (int i = 0; i < 10; ++i)
	{
		cout << name[i] << " ";
	}
	return 0;
}