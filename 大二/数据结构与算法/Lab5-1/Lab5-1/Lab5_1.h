#pragma once
#include<iostream>
using namespace std;
typedef int Type;
int BiSearch(Type L[], Type key, int n)
{
	int left = 1, right = n;
	while (left < right)
	{
		int mid = (left + right) >> 1;
		if (key == L[mid])
			return mid;
		if (key < L[mid])
			right = mid - 1;
		else
			left = mid + 1;
	}
	return -1;
}