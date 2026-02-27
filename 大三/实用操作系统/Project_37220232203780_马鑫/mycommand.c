#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/syscall.h>
#define __NR_myhello 548
long myhello(char *buf,int len)
{
    return syscall(__NR_myhello,buf,len);
}
int main()
{
    char buffer[128];
    long result=myhello(buffer,sizeof(buffer));
    if(result<0)
    {
        perror("System call failed");
        return 1;
    }
    printf("System call returned: %s\n",buffer);
    return 0;
}