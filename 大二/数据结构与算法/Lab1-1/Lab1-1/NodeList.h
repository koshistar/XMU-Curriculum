#pragma once
#include<iostream>
#include<stdlib.h>
#define Type int
#define Status bool
#define OK 1
#define ERROR 0
class List {
private:
	typedef struct Node
	{
		Type data;
		struct Node* next;
	}*Link, * Position;
	typedef struct
	{
		Link head, tail;
		int len;
	}LinkList;
	//����
	static Status MakeNode(Link& p, Type e)
	{
		if (p == NULL)
			return ERROR;
		p->data = e;
		return OK;
	}
	//�ͷ�
	static void FreeNode(Link& p)
	{
		if (p == NULL)
		{
			std::cout << "ERROR!\n";
			return;
		}
		free(p->next);
		p = NULL;
	}
	//����������
	static void InitList(Link& L)
	{
		L->data = 0;
		L->next = NULL;
	}
	//����
	static void DestroyList(Link& L)
	{
		Link p;
		while (L)
		{
			p = L;
			L = L->next;
			delete p;
		}
	}
	//���
	static void ClearList(Link& L)
	{
		if (L == NULL)
			return;
		Link p, q;
		p = L->next;
		while (p)
		{
			q = p->next;
			free(p);
			p = q;
		}
		L->next = NULL;
	}
	//ͷ����
	static Status InsFirst(Link h, Link s)
	{
		if (h == NULL || s == NULL)
			return ERROR;
		Link q = s;
		while (q->next)
			q = q->next;
		q->next = h->next;
		h->next = s;
		return OK;
	}
	//ͷɾ��
	static Status DelFirst(Link h, Link& q)
	{
		if (h == NULL)
			return ERROR;
		q = h->next;
		h->next = q->next;
		q->next = NULL;
		return OK;
	}
	//β����
	static Status Append(LinkList& L, Link s)
	{
		if (s == NULL || L.len <= 0)
			return ERROR;
		L.tail->next->next = s;
		L.tail->next = s;
		L.len++;
		return OK;
	}
	//βɾ��
	static Status Remove(LinkList& L, Link& q)
	{
		if (L.len <= 0)
			return ERROR;
		q = L.tail->next;
		Link p = L.head;
		while (p->next != q)
		{
			p = p->next;
		}
		p->next = q->next;
		L.tail->next = p;
		q->next = NULL;
		L.len--;
		return OK;
	}
	//ǰ��
	static Status InsBefore(LinkList& L, Link& q, Link s)
	{
		if (L.len <= 0 || q == NULL)
			return ERROR;
		Link p = L.head;
		while (p->next != q->next)
		{
			p = p->next;
		}
		p->next = s;
		s->next = q->next;
		q->next = s;
		L.len++;
		return OK;
	}
	//���
	static Status InsAfter(LinkList& L, Link& q, Link s)
	{
		if (L.len <= 0 || q == NULL)
			return ERROR;
		s->next = q->next->next;
		q->next->next = s;
		q->next = s;
		L.len++;
		return OK;
	}
	//����ֵ
	static Status SetCurElem(Link& p, Type e)
	{
		if (p == NULL)
			return ERROR;
		p->next->data = e;
		return OK;
	}
	//����ֵ
	static Type GetCurElem(Link p)
	{
		if (p == NULL || p->next->data == NULL)
			exit(0);
		return p->next->data;
	}
	//�Ƿ������
	static Status ListEmpty(LinkList L)
	{
		if (L.len <= 0)
			return ERROR;
		return OK;
	}
	//Ԫ�ظ���
	static int ListLength(LinkList L)
	{
		return L.len;
	}
	//ͷ���λ��
	static Position GetHead(LinkList L)
	{
		return L.head->next;
	}
	//β�ڵ�λ��
	static Position GetLast(LinkList L)
	{
		return L.tail->next;
	}
	//����ǰ��λ��
	static Position PriorPos(LinkList L, Link p)
	{
		if (L.len <= 0 || p == NULL)
			return NULL;
		Link q = L.head;
		while (q->next != p->next && q != NULL)
		{
			q = q->next;
		}
		return q;
	}
	//���غ��λ��
	static Position NextPos(LinkList L, Link p)
	{
		if (L.len <= 0 || p == NULL || p->next == NULL)
			return NULL;
		return p->next->next;
	}
	//���ؽ��
	static Status LocatePos(LinkList L, int i, Link& p)
	{
		if (L.len <= 0 || i < 1)
			return ERROR;
		Link q = L.head;
		while (i--)
		{
			q = q->next;
		}
		p->next = q;
		return OK;
	}
	//��һ����e�����ϵ��λ��
	static Position LocateElem(LinkList L, Type e, Status(*Compare)(Type, Type))
	{
		if (L.len <= 0)
			return NULL;
		Link p = L.head->next;
		while (p)
		{
			if (Compare(p->data, e))
				return p;
		}
		return NULL;
	}
	bool Compare(Type e1, Type e2)
	{

	}
	//���δ���
	static Status ListTraverse(LinkList L, Status(*visit)(Type))
	{
		if (L.len <= 0)
			return ERROR;
		Link p = L.head->next;
		while (p)
		{
			if (!visit(p->data))
				return ERROR;
		}
		return OK;
	}
	Status visit(Type e)
	{

	}
	//��ʾ
	static void Show(Link p)
	{
		p = p->next;
		while (p)
		{
			std::cout << p->data << "  ";
			p = p->next;
		}
		std::cout << std::endl;
	}
public:
	static void Test()
	{
		using namespace std;
		Link p = (Link)malloc(sizeof(Link));
		Link s = (Link)malloc(sizeof(Link));
		Link q = (Link)malloc(sizeof(Link));
		InitList(s);
		s->data = 1;
		InitList(p);
		cout << "ͷ��\n";
		if (InsFirst(p, s))
			Show(p);
		InitList(q);
		MakeNode(q, 2);
		if (InsFirst(p, q))
			Show(p);
		cout << "ͷɾ\n";
		if (DelFirst(p, q))
		{
			Show(p);
			Show(q);
		}
		cout << "��ͷ��㵥����\n";
		LinkList* L = (LinkList*)malloc(sizeof(LinkList*));
		L->head = (Link)malloc(sizeof(Link));
		L->tail = (Link)malloc(sizeof(Link));
		L->head->next = p;
		L->tail->next = s;
		L->len = 2;
		cout << "β��\n";
		if (Append(*L, q))
			Show(L->head);
		cout << "βɾ\n";
		if (Remove(*L, q))
			Show(L->head);
		cout << "ǰ��\n";
		Link t = (Link)malloc(sizeof(Link));
		InitList(t);
		t->next = L->head->next;
		Link u = (Link)malloc(sizeof(Link));
		InitList(u);
		MakeNode(u, 3);
		if (InsBefore(*L, t, u))
			Show(L->head);
		cout << "���\n";
		Link v = (Link)malloc(sizeof(Link));
		InitList(v);
		MakeNode(v, 4);
		if (InsAfter(*L, t, v))
			Show(L->head);
		cout << "����ֵ\n";
		if (SetCurElem(t, 5))
			Show(L->head);
		cout << "����ֵ\n";
		cout << GetCurElem(t) << endl;
		Link temp = (Link)malloc(sizeof(Link));
		InitList(temp);
		FreeNode(temp);
		ClearList(temp);
		DestroyList(temp);
	}
};