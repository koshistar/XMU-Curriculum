#include"Lab5-3.h"
int main()
{
	initNameList();
	CreateHashTable();
	cout << "NameList:\n";
	PrintNameList();
	cout << "HashTable:\n";
	PrintHashTable();
	if (Search("ma"))
		cout << "\nFind";
	return 0;
}