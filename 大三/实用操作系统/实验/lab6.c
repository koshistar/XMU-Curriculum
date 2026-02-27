#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#define MAX_SEQUENCE 100
#define MAX_FRAMES 10

typedef struct 
{	
	int pages[MAX_SEQUENCE];
	int length;
}PageSequence;
typedef struct
{
	int page_number;
	int time_in_memory;
	int last_used;
	bool is_valid;
}PageFrame;

void init_page_sequence(PageSequence* seq)
{
	char input[MAX_SEQUENCE*10];
	printf("Input sequence: ");
	//getchar();
	fgets(input,sizeof(input),stdin);
	char* token=strtok(input,",");
	int count=0;
	while(token!=NULL&&count<MAX_SEQUENCE)
	{
		seq->pages[count++]=atoi(token);
		token=strtok(NULL,",");
	}
	seq->length=count;
}
void init_page_frames(PageFrame* frames,int num_frames)
{
	for(int i=0;i<num_frames;i++)
	{
		frames[i].page_number=-1;
		frames[i].time_in_memory=0;
		frames[i].last_used=-1;
		frames[i].is_valid=false;
	}
}
int find_page_in_memory(PageFrame* frames,int num_frames,int page)
{
	for(int i=0;i<num_frames;i++)
	{
		if(frames[i].is_valid&&frames[i].page_number==page)
		{
			return i;
		}
	}
	return -1;
}
int find_free_frame(PageFrame* frames,int num_frames)
{
	for(int i=0;i<num_frames;i++)
	{
		if(!frames[i].is_valid)
		{
			return i;
		}
	}
	return -1;
}
void print_memory_state(PageFrame* frames,int num_frames)
{
	printf(" Memory state: [");
	for(int i=0;i<num_frames;i++)
	{
		if(frames[i].is_valid)
		{
			printf("%d",frames[i].page_number);
		}
		else
		{
			printf("-");
		}
		if(i<num_frames-1)
		{
			printf(", ");
		}
	}
	printf("]\n");
}
void opt(PageSequence* seq,int num_frames)
{
	PageFrame frames[MAX_FRAMES];
	init_page_frames(frames,num_frames);
	int page_faults=0;
	int hits=0;
	for(int i=0;i<seq->length;i++)
	{
		int current_page=seq->pages[i];
		printf("\nPage %d: ",current_page);
		int frame_index=find_page_in_memory(frames,num_frames,current_page);
		if(frame_index!=-1)
		{
			printf("hit");
			hits++;
		}
		else
		{
			printf("fault");
			page_faults++;

			int free_frame=find_free_frame(frames,num_frames);
			if(free_frame!=-1)
			{
				frames[free_frame].page_number=current_page;
				frames[free_frame].is_valid=true;
				printf(" load into free frame %d",free_frame);
			}
			else
			{
				int victim_index=-1;
				int farthest_use=-1;

				for(int j=0;j<num_frames;j++)
				{
					int next_use=-1;
					for(int k=i+1;k<seq->length;k++)
					{
						if(seq->pages[k]==frames[j].page_number)
						{
							next_use=k;
							break;
						}
					}
					if(next_use==-1)
					{
						victim_index=j;
						break;
					}
					else if(next_use>farthest_use)
					{
						farthest_use=next_use;
						victim_index=j;
					}
				}
				printf(" Replace page %d in frame %d",frames[victim_index].page_number,victim_index);
				frames[victim_index].page_number=current_page;
			}
		}
		print_memory_state(frames,num_frames);
	}
	printf("Total visit times: %d\n",seq->length);
	printf("fault times: %d\n",page_faults);
	printf("hit times: %d\n",hits);
	printf("fault rate: %.2f%%\n",(float)page_faults/seq->length*100);
	printf("hit rate: %.2f%%\n",(float)hits/seq->length*100);
}
void fifo(PageSequence* seq,int num_frames)
{
	PageFrame frames[MAX_FRAMES];
	init_page_frames(frames,num_frames);
	int page_faults=0;
	int hits=0;
	int pointer=0;

	for(int i=0;i<seq->length;i++)
	{
		int current_page=seq->pages[i];
		printf("\nvisit page %d: ",current_page);
		int frame_index=find_page_in_memory(frames,num_frames,current_page);
		if(frame_index!=-1)
		{
			printf("hits");
			hits++;
		}
		else
		{
			printf("fault");
			page_faults++;

			int free_frame=find_free_frame(frames,num_frames);
			if(free_frame!=-1)
			{
				frames[free_frame].page_number=current_page;
				frames[free_frame].is_valid=true;
				frames[free_frame].time_in_memory=i;
				printf(" load free frame %d",free_frame);
			}
			else
			{
				int oldest_index=0;
				int oldest_time=frames[0].time_in_memory;
				for(int j=1;j<num_frames;j++)
				{
					if(frames[i].time_in_memory<oldest_time)
					{
						oldest_time=frames[j].time_in_memory;
						oldest_index=j;
					}
				}
				printf(" replace page %d in frame %d",frames[oldest_index].page_number,oldest_index);
				frames[oldest_index].page_number=current_page;
				frames[oldest_index].time_in_memory=i;
			}
		}
		print_memory_state(frames,num_frames);
	}
	printf("\ntotal visit times: %d\n",seq->length);
	printf("fault times: %d\n",page_faults);
	printf("hit times: %d\n",hits);
	printf("fault rate: %.2f%%\n",(float)page_faults/seq->length*100);
	printf("hit rate: %.2f%%\n",(float)hits/seq->length*100);
}
void lru(PageSequence* seq,int num_frames)
{
	PageFrame frames[MAX_FRAMES];
	init_page_frames(frames,num_frames);
	int page_faults=0;
	int hits=0;
	for(int i=0;i<seq->length;i++)
	{
		int current_page=seq->pages[i];
		printf("\nvisit page %d: ",current_page);
		int frame_index=find_page_in_memory(frames,num_frames,current_page);
		if(frame_index!=-1)
		{
			frames[frame_index].last_used=i;
			printf("hit");
			hits++;
		}
		else
		{
			printf("fault");
			page_faults++;
			int free_frame=find_free_frame(frames,num_frames);
			if(free_frame!=-1)
			{
				frames[free_frame].page_number=current_page;
				frames[free_frame].is_valid=true;
				frames[free_frame].last_used=i;
				printf(" load free frame %d",free_frame);
			}
			else
			{
				int lru_index=0;
				int lru_time=frames[0].last_used;
				for(int j=1;j<num_frames;j++)
				{
					if(frames[j].last_used<lru_time)
					{
						lru_time=frames[j].last_used;
						lru_index=j;
					}
				}
				printf(" replace page %d in frame %d",frames[lru_index].page_number,lru_index);
				frames[lru_index].page_number=current_page;
				frames[lru_index].last_used=i;
			}
		}
		print_memory_state(frames,num_frames);
	}
	printf("\ntotal visit times: %d\n",seq->length);
	printf("fault times: %d\n",page_faults);
	printf("hit times: %d\n",hits);
	printf("fault rate: %.2f%%\n",(float)page_faults/seq->length*100);
	printf("hit rate: %.2f%%\n",(float)hits/seq->length*100);
}

int main()
{
	PageSequence page_seq;
	int num_frames=3;
	init_page_sequence(&page_seq);
	printf("\nframes num(1-%d): ",MAX_FRAMES);
	scanf("%d",&num_frames);
	if(num_frames<1||num_frames>MAX_FRAMES)
	{
		printf("\ninput error\n");
		return 1;
	}
	opt(&page_seq,num_frames);
	fifo(&page_seq,num_frames);
	lru(&page_seq,num_frames);
	return 0;
}
