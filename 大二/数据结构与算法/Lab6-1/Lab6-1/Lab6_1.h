#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;
void CountSort(char s[][6], int n)
{
	int count[3] = { 0 };
	for (int i = 0; i < n; i++)
	{
		switch (s[i][0])
		{
		case 'R':count[0]++; break;
		case 'W':count[1]++; break;
		case 'B':count[2]++; break;
			cout << "There is no right color.\n";
		}
	}
	int j = 0;
	while (count[0]--)
	{
		strcpy(s[j++], "Red");
	}
	while (count[1]--)
	{
		strcpy(s[j++], "White");
	}
	while (count[2]--)
	{
		strcpy(s[j++], "Blue");
	}
}