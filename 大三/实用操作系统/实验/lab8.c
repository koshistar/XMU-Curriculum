#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>//????why no ceil
#define DISK_BLOCKS 500
#define BLOCK_SIZE 2
#define MAX_FILES 55

typedef struct DiskBlock
{
	int block_id;
	int file_id;
	struct DiskBlock* next;
}DiskBlock;
typedef struct File
{
	int file_id;
	char name[20];
	double size_kb;
	int blocks_needed;
	DiskBlock* first_block;
	struct File* next;
}File;
DiskBlock* free_list =NULL;
File* file_list=NULL;
int total_free_blocks=DISK_BLOCKS;
void initialize_disk()
{
	printf("total disk blocks: %d, per block size: %dKB\n",DISK_BLOCKS,BLOCK_SIZE);
	for(int i=DISK_BLOCKS;i>=1;i--)
	{
		DiskBlock* new_block=(DiskBlock*)malloc(sizeof(DiskBlock));
		new_block->block_id=i;
		new_block->file_id=0;
		new_block->next=free_list;
		free_list=new_block;
	}
	total_free_blocks=DISK_BLOCKS;
	printf("free blocks: %d",total_free_blocks);
}
void display_free_list()
{
	printf("\ncurrent free blocks list: ");
	DiskBlock* current=free_list;
	int count=0;
	while(current!=NULL&&count<20)
	{
		printf("%d ",current->block_id);
		current=current->next;
		count++;
	}
	if(current!=NULL)
	{
		printf("\ttotal blocks: %d\n",total_free_blocks);
	}
	else
	{
		printf("\n");
	}
}
int calculate_blocks_needed(double size_kb)
{
	const int PRECISION=1000;
	long long size_int=(long long)(size_kb*PRECISION);
	long long block_int=(long long)(BLOCK_SIZE*PRECISION);
	return (int)((size_int+block_int-1)/block_int);
}
DiskBlock* allocate_blocks(int blocks_needed)
{
	if(blocks_needed>total_free_blocks)
	{
		printf("free blocks is not enough.");
		return NULL;
	}
	DiskBlock* allocated_chain=NULL;
	DiskBlock* last_allocated=NULL;
	DiskBlock* rollback_chain=NULL;
	for(int i=0;i<blocks_needed;i++)
	{
		if(free_list==NULL)
		{
			printf("Error: free blocks list is NULL");
			DiskBlock* rb_current=rollback_chain;
			DiskBlock* rb_last=NULL;
			while(rb_current!=NULL)
			{
				rb_current->file_id=0;
				rb_last=rb_current;
				rb_current=rb_current->next;
			}
			if(rb_last!=NULL)
			{
				rb_last->next=free_list;
				free_list=rollback_chain;
			}
			total_free_blocks+=i;
			return NULL;
		}
		DiskBlock* allocated_block=free_list;
		free_list=free_list->next;
		total_free_blocks--;
		allocated_block->next=NULL;
		if(allocated_chain==NULL)
		{
			allocated_chain=allocated_block;
			last_allocated=allocated_block;
		}
		else
		{
			last_allocated->next=allocated_block;
			last_allocated=allocated_block;
		}
		if(rollback_chain==NULL)
		{
			rollback_chain=allocated_block;
		}
	}
	return allocated_chain;
}
File* create_file(int file_id,const char* name,double size_kb)
{
	int blocks_needed=calculate_blocks_needed(size_kb);
	if(blocks_needed>total_free_blocks)
	{
		printf("Error: can't create file %s, free blocks is not enough\n",name);
		return NULL;
	}
	DiskBlock* allocated_blocks=allocate_blocks(blocks_needed);
	if(allocated_blocks==NULL)
	{
		return NULL;
	}
	File* new_file=(File*)malloc(sizeof(File));
	new_file->file_id=file_id;
	strncpy(new_file->name,name,sizeof(new_file->name)-1);
	new_file->name[sizeof(new_file->name)-1]='\0';
	new_file->size_kb=size_kb;
	new_file->blocks_needed=blocks_needed;
	new_file->first_block=allocated_blocks;
	new_file->next=NULL;
	DiskBlock* current_block=allocated_blocks;
	while(current_block!=NULL)
	{
		current_block->file_id=file_id;
		current_block=current_block->next;
	}
	if(file_list==NULL)
	{
		file_list=new_file;
	}
	else
	{
		File* current=file_list;
		while(current->next!=NULL)
		{
			current=current->next;
		}
		current->next=new_file;
	}
	printf("create file: %s(size: %.1fKB, blocks num: %d)\n",name,size_kb,blocks_needed);
	return new_file;
}
int blocks_in_chain(DiskBlock* block_chain)
{
	int count=0;
	DiskBlock* current=block_chain;
	while(current!=NULL)
	{
		count++;
		current=current->next;
	}
	return count;
}
void release_blocks(DiskBlock* block_chain)
{
	if(block_chain==NULL)
	{
		return;
	}
	DiskBlock* current=block_chain;
	DiskBlock* last=NULL;
	while(current!=NULL)
	{
		current->file_id=0;
		last=current;
		current=current->next;
	}
	int blocks=blocks_in_chain(block_chain);
	if(last!=NULL)
	{
		last->next=free_list;
		free_list=block_chain;
		total_free_blocks+=blocks;
	}
}
void delete_file_by_name(const char* name)
{
	File* prev=NULL;
	File* current=file_list;
	while(current!=NULL)
	{
		if(strcmp(current->name,name)==0)
		{
			release_blocks(current->first_block);
			if(prev==NULL)
			{
				file_list=current->next;
			}
			else
			{
				prev->next=current->next;
			}
			printf("delete file: %s(release %d block)\n",name,current->blocks_needed);
			free(current);
			return;
		}
		prev=current;
		current=current->next;
	}
	printf("Warning: can't find file %s\n",name);
}
void display_file_allocation(const char* name)
{
	File* current=file_list;
	while(current!=NULL)
	{
		if(strcmp(current->name,name)==0)
		{
			printf("\nfile %s allocation:\n",name);
			printf("file size: %.1fKB\n blocks num:%d\n allocate block: ",current->size_kb,current->blocks_needed);
			DiskBlock* block=current->first_block;
			while(block!=NULL)
			{
				printf("%d",block->block_id);
				if(block->next!=NULL)
				{
					printf(" -> ");
				}
				block=block->next;
			}
			printf("\n");
			return;
		}
		current=current->next;
	}
	printf("can't find file %s\n",name);
}
int generate_random_size()
{
	return (rand()%9)+2;
}
void create_random_files()
{
	srand(time(NULL));
	for(int i=1;i<=50;i++)
	{
		char filename[20];
		sprintf(filename,"%d.txt",i);
		int file_size=generate_random_size();
		create_file(i,filename,file_size);
	}
	display_free_list();
}
void delete_odd_files()
{
	for(int i=1;i<=50;i+=2)
	{
		char filename[20];
		sprintf(filename,"%d.txt",i);
		delete_file_by_name(filename);
	}
	display_free_list();
}
void create_new_files()
{
	File* fileA=create_file(51,"A.txt",7);
	File* fileB=create_file(52,"B.txt",5);
	File* fileC=create_file(53,"C.txt",2);
	File* fileD=create_file(54,"D.txt",9);
	File* fileE=create_file(55,"E.txt",3.5);
	display_free_list();
}
void display_all_files_allocation()
{
	File* current=file_list;
	int file_count=0;
	while(current!=NULL)
	{
		display_file_allocation(current->name);
		current=current->next;
		file_count++;
	}
	printf("\ntotal file nums: %d",file_count);
}
void display_disk_block_status()
{
	printf("total block nums: %d\nfree block nums: %d\nused block nums: %d\nfree rate: %.2f%%\n",DISK_BLOCKS,total_free_blocks,DISK_BLOCKS-total_free_blocks,(double)total_free_blocks/DISK_BLOCKS*100);
}
void cleanup()
{
	File* file_current=file_list;
	while(file_current!=NULL)
	{
		File* next_file=file_current->next;
		DiskBlock* block_current=file_current->first_block;
		while(block_current!=NULL)
		{
			DiskBlock* next_block=block_current->next;
			free(block_current);
			block_current=next_block;
		}
		free(file_current);
		file_current=next_file;
	}
	DiskBlock* block_current=free_list;
	while(block_current!=NULL)
	{
		DiskBlock* next_block=block_current->next;
		free(block_current);
		block_current=next_block;
	}
	free_list=NULL;
	file_list=NULL;
	total_free_blocks=0;
}
int main()
{
	initialize_disk();
	create_random_files();
	delete_odd_files();
	create_new_files();
	display_file_allocation("A.txt");
	display_file_allocation("B.txt");
	display_file_allocation("C.txt");
	display_file_allocation("D.txt");
	display_file_allocation("E.txt");
	//display_all_files_allocation();
	display_disk_block_status();
	cleanup();
	return 0;
}

