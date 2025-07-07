#pragma once
#include<iostream>
#include<string>
#define SCENE_NUM 10
#define EDGE_NUM 13
using namespace std;
typedef struct
{
	string name[SCENE_NUM];
	string introduce[SCENE_NUM];
	int edges[SCENE_NUM][SCENE_NUM];
}MGraph;
int fp[SCENE_NUM][SCENE_NUM], fd[SCENE_NUM][SCENE_NUM];
void InitialGraph(MGraph &G)
{
	G.name[0] = "东门";
	G.introduce[0] = "出入厦门大学翔安校区的一个关卡。";
	G.name[1] = "一期运动场";
	G.introduce[1] = "厦门大学翔安校区最大的操场，周边有篮球场，足球场，羽毛球场，排球场，网球场等。";
	G.name[2] = "学生活动中心。";
	G.introduce[2] = "学生事务办理办理大厅，钢琴房，展览厅，排练厅，办公室，心理辅导室，社团工作室，多功能厅等的所在地。";
	G.name[3] = "学生广场";
	G.introduce[3] = "俗称“小巨蛋”，一些大型活动的举办场地。";
	G.name[4] = "德旺图书馆";
	G.introduce[4] = "3-6楼是学生自习和借阅书籍的场所，7、8楼是行政办公区域，2楼设有两个报告厅与一个艺术展厅。";
	G.name[5] = "丰庭餐厅";
	G.introduce[5] = "位于二期的学生餐厅，三楼拥有多个店铺。";
	G.name[6] = "二期运动场";
	G.introduce[6] = "位于二期的运动场，其上附有篮球场。";
	G.name[7] = "思源餐厅";
	G.introduce[7] = "位于四期的学生餐厅，一般用餐人数是是各学生餐厅中最多的，周边设有多个店铺。";
	G.name[8] = "爱秋体育馆";
	G.introduce[8] = "各项体育活动的教室以及比赛场地的所在地。";
	G.name[9] = "新工科研发大楼";
	G.introduce[9] = "众多科研项目的实验室的所在地，并配备多间教室，可供自习和学生社团活动等。";
	for (int i = 0; i < SCENE_NUM; i++)
	{
		for (int j = 0; j < SCENE_NUM; j++)
		{
			if (i == j)
				G.edges[i][j] = 0;
			else
				G.edges[i][j] = INT_MAX/2;
		}
	}
	G.edges[0][1] = G.edges[1][0] = 191;
	G.edges[0][2] = G.edges[2][0] = 519;
	G.edges[0][3] = G.edges[3][0] = 627;
	G.edges[0][4] = G.edges[4][0] = 1200;
	G.edges[1][2] = G.edges[2][1] = 184;
	G.edges[2][3] = G.edges[3][2] = 499;
	G.edges[2][8] = G.edges[8][2] = 236;
	G.edges[3][4] = G.edges[4][3] = 404;
	G.edges[3][8] = G.edges[8][3] = 487;
	G.edges[4][5] = G.edges[5][4] = 1200;
	G.edges[5][6] = G.edges[6][5] = 156;
	G.edges[5][7] = G.edges[7][5] = 938;
	G.edges[7][8] = G.edges[8][7] = 98;
	G.edges[7][9] = G.edges[9][7] = 345;
	G.edges[8][9] = G.edges[9][8] = 363;
}
//DIjkstra求最短路径
void Dijstra(MGraph G, int v0, int *p, int *d)
{
	int min;
	bool find[10];
	for (int i = 0; i < SCENE_NUM; i++)
	{
		find[i] = false;
		d[i] = G.edges[v0][i];
		p[i] = v0;
	}
	d[v0] = 0;
	find[v0] = true;
	int k;
	for (int i = 1; i < SCENE_NUM; i++)
	{
		min = INT_MAX/2;
		for (int j = 0; j < SCENE_NUM; j++)
		{
			if (!find[j] && d[j] < min)
			{
				k = j;
				min = d[j];
			}
		}
		find[k] = true;
		for (int j = 0; j < SCENE_NUM; j++)
		{
			if (!find[j] && (min + G.edges[k][j]) < d[j])
			{
				d[j] = min + G.edges[k][j];
				p[j] = k;
			}
		}
	}
}
//Floyd求多个景点的最佳路径
void Floyd(MGraph G)
{
	for (int i = 0; i < SCENE_NUM; i++)
	{
		for (int j = 0; j < SCENE_NUM; j++)
		{
			fd[i][j] = G.edges[i][j];
			fp[i][j] = j;
		}
	}
	for (int k = 0; k < SCENE_NUM; ++k)
	{
		for (int i = 0; i < SCENE_NUM; ++i)
		{
			for (int j = 0; j < SCENE_NUM; ++j)
			{
				if (fd[i][j] > fd[i][k] + fd[k][j])
				{
					fd[i][j] = fd[i][k] + fd[k][j];
					fp[i][j] = fp[i][k];
				}
			}
		}
	}
}