#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<sys/wait.h>
#define MAXMSG 512
struct my_msg
{
    long int my_msg_type;
    char some_text[MAXMSG];
}msg;
int main()
{
    int msgid; //定义消息缓冲区内部标识
    long int msg_to_receive=0;
    msgid=msgget(1234,0666|IPC_CREAT); //创建消息队列,key为1234
    while(1)
    {
        msgrcv(msgid,&msg,BUFSIZ,msg_to_receive,0); //接收消息
        printf("You wrote: %s",msg.some_text); //显示消息
        if(strncmp(msg.some_text,"end",3)==0) //消息为"end"则结束
            break;
    }
    msgctl(msgid,IPC_RMID,0); //删除消息队列
    exit(0);
    return 0;
}