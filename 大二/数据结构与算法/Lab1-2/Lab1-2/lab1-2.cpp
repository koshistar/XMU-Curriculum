#include<iostream>
#include<stdlib.h>
#include<assert.h>
using namespace std;
typedef struct Node
{
	Node* before;
	Node* next;
	char ch;
}List;
void Destroy(List* list)
{
	/*list->before->next = NULL;
	List* tmp;
	while (list)
	{
		tmp = list;
		list = list->next;
		free(tmp);
	}*/
	list->before->next = list->next;
	list->next->before = list->before;
	free(list);
}
int main()
{
	List* list = new List;
	list->before = list;
	list->next = list;
	List* pre, * cur;
	pre = list;
	cur = NULL;
	int cnt = 0;
	cout << "������80λ���ڳ��ȵ��ַ���(����'#'�˳�):";
	while (1)
	{
		char c;
		cin.get(c);
		if (c == '#')
			break;
		if (c != '\n')
		{
			cur = new List;
			pre->ch = c;
			cur->ch = '\0';
			pre->next = cur;
			cur->before = pre;
			cur->next = cur;
			pre = cur;
			cnt++;
			if (cnt > 80)
			{
				cout << "��������ַ������ȴ���80";
				exit(0);
			}
		}
		else
		{
			pre = list;
			cur = cur->before;
			bool flag = true;
			while (pre->next != cur->before && pre->before != cur)
			{
				if (pre->ch != cur->ch)
				{
					cout << "Wrong.\n";
					flag = false;
					break;
				}
				pre = pre->next;
				cur = cur->before;
			}
			if (flag)
				cout << "True.\n";
			Destroy(list);
			list = new List;
			list->before = list;
			list->next = list;
			cur = NULL;
			pre = list;
			cout << "������80λ���ڳ��ȵ��ַ���(����'#'�˳�):";
		}
	}
	return 0;
}