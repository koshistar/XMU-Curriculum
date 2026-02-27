#include<stdio.h>
#include<unistd.h>

int main()
{
	int val=0;
	pid_t pid=fork();
	if(pid<0)
	{
		printf("error in fork");
	}
	else if(pid==0)
	{
		printf("Child process: val = %d, PID = %d\n", val, getpid());
		val += 5;
		printf("After val += 5: val = %d, PID = %d\n", val, getpid());
	}
	else
	{
		printf("Parent Process: val = %d, PID = %d\n", val, getpid());
		val += 10;
		printf("After val += 10: val = %d, PID = %d\n", val, getpid());
	}
	return 0;
}
