#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#define MAX_PROCESS 10
#define MAX_RESOURCE 10
int n,m;
int R[MAX_RESOURCE];
int C[MAX_PROCESS][MAX_RESOURCE];
int A[MAX_PROCESS][MAX_RESOURCE];
int N[MAX_PROCESS][MAX_RESOURCE];
int V[MAX_RESOURCE];

void initialize()
{
	printf("Input sources type nums(m): ");
	scanf("%d",&m);
	printf("Input sources vector(R): ");
	for(int i=0;i<m;i++)
	{
		scanf("%d",&R[i]);
		V[i]=R[i];
	}
	printf("Input process nums(n): ");
	scanf("%d",&n);
	printf("Input process needed matrix(C %d x %d ): ",n,m);
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<m;j++)
		{
			scanf("%d",&C[i][j]);
			A[i][j]=0;
			N[i][j]=C[i][j];
		}
	}
}
void display_state()
{
	printf("R: ");
	for(int i=0;i<m;i++)
	{
		printf("%d ",R[i]);
	}
	printf("\n");
	printf("C: \n");
	for(int i=0;i<n;i++)
	{
		printf("process %d: ",i);
		for(int j=0;j<m;j++)
		{
			printf("%d ",C[i][j]);
		}
		printf("\n");
	}
	printf("N: \n");
	for(int i=0;i<n;i++)
	{
		printf("process %d: ",i);
		for(int j=0;j<m;j++)
		{
			printf("%d ",N[i][j]);
		}
		printf("\n");
	}
}
bool safety_check(int* safe_sequence)
{
	int work[MAX_RESOURCE];
	bool finish[MAX_PROCESS]={false};
	int count=0;
	for(int i=0;i<m;i++)
	{
		work[i]=V[i];
	}
	while(count<n)
	{
		bool found=false;
		for(int i=0;i<n;i++)
		{
			if(!finish[i])
			{
				bool can_allocate=true;
				for(int j=0;j<m;j++)
				{
					if(N[i][j]>work[j])
					{
						can_allocate=false;
						break;
					}
				}
				if(can_allocate)
				{
					for(int j=0;j<m;j++)
					{
						work[j]+=A[i][j];
					}
					finish[i]=true;
					safe_sequence[count++]=i;
					found=true;
				}
			}
		}
		if(!found)
		{
			return false;
		}
	}
	return true;
}
bool try_allocate(int process_id,int request[])
{
	for(int i=0;i<m;i++)
	{
		if(request[i]>N[process_id][i])
		{
			printf("Error: process %d request too much.\n",process_id);
			return false;
		}
	}
	for(int i=0;i<m;i++)
	{
		if(request[i]>V[i])
		{
			printf("Error: process %d request too much.\n",process_id);
			return false;
		}
	}
	
	for(int i=0;i<m;i++)
	{
		V[i]-=request[i];
		A[process_id][i]+=request[i];
		N[process_id][i]-=request[i];
	}
	int safe_sequence[MAX_PROCESS];
	bool is_safe=safety_check(safe_sequence);
	if(!is_safe)
	{
		for(int i=0;i<m;i++)
		{
			V[i]+=request[i];
			A[process_id][i]-=request[i];
			N[process_id][i]+=request[i];
		}
		printf("Refuse allocating: would lead to unsafe condition.");
		return false;
	}
	printf("Allocate success, safe sequence: ");
	for(int i=0;i<n;i++)
	{
		printf("%d ",safe_sequence[i]);
	}
	printf("\n");
	return true;
}
int main()
{
	initialize();
	display_state();
	char input;
	int process_id;
	int request[MAX_RESOURCE];
	while(true)
	{
		printf("\nInput PID(0-%d): ",n-1);
		if(scanf("%d",&process_id)!=1)
		{
			while(getchar()!='\n')
			;
			break;
		}
		if(process_id<0||process_id>=n)
		{
			printf("Error: invalid PID.\n");
			continue;
		}
		printf("Input resource request vector(%d resources): ",m);
		for(int i=0;i<m;i++)
		{
			scanf("%d",&request[i]);
		}
		if(try_allocate(process_id,request))
		{
			printf("A: \n");
			for(int i=0;i<n;i++)
			{
				printf("process %d: ",i);
				for(int j=0;j<m;j++)
				{
					printf("%d ",A[i][j]);
				}
				printf("\n");
			}
			printf("V: ");
			for(int i=0;i<m;i++)
			{
				printf("%d ",V[i]);
			}
			printf("\n");
		}
		getchar();
		while(getchar()!='\n');
	}
	return 0;
}
