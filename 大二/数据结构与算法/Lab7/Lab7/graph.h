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
	G.name[0] = "����";
	G.introduce[0] = "�������Ŵ�ѧ�谲У����һ���ؿ���";
	G.name[1] = "һ���˶���";
	G.introduce[1] = "���Ŵ�ѧ�谲У�����Ĳٳ����ܱ������򳡣����򳡣���ë�򳡣����򳡣����򳡵ȡ�";
	G.name[2] = "ѧ������ġ�";
	G.introduce[2] = "ѧ��������������������ٷ���չ���������������칫�ң��������ң����Ź����ң��๦�����ȵ����ڵء�";
	G.name[3] = "ѧ���㳡";
	G.introduce[3] = "�׳ơ�С�޵�����һЩ���ͻ�ľٰ쳡�ء�";
	G.name[4] = "����ͼ���";
	G.introduce[4] = "3-6¥��ѧ����ϰ�ͽ����鼮�ĳ�����7��8¥�������칫����2¥����������������һ������չ����";
	G.name[5] = "��ͥ����";
	G.introduce[5] = "λ�ڶ��ڵ�ѧ����������¥ӵ�ж�����̡�";
	G.name[6] = "�����˶���";
	G.introduce[6] = "λ�ڶ��ڵ��˶��������ϸ������򳡡�";
	G.name[7] = "˼Դ����";
	G.introduce[7] = "λ�����ڵ�ѧ��������һ���ò��������Ǹ�ѧ�����������ģ��ܱ����ж�����̡�";
	G.name[8] = "����������";
	G.introduce[8] = "����������Ľ����Լ��������ص����ڵء�";
	G.name[9] = "�¹����з���¥";
	G.introduce[9] = "�ڶ������Ŀ��ʵ���ҵ����ڵأ����䱸�����ң��ɹ���ϰ��ѧ�����Ż�ȡ�";
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
//DIjkstra�����·��
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
//Floyd������������·��
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