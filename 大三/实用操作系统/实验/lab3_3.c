#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/types.h>
pid_t pid1,pid2;
void fun(int signal)
{
	kill(pid1,SIGUSR1);
	kill(pid2,SIGUSR1);
}
void fun2(int signal)
{
	printf("Child process 1 is killed by parent!\n");
	exit(0);
}
void fun1(int signal)
{
	printf("Child process 2 is killed by parent!\n");
	exit(0);
}
int main()
{
	pid1=fork();
	if(pid1==0)
	{
		signal(SIGUSR1,fun1);
		pause();
		exit(0);
	}
	else if(pid1<0)
	{
		printf("Child process 1 is error.\n");
		exit(1);
	}
	pid2=fork();
	if(pid2==0)
	{
		signal(SIGUSR1,fun2);
		pause();
		exit(0);
	}
	else if(pid2<0)
	{
		printf("Child process 2 is error.\n");
		exit(1);
	}
	else
	{
		signal(SIGINT,fun);
		int status;
		wait(&status);
		wait(&status);
		printf("Parent process is killed!\n");
	}
	return 0;
}
