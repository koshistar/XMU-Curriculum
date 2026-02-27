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
    char buffer[BUFSIZ]; //定义用户缓冲区
    msgid=msgget(1234,0666|IPC_CREAT); //创建消息队列,key为1234
    while(1)
    {
        puts("Enter some text:"); //提示键入消息内容
        fgets(buffer,BUFSIZ,stdin);  //标准输入送buffer
        msg.my_msg_type=1; //设置消息类型为1
        strcpy(msg.some_text,buffer); //buffer送消息缓冲
        msgsnd(msgid,&msg,MAXMSG,0); //发送消息到消息队列
        if(strncmp(msg.some_text,"end",3)==0) //消息为"end"则结束
            break;
    }
    return 0;
}