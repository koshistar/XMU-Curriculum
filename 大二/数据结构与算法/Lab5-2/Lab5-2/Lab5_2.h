#pragma once
#include<iostream>
using namespace std;
typedef int KeyType;
typedef struct BiTNode
{
	KeyType key;
	//ElemType *otherinfo ��������
	struct BiTNode* lChild, * rChild;
}BiTNode, * BiTree;
//����
BiTree SearchT(BiTree T, KeyType key, BiTree& p)
{
	if (!T)
		return NULL;
	if (key == T->key)
	{
		return T;
	}
	p = T;
	if (key < T->key)
		return SearchT(T->lChild, key, p);
	else
		return SearchT(T->rChild, key, p);
}
//����
bool InsertT(BiTree& T, KeyType key)
{
	BiTree p = T;
	if (SearchT(T, key, p))
		return false;//������и�Ԫ�أ�����false
	//����pָ��ĩ��
	BiTree s = new BiTNode;
	s->key = key;
	s->lChild = s->rChild = NULL;
	if (!p)//T�ǿ���
	{
		T = s;
	}
	else if (key < p->key)
	{
		p->lChild = s;
	}
	else
	{
		p->rChild = s;
	}
	return true;
}
//ɾ��
void Delete(BiTree* p)
{
	BiTree q;
	if ((*p)->rChild == NULL)
	{
		q = (*p);
		(*p) = (*p)->lChild;
		free(q);
	}
	else if ((*p)->lChild == NULL)
	{
		q = (*p);
		(*p) = (*p)->rChild;
		free(q);
	}
	else//��������������Ϊ��ʱ��ɾ������λ�ñ�Ϊ��ǰ��
	{
		q = (*p);
		BiTree s = (*p)->lChild;
		while (s->rChild)
		{
			q = s;
			s = s->rChild;
		}
		(*p)->key = s->key;
		if (q != (*p))
		{
			q->rChild = s->lChild;
		}
		else//p����������һ����㼴Ϊ��ǰ��
		{
			q->lChild = s->lChild;
		}
		free(s);
	}
}
bool DeleteT(BiTree* T, KeyType key)
{
	if (!*T)
		return false;
	else
	{
		if (key == (*T)->key)
		{
			Delete(T);
			return true;
		}
		else if (key < (*T)->key)
			return DeleteT(&(*T)->lChild, key);
		else
			return DeleteT(&(*T)->rChild, key);
	}
}

//����,���ڹ۲�
void inOrderRecur(BiTree root)//�������
{
	if (root == NULL)
		return;
	inOrderRecur(root->lChild);
	cout << root->key << " ";
	inOrderRecur(root->rChild);
}