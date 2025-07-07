#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
void Names(char A[][25], int n)
{
	srand(time(0));
	int i, j, k;
	for (i = 0; i < n; i++)
	{
		k = 2 * (rand() % 10 + 3);	//部门字数
		for (j = 0; j < k; j++)
			A[i][j] = rand() % 30 + 176; //汉字区
		A[i][j] = '\0';
	}
}
void NameSort(char name[][25], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = n - 1; j > i; j--)
        {
            if (strcmp(name[j - 1], name[j]) > 0)
            {
                char temp[31] = { 0 };
                strcpy(temp, name[j - 1]);
                strcpy(name[j - 1], name[j]);
                strcpy(name[j], temp);
            }
        }
    }
}