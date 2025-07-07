#pragma once
#include<iostream>
#include<queue>
#include<cmath>
using namespace std;	
#define OK true
#define ERROR false
#define INFEASIBLE -1
#define OVERFLOW -2
typedef bool Status;
typedef int Type;
typedef struct BiTNode
{
	Type data;  //������Type: �û�������������
	struct BiTNode* Lchild;  //��ָ����
	struct BiTNode* Rchild;  //��ָ����
} BiTNode, * BiTree;
//����ն�����
Status InitBiTree(BiTree &T)
{
	T = NULL;
	return OK;
}
//���ٶ�����
Status DestroyBiTree(BiTree& T)
{
	if (T == NULL)
		return ERROR;
	DestroyBiTree(T->Lchild);
	DestroyBiTree(T->Rchild);
	free(T);
	return OK;
}
//���������� 1-ǰ��2-����3-����
Status CreateBiTree(BiTree& T, int definition = 1)
{
	Type value;
	cin >> value;
	//Type为int的情况，其他情况分别讨论
	if (value == -1)
	{
		T = NULL;
	}
	else
	{
		T = (BiTNode*)malloc(sizeof(BiTNode));
		if (T == NULL)
			return ERROR; 
		if (definition == 1)
		{
			T->data = value;
		}
		CreateBiTree(T->Lchild, definition);
		if (definition == 2)
		{
			T->data = value;
		}
		CreateBiTree(T->Rchild, definition);
		if (definition == 3)
		{
			T->data = value;
		}
	}
	return OK;
}
//��ն�����
Status ClearBiTree(BiTree& T)
{
	if (T == NULL)
	{
		return OK;
	}
	ClearBiTree(T->Lchild);
	ClearBiTree(T->Rchild);
	free(T);
	T = NULL;
	return OK;
}
//�Ƿ�Ϊ��
bool BiTreeEmpty(BiTree T)
{
	return T == NULL;
}
//�������
int BiTreeDepth(BiTree T)
{
	if (T == NULL)
		return 0;
	int leftDepth = BiTreeDepth(T->Lchild);
	int rightDepth = BiTreeDepth(T->Rchild);
	return 1 + max(leftDepth, rightDepth);
}
//���ظ�
Type Root(BiTree T)
{
	return T->data;
}
//����e��ֵ
Type Value(BiTree T, BiTree e)
{
	if (T == NULL || e == NULL)
		exit(0);
	return e->data;
	//���
}
//��ֵ e
Status Assign(BiTree T, BiTree& e, Type value)
{
	if (T == NULL)
		return ERROR;
	e->data = value;
	return OK;
}
//����˫��
BiTree Parent(BiTree T, BiTree e)
{
	BiTree p = NULL;
	if (T == NULL || e == NULL)
		return NULL;
	if (T->Lchild == e || T->Rchild == e)
		return T;
	p = Parent(T->Lchild, e);
	if (p == NULL)
		p = Parent(T->Rchild, e);
	return p;
}
//��������
BiTree LeftChild(BiTree T, BiTree e)
{
	if (T == NULL || e == NULL)
		return NULL;
	return e->Lchild;
}
//�����Һ���
BiTree RightChild(BiTree T, BiTree e)
{
	if (T == NULL || e == NULL)
		return NULL;
	return e->Rchild;
}
//�������ֵ�
BiTree LeftSibling(BiTree T, BiTree e)
{
	BiTree parent = Parent(T, e);
	if (parent->Rchild == e)
		return parent->Lchild;
	return NULL;
}
//�������ֵ�
BiTree RightSibling(BiTree T, BiTree e)
{
	BiTree parent = Parent(T, e);
	if (parent->Lchild == e)
		return parent->Rchild;
	return NULL;
}
//�庢��
Status InsertChild(BiTree& T, BiTree p, int LR, BiTree c)
{
	if (T == NULL || p == NULL || c == NULL)
		return ERROR;
	if (LR == 0)
	{
		c->Rchild = p->Lchild;
		p->Lchild = c;
	}
	if (LR == 1)
	{
		c->Rchild = p->Rchild;
		p->Rchild = c;
	}
	return OK;
}
//ɾ������
Status DeleteChild(BiTree& T, BiTree p, int LR)
{
	if (T == NULL || p == NULL)
		return ERROR;
	if (LR == 0)
	{
		DestroyBiTree(p->Lchild);
		p->Lchild = NULL;
	}
	if (LR == 1)
	{
		DestroyBiTree(p->Rchild);
		p->Rchild = NULL;
	}
	return OK;
}
//�������
Status PreOrderTraverse(BiTree T, Status(*visit)(Type))
{
	if (T == NULL)
		return ERROR;
	if (!visit(T->data))
		return ERROR;
	PreOrderTraverse(T->Lchild, visit);
	PreOrderTraverse(T->Rchild, visit);
	return OK;
}
//�������
Status InOrderTraverse(BiTree T, Status(*visit)(Type))
{
	if (T == NULL)
		return ERROR;
	InOrderTraverse(T->Lchild, visit);
	if (!visit(T->data))
		return ERROR;
	InOrderTraverse(T->Rchild, visit);
	return OK;
}
//�������
Status PostOrderTraverse(BiTree T, Status(*visit)(Type))
{
	if (T == NULL)
		return ERROR;
	PostOrderTraverse(T->Lchild, visit);
	PostOrderTraverse(T->Rchild, visit);
	if (!visit(T->data))
		return ERROR;
	return OK;
}
//�������
Status LevelOrderTraverse(BiTree T, Status(*visit)(Type))
{
	if (T == NULL)
		return ERROR;
	queue<BiTree> q;
	q.push(T);
	while (!q.empty())
	{
		BiTree front = q.front();
		if(!visit(front->data))
			return ERROR;
		q.pop();
		if (front->Lchild != NULL)
			q.push(front->Lchild);
		if (front->Rchild != NULL)
			q.push(front->Rchild);
	}
	return OK;
}
//�����ú���
Status Print(Type t)
{
	cout << t << " ";
	return OK;
}