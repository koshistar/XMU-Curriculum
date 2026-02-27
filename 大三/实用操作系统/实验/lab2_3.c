#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>

int main()
{
	pid_t pid1,pid2;
	pid1=fork();
	if(pid1<0)
	{
		printf("error in fork\n");
	}
	else if(pid1==0)
	{
		printf("Child process 1 is executing\n");
		sleep(1);
		exit(0);
	}
	pid2=fork();
	if(pid2<0)
	{
		printf("error in fork\n");
	}
	else if(pid2==0)
	{
		printf("Child process 2 is executing\n");
		sleep(1);
		exit(0);
	}

	printf("Father process is executing\n");
	sleep(3);

	kill(pid1,SIGKILL);
	printf("Child1 process 1 is killed.\n");
	kill(pid2,SIGKILL);
	printf("Child2 process 2 is killed.\n");
	printf("Father process is over.\n");
	
	return 0;
}	
