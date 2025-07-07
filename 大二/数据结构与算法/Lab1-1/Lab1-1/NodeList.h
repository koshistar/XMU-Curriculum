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
	//分配
	static Status MakeNode(Link& p, Type e)
	{
		if (p == NULL)
			return ERROR;
		p->data = e;
		return OK;
	}
	//释放
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
	//构建空链表
	static void InitList(Link& L)
	{
		L->data = 0;
		L->next = NULL;
	}
	//销毁
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
	//清空
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
	//头插入
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
	//头删除
	static Status DelFirst(Link h, Link& q)
	{
		if (h == NULL)
			return ERROR;
		q = h->next;
		h->next = q->next;
		q->next = NULL;
		return OK;
	}
	//尾插入
	static Status Append(LinkList& L, Link s)
	{
		if (s == NULL || L.len <= 0)
			return ERROR;
		L.tail->next->next = s;
		L.tail->next = s;
		L.len++;
		return OK;
	}
	//尾删除
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
	//前插
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
	//后插
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
	//更新值
	static Status SetCurElem(Link& p, Type e)
	{
		if (p == NULL)
			return ERROR;
		p->next->data = e;
		return OK;
	}
	//返回值
	static Type GetCurElem(Link p)
	{
		if (p == NULL || p->next->data == NULL)
			exit(0);
		return p->next->data;
	}
	//是否空链表
	static Status ListEmpty(LinkList L)
	{
		if (L.len <= 0)
			return ERROR;
		return OK;
	}
	//元素个数
	static int ListLength(LinkList L)
	{
		return L.len;
	}
	//头结点位置
	static Position GetHead(LinkList L)
	{
		return L.head->next;
	}
	//尾节点位置
	static Position GetLast(LinkList L)
	{
		return L.tail->next;
	}
	//返回前驱位置
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
	//返回后继位置
	static Position NextPos(LinkList L, Link p)
	{
		if (L.len <= 0 || p == NULL || p->next == NULL)
			return NULL;
		return p->next->next;
	}
	//返回结点
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
	//第一个与e满足关系的位序
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
	//依次处理
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
	//显示
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
		cout << "头插\n";
		if (InsFirst(p, s))
			Show(p);
		InitList(q);
		MakeNode(q, 2);
		if (InsFirst(p, q))
			Show(p);
		cout << "头删\n";
		if (DelFirst(p, q))
		{
			Show(p);
			Show(q);
		}
		cout << "带头结点单链表\n";
		LinkList* L = (LinkList*)malloc(sizeof(LinkList*));
		L->head = (Link)malloc(sizeof(Link));
		L->tail = (Link)malloc(sizeof(Link));
		L->head->next = p;
		L->tail->next = s;
		L->len = 2;
		cout << "尾插\n";
		if (Append(*L, q))
			Show(L->head);
		cout << "尾删\n";
		if (Remove(*L, q))
			Show(L->head);
		cout << "前插\n";
		Link t = (Link)malloc(sizeof(Link));
		InitList(t);
		t->next = L->head->next;
		Link u = (Link)malloc(sizeof(Link));
		InitList(u);
		MakeNode(u, 3);
		if (InsBefore(*L, t, u))
			Show(L->head);
		cout << "后插\n";
		Link v = (Link)malloc(sizeof(Link));
		InitList(v);
		MakeNode(v, 4);
		if (InsAfter(*L, t, v))
			Show(L->head);
		cout << "更新值\n";
		if (SetCurElem(t, 5))
			Show(L->head);
		cout << "返回值\n";
		cout << GetCurElem(t) << endl;
		Link temp = (Link)malloc(sizeof(Link));
		InitList(temp);
		FreeNode(temp);
		ClearList(temp);
		DestroyList(temp);
	}
};