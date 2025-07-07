#pragma once
#include<iostream>
using namespace std;
typedef int KeyType;
typedef struct BiTNode
{
	KeyType key;
	//ElemType *otherinfo 其他数据
	struct BiTNode* lChild, * rChild;
}BiTNode, * BiTree;
//查找
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
//插入
bool InsertT(BiTree& T, KeyType key)
{
	BiTree p = T;
	if (SearchT(T, key, p))
		return false;//如果已有该元素，返回false
	//否则p指向末端
	BiTree s = new BiTNode;
	s->key = key;
	s->lChild = s->rChild = NULL;
	if (!p)//T是空树
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
//删除
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
	else//当两个子树均不为空时，删除结点的位置变为其前驱
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
		else//p的左子树第一个借点即为其前驱
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

//遍历,用于观察
void inOrderRecur(BiTree root)//中序遍历
{
	if (root == NULL)
		return;
	inOrderRecur(root->lChild);
	cout << root->key << " ";
	inOrderRecur(root->rChild);
}