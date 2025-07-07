#pragma once
#define HASH_SIZE 50
#define NAME_SIZE 30
#define P 49 //增加p值以减小asl
#include<iostream>
#include<ctype.h>
#include<string>
using namespace std;
typedef struct
{
	string furName;
	int ascii;
}FurName;
typedef struct
{
	string furName;
	int ascii;
	int s;
}Hash;
FurName nameList[NAME_SIZE];
Hash hashTable[HASH_SIZE];
//初始化
void initNameList()
{
	nameList[0].furName = "wang";
	nameList[1].furName = "li";
	nameList[2].furName = "zhang";
	nameList[3].furName = "liu";
	nameList[4].furName = "chen";
	nameList[5].furName = "yang";
	nameList[6].furName = "huang";
	nameList[7].furName = "zhao";
	nameList[8].furName = "wu";
	nameList[9].furName = "zhou";
	nameList[10].furName = "xu";
	nameList[11].furName = "sun";
	nameList[12].furName = "ma";
	nameList[13].furName = "zhu";
	nameList[14].furName = "hu";
	nameList[15].furName = "guo";
	nameList[16].furName = "he";
	nameList[17].furName = "lin";
	nameList[18].furName = "gao";
	nameList[19].furName = "luo";
	nameList[20].furName = "zheng";
	nameList[21].furName = "liang";
	nameList[22].furName = "xie";
	nameList[23].furName = "song";
	nameList[24].furName = "tang";
	nameList[25].furName = "xu";
	nameList[26].furName = "deng";
	nameList[27].furName = "han";
	nameList[28].furName = "feng";
	nameList[29].furName = "cao";
	for (int i = 0; i < NAME_SIZE; ++i)
	{
		for (char c : nameList[i].furName)
		{
			nameList[i].ascii += toascii(c) - 'a';
		}
	}
}
void Collision(int i)
{
	int key = nameList[i].ascii % P;
	while (hashTable[key].s != 0)
	{
		key = (key + 1) % HASH_SIZE;//线性探测
	}
	hashTable[key].furName = nameList[i].furName;
	hashTable[key].ascii = nameList[i].ascii;
	if (key > nameList[i].ascii % P)
	{
		hashTable[key].s = key - nameList[i].ascii % P + 1;
	}
	else
	{
		hashTable[key].s = HASH_SIZE - nameList[i].ascii % P + key + 1;
	}
}
void CreateHashTable()
{
	for (int i = 0; i < HASH_SIZE; ++i)
	{
		hashTable[i].furName = "\0";
		hashTable[i].ascii = 0;
		hashTable[i].s = 0;
	}
	for (int i = 0; i < NAME_SIZE; ++i)
	{
		int key = nameList[i].ascii % P;//除留余数法
		if (hashTable[key].s == 0)
		{
			hashTable[key].furName = nameList[i].furName;
			hashTable[key].ascii = nameList[i].ascii;
			hashTable[key].s = 1;
		}
		else
		{
			Collision(i);
		}
	}
}
void PrintNameList()
{
	for (auto& var : nameList)
	{
		cout << var.furName << "'s key is " << var.ascii << endl;
	}
}
void PrintHashTable()
{
	float asl = 0;
	int i = 0;
	for (auto& var : hashTable)
	{
		if(var.s)
			cout << i << "'s value is "<< var.furName << " " << var.ascii << endl;
		asl += var.s;
		++i;
	}
	cout << "ASL is " << asl / NAME_SIZE;
}
bool Search(string str)
{
	int ascii = 0;
	for(char c:str)
	{
		ascii += toascii(c - 'a');
	}
	if (hashTable[ascii].furName == str)
	{
		return true;
	}
	for (int i = (ascii + 1) % HASH_SIZE; i != ascii; i = (i + 1) % HASH_SIZE)
	{
		if (hashTable[i].furName == str)
			return true;
	}
	return false;
}