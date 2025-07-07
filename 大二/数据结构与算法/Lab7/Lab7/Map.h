#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include"graph.h"
#include<graphics.h>
#include<mmsystem.h>
#include<conio.h>
#include<wchar.h>
#include<Windows.h>
#include<tchar.h>
#include<vector>
#pragma comment(lib,"winmm.lib")
MGraph T;
int v0 = -1;
int v = -1;
bool isMulti = false;
wchar_t pointName[] = L"目标节点：";
wchar_t pointIntroduce[] = L"景点介绍：";
wchar_t curPoint[] = L"当前节点：";
//检测鼠标显示范围
bool PointInRect(int x,int y,RECT &r)
{
	return (r.left <= x && x <= r.right && r.top <= y && y <= r.bottom);
}
//转换字符串
//TCHAR* Transform(string s)
//{
//	TCHAR result[1024];
//	char c[1024];
//	strcpy_s(c, s.c_str());
//	MultiByteToWideChar(CP_ACP, 0, c, -1, result, 1024);
//	return result;
//}
// 结点转化成名称
LPCTSTR GetName(int i)
{
	switch (i)
	{
	case 0:
		return _T("东门");
	case 1:
		return _T("一期运动场");
	case 2:
		return _T("学生活动中心");
	case 3:
		return _T("学生广场");
	case 4:
		return _T("德旺图书馆");
	case 5:
		return _T("丰庭餐厅");
	case 6:
		return _T("二期运动场");
	case 7:
		return _T("思源餐厅");
	case 8:
		return _T("爱秋体育馆");
	case 9:
		return _T("新工科研发大楼");
	}
}

