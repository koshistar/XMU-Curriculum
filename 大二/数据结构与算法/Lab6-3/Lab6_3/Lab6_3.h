#pragma once
#include<iostream>
#include<cmath>
using namespace std;
typedef int Type;
typedef struct Node
{
	Type key[100];
	int length;
}SqList;
void TreeSort(SqList* L)
{
	int height = (int)ceil(log(L->length) / log(2)) + 1;//树的高度
	int full = pow(2, height) - 1;//结点总数
	int prefull = pow(2, height - 1) - 1;//除去叶子结点的结点数
	Type* t = new Type(full + 1);
	for (int i = 1; i <= prefull; i++)
	{
		t[i] = INT_MAX;//前面结点
	}
	for (int i = 0; i < L->length; i++)
	{
		t[prefull + i + 1] = L->key[i];
	}
	for (int i = prefull + L->length + 1; i <= full; i++)
	{
		t[i] = INT_MAX;//多余结点
	}
	//为非叶子结点赋值
	for (int i = prefull; i >= 1; i--)
	{
		t[i] = min(t[2 * i], t[2 * i + 1]);
	}
	for (int i = 0; i < L->length; i++)
	{
		L->key[i] = t[1];
		int j = 1;
		while (t[2 * j] == t[1] || t[2 * j + 1] == t[1])
		{
			j *= 2;
			if (t[j] != t[1])
			{
				j++;
			}
			if (2 * j >= full)
			{
				break;
			}
		}
		t[j] = INT_MAX;
		for (int k = j / 2; k >= 1; k /= 2)
		{
			t[k] = min(t[2 * k], t[2 * k + 1]);
		}
	}
}