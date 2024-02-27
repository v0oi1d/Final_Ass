//hotel_manager process
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{
    int noTables;
    printf("Enter the Total Number of Tables at the Hotel:");
    scanf("%d",&noTables);
	FILE *fp; // File pointer
	if (fp == NULL) 
	{
		printf("Error opening file.\n");
		return 1; // Return an error code
    	}
    fp = fopen("earnings.txt", "w");
    // Open the shared memory segment
    key_t key_array[noTables];
    int shmid_array[noTables];
    int * status_array[noTables];
    for(int i = 1 ; i<=noTables;i++)
    {
        key_array[i-1] = ftok("hotelmanager.c",i);
        shmid_array[i - 1] = shmget(key_array[i-1], 1024, 0666 | IPC_CREAT);
        status_array[i - 1] = shmat(shmid_array[i-1], NULL, 0);
    }
    for(int i=0;i<noTables;i++)
    {
    	for(int j=0;j<256;j++)
    	{
    		status_array[i][j]=0;
    	}
    }
    int flag=noTables;
    while(flag)
    {
    	for(int i=1;i<=noTables;i++)
    	{
    		if(status_array[i-1][3]==1)//order finished
    		{
    			//print to earnings.txt
    			fprintf(fp, "Earning from Table %d: %d INR\n", i ,  status_array[i-1][2]);
    			status_array[i-1][1]+=status_array[i-1][2];
    			status_array[i-1][3]=0;
    		}
    		if(status_array[i-1][4]==1)
    		{
    			flag--;
    			if(flag==0)
    				break;
    			status_array[i-1][4]=0;
    		}
    	}
    }
    int totalearnings=0;
    for(int i=1;i<=noTables;i++)
    {
    	totalearnings+=status_array[i-1][1];
    }
    int wages=totalearnings*4;
    wages=wages/10;
    int profit=totalearnings-wages;
    fprintf(fp, "Total Earnings of Hotel: %d INR\n", totalearnings);
    fprintf(fp, "Total Wages of Waiters: %d INR\n", wages);
    fprintf(fp, "Total Profit: %d INR\n", profit);
    printf("Total Earnings of Hotel: %d INR\n", totalearnings);
    printf("Total Wages of Waiters: %d INR\n", wages);
    printf("Total Profit: %d INR\n", profit);
    
}