//路径
class Edge
{
private:
	IMAGE& image;
	RECT rect;
public:
	Edge(IMAGE& img) :image(img)
	{
		rect.left = 0;
		rect.right = rect.left + image.getwidth();
		rect.top = 0;
		rect.bottom = rect.top + image.getheight();
	}
	void Show()
	{
		rect.left = 0;
		rect.right = rect.right = rect.left + image.getwidth();
		putimage(rect.left, rect.top, &image, SRCAND);
	}
	void Hide()
	{
		rect.left = 1394;
		rect.right = rect.left + image.getwidth();
	}
};
vector<IMAGE> edges(15);
vector<Edge*> edgepoints;
void InitEdge()
{
	loadimage(&edges[0], _T("graphics\\edge1.png"));
	edgepoints.push_back(new Edge(edges[0]));
	loadimage(&edges[1], _T("graphics\\edge2.png"));
	edgepoints.push_back(new Edge(edges[1]));
	loadimage(&edges[2], _T("graphics\\edge3.png"));
	edgepoints.push_back(new Edge(edges[2]));
	loadimage(&edges[3], _T("graphics\\edge4.png"));
	edgepoints.push_back(new Edge(edges[3]));
	loadimage(&edges[4], _T("graphics\\edge5.png"));
	edgepoints.push_back(new Edge(edges[4]));
	loadimage(&edges[5], _T("graphics\\edge6.png"));
	edgepoints.push_back(new Edge(edges[5]));
	loadimage(&edges[6], _T("graphics\\edge7.png"));
	edgepoints.push_back(new Edge(edges[6]));
	loadimage(&edges[7], _T("graphics\\edge8.png"));
	edgepoints.push_back(new Edge(edges[7]));
	loadimage(&edges[8], _T("graphics\\edge9.png"));
	edgepoints.push_back(new Edge(edges[8]));
	loadimage(&edges[9], _T("graphics\\edge10.png"));
	edgepoints.push_back(new Edge(edges[9]));
	loadimage(&edges[10], _T("graphics\\edge11.png"));
	edgepoints.push_back(new Edge(edges[10]));
	loadimage(&edges[11], _T("graphics\\edge12.png"));
	edgepoints.push_back(new Edge(edges[11]));
	loadimage(&edges[12], _T("graphics\\edge13.png"));
	edgepoints.push_back(new Edge(edges[12]));
	loadimage(&edges[13], _T("graphics\\edge14.png"));
	edgepoints.push_back(new Edge(edges[13]));
	loadimage(&edges[14], _T("graphics\\edge15.png"));
	edgepoints.push_back(new Edge(edges[14]));
}
IMAGE img;
RECT point0, point1, point2, point3, point4, point5, point6, point7, point8, point9, exitpoint, multipoint;
wchar_t s1[] = L"二期操场";
wchar_t s2[] = L"思源餐厅";
wchar_t s3[] = L"爱秋体育馆";
wchar_t s4[] = L"新工科大楼";
wchar_t s5[] = L"丰庭餐厅";
wchar_t s6[] = L"德旺图书馆";
wchar_t s7[] = L"学生活动中心";
wchar_t s8[] = L"一期操场";
wchar_t s9[] = L"东门";
wchar_t s10[] = L"小巨蛋学生广场";
wchar_t exitText[] = L"退出";
wchar_t multi[] = L"多选";
//获取路径
void ShowPath(int i, int j)
{
	if (i == 0)
	{
		if (j == 1)
		{
			edgepoints[0]->Show();
		}
		if (j == 2)
		{
			edgepoints[1]->Show();
		}
		if (j == 3)
		{
			edgepoints[2]->Show();
		}
		if (j == 4)
		{
			edgepoints[3]->Show();
		}
	}
	if (i == 1 && j == 2)
	{
		edgepoints[4]->Show();
	}
	if (i == 2 && j == 3)
	{
		edgepoints[5]->Show();
	}
	if (i == 2 && j == 8)
	{
		edgepoints[6]->Show();
	}
	if (i == 3 && j == 4)
	{
		edgepoints[7]->Show();
	}
	if (i == 3 && j == 8)
	{
		edgepoints[8]->Show();
	}
	if (i == 4 && j == 5)
	{
		edgepoints[9]->Show();
	}
	if (i == 5)
	{
		if (j == 6)
		{
			edgepoints[10]->Show();
		}
		if (j == 7)
		{
			edgepoints[11]->Show();
		}
	}
	if (i == 7)
	{
		if (j == 8)
		{
			edgepoints[12]->Show();
		}
		if (j == 9)
		{
			edgepoints[13]->Show();
		}
	}
	if (i == 8 && j == 9)
	{
		edgepoints[14]->Show();
	}
}
vector<int> GetPath(MGraph T, int v0, int i, int* sum)
{
	int p[10];
	int d[10];
	Dijstra(T, v0, p, d);
	*sum = d[i];
	vector<int> res;
	while (i != v0)
	{
		res.push_back(i);
		i = p[i];
		ShowPath(i, p[i]);
		ShowPath(p[i], i);
	}
	res.push_back(i);
	return res;
}
//刷新界面
void Fresh()
{
	loadimage(&img, _T("graphics\\map.jpg"));
	putimage(0, 0, &img);
	InitEdge();
	//edgepoints[10]->Show();
	//putimage(0, 0, &edges[0]);
	//edgepoints[0]->Hide();
	setfillstyle(BS_NULL);
	setlinestyle(PS_SOLID);
	setlinecolor(BLUE);
	settextcolor(RED);
	settextstyle(50, 10, _T("黑体"));
	setbkmode(TRANSPARENT);
	//rectangle(200, 200, 400, 400);
	fillrectangle(210, 510, 280, 610);
	point0.left = 210;
	point0.top = 510;
	point0.right = 280;
	point0.bottom = 610;
	outtextxy(215, 535, s1);
	//solidrectangle(600, 600, 800, 800);

	fillrectangle(590, 400, 640, 450);
	point1.left = 590;
	point1.top = 400;
	point1.right = 640;
	point1.bottom = 450;
	outtextxy(570, 400, s2);

	fillrectangle(650, 390, 720, 460);
	point2.left = 650;
	point2.top = 390;
	point2.right = 720;
	point2.bottom = 460;
	outtextxy(655, 400, s3);

	fillrectangle(640, 160, 720, 270);
	point3.left = 640;
	point3.top = 160;
	point3.right = 720;
	point3.bottom = 270;
	outtextxy(630, 185, s4);

	fillrectangle(245, 620, 280, 675);
	point4.left = 245;
	point4.top = 620;
	point4.right = 280;
	point4.bottom = 675;
	outtextxy(220, 620, s5);

	fillrectangle(700, 760, 765, 840);
	point5.left = 700;
	point5.top = 760;
	point5.right = 765;
	point5.bottom = 840;
	outtextxy(680, 780, s6);

	fillrectangle(970, 530, 1010, 605);
	point6.left = 970;
	point6.top = 530;
	point6.right = 1010;
	point6.bottom = 605;
	outtextxy(930, 540, s7);

	fillrectangle(1100, 520, 1200, 650);
	point7.left = 1100;
	point7.top = 520;
	point7.right = 1200;
	point7.bottom = 650;
	outtextxy(1110, 570, s8);

	fillrectangle(1190, 640, 1210, 680);
	point8.left = 1190;
	point8.top = 640;
	point8.right = 1210;
	point8.bottom = 680;
	outtextxy(1180, 630, s9);

	fillrectangle(820, 590, 910, 680);
	point9.left = 820;
	point9.top = 590;
	point9.right = 910;
	point9.bottom = 680;
	outtextxy(800, 610, s10);

	fillrectangle(1100, 200, 1290, 250);
	multipoint.left = 1100;
	multipoint.top = 200;
	multipoint.right = 1290;
	multipoint.bottom = 250;
	outtextxy(1180, 200, multi);

	fillrectangle(1100, 260, 1290, 310);
	exitpoint.left = 1100;
	exitpoint.top = 260;
	exitpoint.right = 1290;
	exitpoint.bottom = 310;
	outtextxy(1180, 260, exitText);

	settextcolor(BLUE);
	settextstyle(25, 5, _T("黑体"));
	setbkmode(TRANSPARENT);

	outtextxy(1000, 10, pointName);
	outtextxy(1000, 10 + textheight(pointIntroduce), pointIntroduce);
	outtextxy(1000, 100, curPoint);
}
vector<int> orders;
vector<int> multiorders;
void ShowEdge()
{
	int distance = 0;
	orders = GetPath(T, v0, v, &distance);
	int left = 0;
	for (int i = orders.size() - 1; i > 0; i--)
	{
		outtextxy(left, 10, GetName(orders[i]));
		left += textwidth(GetName(orders[i]));
		outtextxy(left, 10, _T("->"));
		left += textwidth(_T("->"));
		ShowPath(orders[i], orders[i - 1]);
		ShowPath(orders[i - 1], orders[i]);
	}
	outtextxy(left, 10, GetName(orders[0]));
	left += textwidth(GetName(orders[0])) + 10;
	TCHAR ac[128];
	_stprintf_s(ac, _T("总距离:%d"), distance);
	outtextxy(left, 10, ac);
}
void FloydShowEdge(int floyddistance)
{
	int left = 0;
	for (int i = 0; i < orders.size() - 1; i++)
	{
		outtextxy(left, 20, GetName(orders[i]));
		left += textwidth(GetName(orders[i]));
		outtextxy(left, 20, _T("->"));
		left += textwidth(_T("->"));
		ShowPath(orders[i], orders[i + 1]);
		ShowPath(orders[i + 1], orders[i]);
	}
	outtextxy(left, 20, GetName(orders[orders.size() - 1]));
	left += textwidth(GetName(orders[orders.size() - 1])) + 10;
	TCHAR ac[128];
	_stprintf_s(ac, _T("总距离:%d"), floyddistance);
	outtextxy(left, 20, ac);
}
void ShowMultiPoint()
{
	int left = 0;
	for (int i = 0; i < multiorders.size() - 1; i++)
	{
		outtextxy(left, 20, GetName(multiorders[i]));
		left += textwidth(GetName(multiorders[i]));
		outtextxy(left, 20, _T("，"));
		left += textwidth(_T("，"));
	}
	if (!multiorders.empty())
	{
		outtextxy(left, 20, GetName(multiorders[multiorders.size() - 1]));
		left += textwidth(GetName(multiorders[multiorders.size() - 1])) + 10;
		TCHAR ac[128];
		_stprintf_s(ac, _T("选择景点数:%d"), (int)multiorders.size());
		outtextxy(left, 20, ac);
	}
}
void ShowPoint()
{
	if (isMulti)
	{
		auto it = find(multiorders.begin(), multiorders.end(), v);
		if (it == multiorders.end())
		{
			multiorders.push_back(v);
		}
		else
		{
			multiorders.erase(it);
		}
		ShowMultiPoint();
	}
	if (v == 6)
	{
		outtextxy(1000 + textwidth(pointName), 10, s1);
		LPCTSTR res = _T("位于二期的运动场，其上附有篮球场。");
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce), res);
		if (v0 != -1)
		{
			/*Dijstra(T, v0, p, d);
			int i = 6;
			vector<int> order;
			order.push_back(i);
			double distance;
			while (i != v0)
			{
				i = p[i];

			}*/
			//HRGN rgn = CreateRectRgn(0, 10, 1000, 30);
			//setcliprgn(rgn);
			//clearcliprgn();
			ShowEdge();
		}
	}
	else if (v == 7)
	{
		outtextxy(1000 + textwidth(pointName), 10, s2);
		LPCTSTR res = _T("位于四期的学生餐厅，一般用餐人数是是各学生餐厅中最");
		LPCTSTR res1 = _T("多的，周边设有多个店铺。");
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce), res);
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce) + textheight(res), res1);
		if (v0 != -1)
		{
			//HRGN rgn = CreateRectRgn(0, 10, 1000, 30);
			//setcliprgn(rgn);
			//clearcliprgn();
			ShowEdge();
		}
	}
	else if (v == 8)
	{
		outtextxy(1000 + textwidth(pointName), 10, s3);
		LPCTSTR res = _T("各项体育活动的教室以及比赛场地的所在地。");
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce), res);
		if (v0 != -1)
		{
			//HRGN rgn = CreateRectRgn(0, 10, 1000, 30);
			//setcliprgn(rgn);
			//clearcliprgn();
			ShowEdge();
		}
	}
	else if (v == 9)
	{
		outtextxy(1000 + textwidth(pointName), 10, s4);
		LPCTSTR res = _T("众多科研项目的实验室的所在地，并配备多间教室，可供");
		LPCTSTR res1 = _T("自习和学生社团活动等。");
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce), res);
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce) + textheight(res), res1);
		if (v0 != -1)
		{
			//HRGN rgn = CreateRectRgn(0, 10, 1000, 30);
			//setcliprgn(rgn);
			//clearcliprgn();
			ShowEdge();
		}
	}
	else if (v == 5)
	{
		outtextxy(1000 + textwidth(pointName), 10, s5);
		LPCTSTR res = _T("位于二期的学生餐厅，三楼拥有多个店铺。");
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce), res);
		if (v0 != -1)
		{
			/*HRGN rgn = CreateRectRgn(0, 10, 1000, 30);
			setcliprgn(rgn);
			clearcliprgn();*/
			ShowEdge();
		}
	}
	else if (v == 4)
	{
		outtextxy(1000 + textwidth(pointName), 10, s6);
		LPCTSTR res = _T("3-6楼是学生自习和借阅书籍的场所，7、8楼是行政办公");
		LPCTSTR res1 = _T("区域，2楼设有两个报告厅与一个艺术展厅。");
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce), res);
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce) + textheight(res), res1);
		if (v0 != -1)
		{
			/*HRGN rgn = CreateRectRgn(0, 10, 1000, 30);
			setcliprgn(rgn);
			clearcliprgn();*/
			ShowEdge();
		}
	}
	else if (v == 2)
	{
		outtextxy(1000 + textwidth(pointName), 10, s7);
		LPCTSTR res = _T("学生事务办理办理大厅，钢琴房，展览厅，排练厅，办公");
		LPCTSTR res1 = _T("室，心理辅导室，社团工作室，多功能厅等的所在地。");
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce), res);
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce) + textheight(res), res1);
		if (v0 != -1)
		{
			//HRGN rgn = CreateRectRgn(0, 10, 1000, 30);
			//setcliprgn(rgn);
			//clearcliprgn();
			ShowEdge();
		}
	}
	else if (v == 1)
	{
		outtextxy(1000 + textwidth(pointName), 10, s8);
		LPCTSTR res = _T("厦门大学翔安校区最大的操场，周边有篮球场，足球场，");
		LPCTSTR res1 = _T("羽毛球场，排球场，网球场等。");
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce), res);
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce) + textheight(res), res1);
		if (v0 != -1)
		{
			//HRGN rgn = CreateRectRgn(0, 10, 1000, 30);
			//setcliprgn(rgn);
			//clearcliprgn();
			ShowEdge();
		}
	}
	else if (v == 0)
	{
		outtextxy(1000 + textwidth(pointName), 10, s9);
		LPCTSTR res = _T("出入厦门大学翔安校区的一个关卡。");
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce), res);
		if (v0 != -1)
		{
			//HRGN rgn = CreateRectRgn(0, 10, 1000, 30);
			//setcliprgn(rgn);
			//clearcliprgn();
			ShowEdge();
		}
	}
	else if (v == 3)
	{
		outtextxy(1000 + textwidth(pointName), 10, s10);
		LPCTSTR res = _T("俗称“小巨蛋”，一些大型活动的举办场地。");
		outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce), res);
		if (v0 != -1)
		{
			//HRGN rgn = CreateRectRgn(0, 10, 1000, 30);
			//setcliprgn(rgn);
			//clearcliprgn();
			ShowEdge();
		}
	}
	switch (v0)
	{
	case 6:outtextxy(1000 + textwidth(pointName), 100, s1); break;
	case 7:outtextxy(1000 + textwidth(pointName), 100, s2); break;
	case 8:outtextxy(1000 + textwidth(pointName), 100, s3); break;
	case 9:outtextxy(1000 + textwidth(pointName), 100, s4); break;
	case 5:outtextxy(1000 + textwidth(pointName), 100, s5); break;
	case 4:outtextxy(1000 + textwidth(pointName), 100, s6); break;
	case 2:outtextxy(1000 + textwidth(pointName), 100, s7); break;
	case 1:outtextxy(1000 + textwidth(pointName), 100, s8); break;
	case 0:outtextxy(1000 + textwidth(pointName), 100, s9); break;
	case 3:outtextxy(1000 + textwidth(pointName), 100, s10); break;
	}
	mciSendString(_T("open sfx\\pointclick.mp3 alias pc"), NULL, 0, NULL);
	mciSendString(_T("seek pc to start"), NULL, 0, NULL);
	mciSendString(_T("play pc"), NULL, 0, NULL);
}
void FloyedShowPath(int s, int e)
{
	if (fp[s][e] == e || fp[s][e] == s || fp[s][e] == INT_MAX / 2)
	{
		return;
	}
	else
	{
		FloyedShowPath(s, fp[s][e]);
		orders.push_back(fp[s][e]);
		FloyedShowPath(fp[s][e], e);
	}
}
void ShowMultiEdge()
{
	int distance = 0;
	for (int i = 0; i < multiorders.size() - 1; i++)
	{
		orders.push_back(multiorders[i]);
		FloyedShowPath(multiorders[i], multiorders[i + 1]);
		distance += fd[multiorders[i]][multiorders[i + 1]];
	}
	orders.push_back(multiorders[multiorders.size() - 1]);
	FloydShowEdge(distance);
}
void Run()
{
	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EX_MOUSE);
		if (mess.lbutton)
		{
			Fresh();
			settextstyle(25, 5, _T("黑体"));
			settextcolor(BLACK);
			//HRGN rgn = CreateRectRgn(1000 + textwidth(pointName), 10, 1300, 100);
			//setcliprgn(rgn);
			//clearcliprgn();
			if (PointInRect(mess.x, mess.y, point0))
			{
				v = 6;
				ShowPoint();
				////clearcliprgn();
				//outtextxy(1000 + textwidth(pointName), 10, s1);
				////TCHAR* intro = Transform(T.introduce[6]);
				///*char intro[100];
				//sprintf(intro,"%s", T.introduce[6]);*/
				////LPCTSTR res = _T(intro);
				//LPCTSTR res = _T("位于二期的运动场，其上附有篮球场。");
				//outtextxy(1000 + textwidth(pointIntroduce), 10 + textheight(pointIntroduce), res);
				//if (v0 != -1)
				//{
				//	/*Dijstra(T, v0, p, d);
				//	int i = 6;
				//	vector<int> order;
				//	order.push_back(i);
				//	double distance;
				//	while (i != v0)
				//	{
				//		i = p[i];

				//	}*/
				//	//HRGN rgn = CreateRectRgn(0, 10, 1000, 30);
				//	//setcliprgn(rgn);
				//	//clearcliprgn();
				//	int distance = 0;
				//	orders = GetPath(T, v0, v, &distance);
				//	int left = 0;
				//	for (int i = orders.size() - 1; i > 0; i--)
				//	{
				//		outtextxy(left, 10, GetName(orders[i]));
				//		left += textwidth(GetName(orders[i]));
				//		outtextxy(left, 10, _T("->"));
				//		left += textwidth(_T("->"));
				//		ShowPath(orders[i], orders[i - 1]);
				//		ShowPath(orders[i - 1], orders[i]);
				//	}
				//	outtextxy(left, 10, GetName(orders[0]));
				//	left += textwidth(GetName(orders[0])) + 10;
				//	TCHAR ac[128];
				//	_stprintf_s(ac, _T("总距离:%d"), distance);
				//	outtextxy(left, 10, ac);
				//}
			}
			if (PointInRect(mess.x, mess.y, point1))
			{
				v = 7;
				//clearcliprgn();
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point2))
			{
				v = 8;
				//clearcliprgn();
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point3))
			{
				v = 9;
				//clearcliprgn();
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point4))
			{
				v = 5;
				//clearcliprgn();
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point5))
			{
				v = 4;
				//clearcliprgn();
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point6))
			{
				v = 2;
				//clearcliprgn();
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point7))
			{
				v = 1;
				//clearcliprgn();
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point8))
			{
				v = 0;
				//clearcliprgn();
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point9))
			{
				v = 3;
				//clearcliprgn();
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, multipoint))
			{
				if (!isMulti)
				{
					multiorders.clear();
					orders.clear();
				}
				isMulti = !isMulti;
				v0 = -1;
				if (!isMulti && multiorders.size() > 1)
				{
					ShowMultiEdge();
				}
				mciSendString(_T("open sfx\\buttonclick.mp3 alias bc"), NULL, 0, NULL);
				mciSendString(_T("seek bc to start"), NULL, 0, NULL);
				mciSendString(_T("play bc"), NULL, 0, NULL);
			}
			if (PointInRect(mess.x, mess.y, exitpoint))
			{
				mciSendString(_T("open sfx\\buttonclick.mp3 alias bc"), NULL, 0, NULL);
				mciSendString(_T("seek bc to start"), NULL, 0, NULL);
				mciSendString(_T("play bc"), NULL, 0, NULL);
				exit(0);
			}
		}
		else if (mess.rbutton && !isMulti)
		{
			Fresh();
			settextstyle(25, 5, _T("黑体"));
			settextcolor(WHITE); 
			//HRGN rgn = CreateRectRgn(1000 + textwidth(pointName), 100, 1300, 200);
			//setcliprgn(rgn);
			//clearcliprgn();
			if (PointInRect(mess.x, mess.y, point0))
			{
				//clearcliprgn();
				v0 = 6;
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point1))
			{
				//clearcliprgn();
				v0 = 7;
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point2))
			{
				//clearcliprgn();
				v0 = 8;
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point3))
			{
				//clearcliprgn();
				v0 = 9;
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point4))
			{
				//clearcliprgn();
				v0 = 5;
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point5))
			{
				//clearcliprgn();
				v0 = 4;
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point6))
			{
				//clearcliprgn();
				v0 = 2;
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point7))
			{
				//clearcliprgn();
				v0 = 1;
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point8))
			{
				//clearcliprgn();
				v0 = 0;
				ShowPoint();
			}
			if (PointInRect(mess.x, mess.y, point9))
			{
				//clearcliprgn();
				v0 = 3;
				ShowPoint();
			}
		}
	}
	//HRGN rgn = CreateRectRgn(0, 0, 1394, 1080);
	//setcliprgn(rgn);
	//受不了啦
	//for (int i = 0; i < orders.size() - 1; i++)
	//{
	//	ShowPath(orders[i], orders[i + 1]);
	//	ShowPath(orders[i + 1], orders[i]);
	//	/*if (orders[i] == 0)
	//	{
	//		if (orders[i+1] == 1)
	//		{
	//			edgepoints[0]->Show();
	//		}
	//		if (orders[i+1] == 2)
	//		{
	//			edgepoints[1]->Show();
	//		}
	//		if (orders[i + 1] == 3)
	//		{
	//			edgepoints[2]->Show();
	//		}
	//		if (orders[i + 1] == 4)
	//		{
	//			edgepoints[3]->Show();
	//		}
	//	}
	//	if (orders[i] == 1 && orders[i + 1] == 2)
	//	{
	//		edgepoints[4]->Show();
	//	}
	//	if (orders[i] == 2 && orders[i + 1] == 3)
	//	{
	//		edgepoints[5]->Show();
	//	}
	//	if (orders[i] == 2 && orders[i + 1] == 8)
	//	{
	//		edgepoints[6]->Show();
	//	}
	//	if (orders[i] == 3 && orders[i + 1] == 4)
	//	{
	//		edgepoints[7]->Show();
	//	}
	//	if (orders[i] == 3 && orders[i + 1] == 8)
	//	{
	//		edgepoints[8]->Show();
	//	}
	//	if (orders[i] == 4 && orders[i + 1] == 5)
	//	{
	//		edgepoints[9]->Show();
	//	}
	//	if (orders[i] == 5)
	//	{
	//		if (orders[i + 1] == 6)
	//		{
	//			edgepoints[10]->Show();
	//		}
	//		if (orders[i + 1] == 7)
	//		{
	//			edgepoints[11]->Show();
	//		}
	//	}
	//	if (orders[i] == 7)
	//	{
	//		if (orders[i + 1] == 8)
	//		{
	//			edgepoints[12]->Show();
	//		}
	//		if (orders[i + 1] == 9)
	//		{
	//			edgepoints[13]->Show();
	//		}
	//	}
	//	if (orders[i] == 8 && orders[i + 1] == 9)
	//	{
	//		edgepoints[14]->Show();
	//	}*/
	//}
}
//开始界面
void Welcome()
{
	initgraph(1394, 1080, EW_NOMINIMIZE | EW_SHOWCONSOLE);
	InitialGraph(T);
	Floyd(T);
	BeginBatchDraw();

	//设置背景
	setbkcolor(BLACK);
	cleardevice();
	Fresh();
	
	EndBatchDraw();
	//获得输入
	Run();
	closegraph();
}
