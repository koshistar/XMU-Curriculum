#include<stdio.h>
#include<unistd.h>
int main()
{
	pid_t pid = fork();
	if(pid<0)
	{
		printf("error in fork!");
	}
	else if(pid==0)
	{
		printf("This is the child process. PID: %d\n",getpid());
	}
	else
	{
		printf("This is the parent process. PID: %d\n",getpid());
	}
	return 0;
}
