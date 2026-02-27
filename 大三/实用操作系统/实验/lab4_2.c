#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<time.h>
#include<string.h>
#define NUM_READERS 2
#define NUM_WRITERS 1
#define NUM_OPERATIONS 5

typedef struct{
	int shared_data;
	int read_count;
	int reader_completed;
	int writer_completed;
}shared_data_t;

#define RESOURCE 0
#define READ_MUTEX 1

int create_semaphores()
{
	int semid;
	key_t key;
	if((key=ftok(".",'R'))==-1)
	{
		printf("ftok");
		exit(1);
	}
	if((semid=semget(key,2,IPC_CREAT|0666))==-1)
	{
		printf("semget");
		exit(1);
	}
	return semid;
}
void init_semaphores(int semid)
{
	if(semctl(semid,RESOURCE,SETVAL,1)==-1)
	{
		printf("semctl RESOURCE");
		exit(1);
	}
	if(semctl(semid,READ_MUTEX,SETVAL,1)==-1)
	{
		printf("semctll READ_MUTEX");
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
		exit(1);
	}
}
void reader_process(int semid,shared_data_t *shared_data,int reader_id)
{
	srand(time(NULL)^getpid());
	int operations=0;
	printf("Reader %d (PID %d) active\n",reader_id,getpid());
	
	while(operations<NUM_OPERATIONS)
	{
		p(semid,READ_MUTEX);
		shared_data->read_count++;
		if(shared_data->read_count==1)
		{
			p(semid,RESOURCE);
		}
		
		v(semid,READ_MUTEX);
		printf("Reader %d is reading data %d\n",reader_id,shared_data->shared_data);
		sleep(1);
		
		p(semid,READ_MUTEX);
		shared_data->read_count--;
		if(shared_data->read_count==0)
		{
			v(semid,RESOURCE);
		}
		v(semid,READ_MUTEX);
		operations++;
		shared_data->reader_completed++;
		sleep(1);
	}
	printf("Reader %d (PID %d) complete %d operations\n",reader_id,getpid(),NUM_OPERATIONS);
}
void writer_process(int semid,shared_data_t *shared_data,int writer_id)
{
	srand(time(NULL)^getpid());
	int operations=0;
	printf("Writer %d (PID %d) active\n",writer_id,getpid());
	
	while(operations<NUM_OPERATIONS)
	{
		p(semid,RESOURCE);
		int new_data=rand()%100;
		shared_data->shared_data=new_data;
		printf("Writer %d is writing data %d\n",writer_id,new_data);
		sleep(1);
		
		v(semid,RESOURCE);
		operations++;
		shared_data->writer_completed++;
		sleep(2);
	}
	printf("Writer %d (PID %d) complete %d operations\n",writer_id,getpid(),NUM_OPERATIONS);
}
int main()
{
	int semid;
	shared_data_t *shared_data;
	pid_t pids[NUM_READERS+NUM_WRITERS];
	int i;

	semid=create_semaphores();
	init_semaphores(semid);
	
	shared_data=mmap(NULL,sizeof(shared_data_t),PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	if(shared_data==MAP_FAILED)
	{
		printf("mmap");
		exit(1);
	}
	memset(shared_data,0,sizeof(shared_data_t));
	shared_data->shared_data=0;
	shared_data->read_count=0;
	shared_data->reader_completed=0;
	shared_data->writer_completed=0;
	
	for(i=0;i<NUM_READERS;i++)
	{
		pid_t pid=fork();
		if(pid==-1)
		{
			printf("fork");
			exit(1);
		}
		else if(pid==0)
		{
			reader_process(semid,shared_data,i+1);
			exit(0);
		}
		else
		{
			pids[i]=pid;
		}
	}
	for(i=0;i<NUM_WRITERS;i++)
	{
		pid_t pid=fork();
		if(pid==-1)
		{
			printf("fork");
			exit(1);
		}
		else if(pid==0)
		{
			writer_process(semid,shared_data,i+1);
			exit(0);
		}
		else
		{
			pids[NUM_READERS+i]=pid;
		}
	}
	for(i=0;i<NUM_READERS+NUM_WRITERS;i++)
	{
		waitpid(pids[i],NULL,0);
	}
	
	printf("Reader complete operations count %d\n",shared_data->reader_completed);
	printf("Writer complete operations count %d\n",shared_data->writer_completed);
	printf("Total shared data count %d\n",shared_data->shared_data);
	if(semctl(semid,0,IPC_RMID)==-1)
	{
		printf("semctl IPC_RMID");
	}
	if(munmap(shared_data,sizeof(shared_data_t)))
	{
		printf("munmap");
	}
	return 0;
}
