#include"Lab6_1.h"
int main()
{
	char s[9][6] = { "Red","Blue","White","White","Blue","Red","Red","Blue","White" };
	CountSort(s, 9);
	for (int i = 0; i < 9; i++)
	{
		cout << s[i] << " ";
	}
}