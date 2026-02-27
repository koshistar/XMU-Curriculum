#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#define MAX_PROCESS 10
typedef struct 
{
	char name;
	int arrival_time;
	int service_time;
	int start_time;
	int finish_time;
	int turnaround_time;
	float weighted_turnaround;
	float response_ratio;
	int remaining_time;
	int waiting_time;
}Process;
Process processes[MAX_PROCESS];
int n=5;
void initialize_processes()
{
	processes[0].name='A';
	processes[0].arrival_time=0;
	processes[0].service_time=3;

	processes[1].name='B';
	processes[1].arrival_time=2;
	processes[1].service_time=6;

	processes[2].name='C';
	processes[2].arrival_time=4;
	processes[2].service_time=4;

	processes[3].name='D';
	processes[3].arrival_time=6;
	processes[3].service_time=5;

	processes[4].name='E';
	processes[4].arrival_time=8;
	processes[4].service_time=2;
}
void reset_processes()
{
	for(int i=0;i<n;i++)
	{
		processes[i].start_time=-1;
		processes[i].finish_time=-1;
		processes[i].turnaround_time=0;
		processes[i].weighted_turnaround=0;
		processes[i].response_ratio=0;
		processes[i].remaining_time=processes[i].service_time;
		processes[i].waiting_time=0;
	}
}
void print_result(char* algorithm_name)
{
	printf("\n%s's result:",algorithm_name);
	printf("\nprocess\tarrival time\tservice time\tstart time\tfinish time\tturnaround time\tweighted turnaround time");
	if(strcmp(algorithm_name,"HRRN")==0)
		printf("\tresponse ratio");
	printf("\n");
	float avg_turnaround=0;
	float avg_weighted_turnaround=0;

	for(int i=0;i<n;i++)
	{
		printf("%c\t%d\t\t%d\t\t",processes[i].name,processes[i].arrival_time,processes[i].service_time);
		if(processes[i].start_time!=-1)
		{
			printf("%d\t\t%d\t\t%d\t\t%.2f",processes[i].start_time,processes[i].finish_time,processes[i].turnaround_time,processes[i].weighted_turnaround);
		}
		else
		{
			printf("-\t\t-\t\t-\t\t-");
		}
		if(strcmp(algorithm_name,"HRRN")==0)
		{
			printf("\t\t%.2f",processes[i].response_ratio);
		}
		printf("\n");
		if(processes[i].finish_time!=-1)
		{
			avg_turnaround+=processes[i].turnaround_time;
			avg_weighted_turnaround+=processes[i].weighted_turnaround;
		}
	}
	printf("average turnaround time: %.2f\n",avg_turnaround/n);
	printf("avergge weighted turnaround time: %.2f\n",avg_weighted_turnaround/n);
}
void fcfs_scheduling()
{
	reset_processes();
	int current_time=0;
	int completed=0;
	while(completed<n)
	{
		int next_process=-1;
		int min_arrival=1000;
		for(int i=0;i<n;i++)
		{
			if(processes[i].arrival_time<=current_time&&processes[i].start_time==-1&&processes[i].arrival_time<min_arrival)
			{
				min_arrival=processes[i].arrival_time;
				next_process=i;
			}
		}
		if(next_process!=-1)
		{
			processes[next_process].start_time=current_time;
			processes[next_process].finish_time=current_time+processes[next_process].service_time;
			processes[next_process].turnaround_time=processes[next_process].finish_time-processes[next_process].arrival_time;
			processes[next_process].weighted_turnaround=(float)processes[next_process].turnaround_time/processes[next_process].service_time;
			current_time=processes[next_process].finish_time;
			completed++;
		}
		else
		{
			current_time++;
		}
	}
	print_result("FCFS");
}
void rr_scheduling()
{
	reset_processes();
	int current_time=0;
	int completed=0;
	int queue[MAX_PROCESS*10];
	int front=0,rear=0;
	bool in_queue[MAX_PROCESS]={false};
	for(int i=0;i<n;i++)
	{
		if(processes[i].arrival_time<=current_time&&processes[i].remaining_time>0)
		{
			queue[rear++]=i;
			in_queue[i]=true;
			if(processes[i].start_time==-1)
			{
				processes[i].start_time=current_time;
			}
		}
	}
	while(completed<n)
	{
		if(front<rear)
		{
			int current_process=queue[front++];
			processes[current_process].remaining_time--;
			current_time++;
			for(int i=0;i<n;i++)
			{
				if(!in_queue[i]&&processes[i].arrival_time<=current_time&&processes[i].remaining_time>0)
				{
					queue[rear++]=i;
					in_queue[i]=true;
					if(processes[i].start_time==-1)
					{
						processes[i].start_time=current_time;
					}
				}
			}
			if(processes[current_process].remaining_time>0)
			{
				queue[rear++]=current_process;
			}
			else
			{
				processes[current_process].finish_time=current_time;
				processes[current_process].turnaround_time=processes[current_process].finish_time-processes[current_process].arrival_time;
				processes[current_process].weighted_turnaround=(float)processes[current_process].turnaround_time/processes[current_process].service_time;
				completed++;
				in_queue[current_process]=false;
			}

		}
		else
		{
			current_time++;
			for(int i=0;i<n;i++)
			{
				if(!in_queue[i]&&processes[i].arrival_time<=current_time&&processes[i].remaining_time>0)
				{
					queue[rear++]=i;
					in_queue[i]=true;
					if(processes[i].start_time==-1)
					{
						processes[i].start_time=current_time;
					}
					break;
				}
			}
		}
	}
	print_result("RR(q=1)");
}
void spn_scheduling()
{
	reset_processes();
	int current_time=0;
	int completed=0;
	while(completed<n)
	{
		int next_process=-1;
		int min_service=1000;
		for(int i=0;i<n;i++)
		{
			if(processes[i].arrival_time<=current_time&&processes[i].start_time==-1&&processes[i].service_time<min_service)
			{
				min_service=processes[i].service_time;
				next_process=i;
			}
		}
		if(next_process!=-1)
		{
			processes[next_process].start_time=current_time;
			processes[next_process].finish_time=current_time+processes[next_process].service_time;
			processes[next_process].turnaround_time=processes[next_process].finish_time-processes[next_process].arrival_time;
			processes[next_process].weighted_turnaround=(float)processes[next_process].turnaround_time/processes[next_process].service_time;
			current_time=processes[next_process].finish_time;
			completed++;
		}
		else
		{
			current_time++;
		}
	}
	print_result("SPN");
}
void srt_scheduling()
{
	reset_processes();
	int current_time=0;
	int completed=0;
	int current_process=-1;
	while(completed<n)
	{
		int next_process=-1;
		int min_remaining=1000;
		for(int i=0;i<n;i++)
		{
			if(processes[i].arrival_time<=current_time&&processes[i].remaining_time>0&&processes[i].remaining_time<=min_remaining)
			{
				min_remaining=processes[i].remaining_time;
				next_process=i;
			}
		}
		if(next_process!=-1)
		{
			if(processes[next_process].start_time==-1)
			{
				processes[next_process].start_time=current_time;
			}
			processes[next_process].remaining_time--;
			current_time++;
			if(processes[next_process].remaining_time==0)
			{
				processes[next_process].finish_time=current_time;
				processes[next_process].turnaround_time=processes[next_process].finish_time-processes[next_process].arrival_time;
				processes[next_process].weighted_turnaround=(float)processes[next_process].turnaround_time/processes[next_process].service_time;
				completed++;
			}
		}
		else
		{
			current_time++;
		}
	}
	print_result("SRT");
}
void hrrn_scheduling()
{
	reset_processes();
	int current_time=0;
	int completed=0;
	while(completed<n)
	{
		for(int i=0;i<n;i++)
		{
			if(processes[i].arrival_time<=current_time&&processes[i].start_time==-1)
			{
				processes[i].waiting_time=current_time-processes[i].arrival_time;
				processes[i].response_ratio=(float)(processes[i].waiting_time+processes[i].service_time)/processes[i].service_time;
			}
		}
		int next_process=-1;
		float max_response_ratio=-1;
		for(int i=0;i<n;i++)
		{
			if(processes[i].arrival_time<=current_time&&processes[i].start_time==-1&&processes[i].response_ratio>max_response_ratio)
			{
				max_response_ratio=processes[i].response_ratio;
				next_process=i;
			}
		}
		if(next_process!=-1)
		{
			processes[next_process].start_time=current_time;
			processes[next_process].finish_time=current_time+processes[next_process].service_time;
			processes[next_process].turnaround_time=processes[next_process].finish_time-processes[next_process].arrival_time;
			processes[next_process].weighted_turnaround=(float)processes[next_process].turnaround_time/processes[next_process].service_time;
			current_time=processes[next_process].finish_time;
			completed++;
		}
		else
		{
			current_time++;
		}
	}
	print_result("HRRN");
}

int main()
{
	initialize_processes();
	fcfs_scheduling();
	rr_scheduling();
	spn_scheduling();
	srt_scheduling();
	hrrn_scheduling();
	return 0;
}
