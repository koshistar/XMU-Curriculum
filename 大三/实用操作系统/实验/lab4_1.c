#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/wait.h>
#include<sys/mman.h>

#define BUFFER_SIZE 5
#define MAX_ITEMS 10

typedef struct
{
	int buffer[BUFFER_SIZE];
	int in;
	int out;
	int count;
	int produced_count;
	int consumed_count;
}shared_data_t;
struct sembuf P={0,-1,SEM_UNDO};
struct sembuf V={0,-1,SEM_UNDO};
#define MUTEX 0
#define EMPTY 1
#define FULL 2
int create_semaphores()
{
	int semid;
	key_t key;
	if((key=ftok(".",'S'))==-1)
	{
		printf("ftok");
		exit(1);
	}
	if((semid=semget(key,3,IPC_CREAT|0666))==-1)
	{
		printf("semget");
		exit(1);
	}
	return semid;
}
void init_semaphores(int semid)
{
	if(semctl(semid,MUTEX,SETVAL,1)==-1)
	{
		printf("semctl MUTEX");
		exit(1);
	}
	if(semctl(semid,EMPTY,SETVAL,BUFFER_SIZE)==-1)
	{
		printf("semctl EMPTY");
		exit(1);
	}
	if(semctl(semid,FULL,SETVAL,0)==-1)
	{
		printf("semctl FULL");
		exit(1);
	}

}
void p(int semid,int sem_num)
{
	struct sembuf op={sem_num,-1,SEM_UNDO};
	if(semop(semid,&op,1)==-1)
	{
		printf("semop P");
		exit(1);
	}
}
void v(int semid,int sem_num)
{
	struct sembuf op={sem_num,1,SEM_UNDO};
	if(semop(semid,&op,1)==-1)
	{
		printf("semop V");
		exit(0);
	}
}

void producer_process(int semid,shared_data_t *shared_data)
{
	while(shared_data->produced_count<MAX_ITEMS)
	{
		int item=rand()%100;
		p(semid,EMPTY);
		p(semid,MUTEX);
		shared_data->buffer[shared_data->in]=item;
		printf("Producer %d produce %d in position %d\n",getpid(),item,shared_data->in);
		shared_data->in=(shared_data->in+1)%BUFFER_SIZE;
		shared_data->count++;
		shared_data->produced_count++;
		v(semid,MUTEX);
		v(semid,FULL);
		sleep(1);
	}
	printf("Producer %d complete producing.\n",getpid());
}
void consumer_process(int semid,shared_data_t *shared_data)
{
	while(shared_data->consumed_count<MAX_ITEMS)
	{
		p(semid,FULL);
		p(semid,MUTEX);
		int item=shared_data->buffer[shared_data->out];
		printf("Consumer %d consume %d from %d\n",getpid(),item,shared_data->out);
		shared_data->out=(shared_data->out+1)%BUFFER_SIZE;
		shared_data->count--;
		shared_data->consumed_count++;
		v(semid,MUTEX);
		v(semid,EMPTY);
		sleep(2);
	}
	printf("Consumer %d complete consuming.\n",getpid());
}
int main()
{
	int semid;
	shared_data_t *shared_data;
	pid_t pid;
	semid=create_semaphores();
	init_semaphores(semid);
	shared_data=mmap(NULL,sizeof(shared_data_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	if(shared_data==MAP_FAILED)
	{
		printf("mmp");
		exit(1);
	}
	shared_data->in=0;
	shared_data->out=0;
	shared_data->count=0;
	shared_data->produced_count=0;
	shared_data->consumed_count=0;
	pid=fork();
	if(pid==-1)
	{
		printf("fork");
		exit(1);
	}
	else if(pid==0)
	{
		consumer_process(semid,shared_data);
		exit(0);
	}
	else
	{
		producer_process(semid,shared_data);
		wait(NULL);
		printf("Final:\n");
		printf("Produced count: %d\n",shared_data->produced_count);
		printf("Consumed count: %d\n",shared_data->consumed_count);
		printf("Remain count: %d\n",shared_data->count);

		if(semctl(semid,0,IPC_RMID)==-1)
		{
			printf("semctl IPC_RMID");
		}
		if(munmap(shared_data,sizeof(shared_data_t))==-1)
		{
			printf("munmap");
		}
	}
	return 0;
}
