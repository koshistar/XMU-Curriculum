#include"lab2-1.h"
int main()
{
	BiTree T;
	if (!InitBiTree(T))
		cout << "ERROR!"<<endl;
	cout << "Create\n";
	if (!CreateBiTree(T,1))
		cout << "ERROR!"<<endl;
	cout << "PreOrderTraverse\n";
	if (!PreOrderTraverse(T, Print))
		cout << "ERROR!" << endl;
	cout << endl << "Destroy\n";
	if (!DestroyBiTree(T))
		cout << "ERROR!"<<endl;
	cout << "create\n";
	if (!CreateBiTree(T))
		cout << "ERROR!"<<endl;
	cout << "InOrderTraverse\n";
	if (!InOrderTraverse(T, Print))
		cout << "ERROR!" << endl;
	cout << "\ndepth\n";
	cout << BiTreeDepth(T) << endl;
	BiTree e = T->Lchild;
	cout << "leftchild's value\n";
	cout << Value(T,e) << endl;
	BiTree c;
	cout << "create\n";
	if (!CreateBiTree(c))
		cout << "ERROR!";
	cout << "insert\n";
	if (!InsertChild(T,e,0,c))
		cout << "ERROR!";
	cout << "PostOrderTraverse\n";
	if (!PostOrderTraverse(T, Print))
		cout << "ERROR" << endl;
	cout << "\ndelete\n";
	if (!DeleteChild(T, e, 1))
		cout << "ERROR!";
	cout << "PostOrderTraverse\n";
	if (!PostOrderTraverse(T, Print))
		cout << "ERROR" << endl;
	cout << endl;
	return 0;
}