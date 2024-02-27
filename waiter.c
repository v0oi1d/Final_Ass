//waiter.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_SIZE 1024
#define MENU_FILE "menu.txt"



int main()
{
    char menuLine[100];
    FILE *menuFile = fopen(MENU_FILE, "r");
    if (menuFile == NULL)
    {
        printf("Menu file not found!\n");
        exit(EXIT_FAILURE);
    }
    //extracting prices from the menu
    int countLines = 0;
    int *prices=NULL;
    
    while (fgets(menuLine, sizeof(menuLine), menuFile) != NULL)
    {
        countLines++;
        //extracting prices and storing in an array
        char *token = strtok(menuLine, " ");
        int numWords = 0;
        char *prevWord = token;
        while(token!=NULL){
            if(token[0]=='I' && token[1]=='N'&& token[2]=='R') break;
            numWords++;
            prevWord = token;
            token = strtok(NULL, " ");    
        }
        int price = atof(prevWord);
        prices = (int *)realloc(prices, (countLines) * sizeof(int));
        if (prices == NULL) {
                printf("Memory allocation failed\n");
                return 1;
            }
        prices[countLines-1] = price;
    }

    int shm_fd;
    void *ptr;
    int waiterID;

    printf("Enter Waiter ID: ");
    scanf("%d", &waiterID);

    // Open the shared memory segment
    key_t shared = ftok("waiter.c", waiterID);
    int shmid = shmget(shared, 1024, 0666 | IPC_CREAT); // Printing the orders
    int *proj;
    proj =shmat(shmid, NULL, 0);
    printf("Waiter Process for Waiter ID: %d\n", waiterID);
    key_t hotelshared=ftok("hotelmanager.c", waiterID);
    int hotelshmid=shmget(hotelshared, 1024, 0666 | IPC_CREAT);
    int *hotelproj;
    hotelproj=shmat(hotelshmid,NULL,0);
    
    proj[1]=0;
    int flag=1;
    while(flag)
    {
    	while(1)
	   {
	    	if(proj[3]==1)
	    	{	
	    		
	    		 //calculating curr bill
		    proj[2]=0;
		    for(int i = 0; i<proj[0];i++){
			int priceIndex = proj[10+i]-1;
			proj[2]+=prices[priceIndex];
		    }
		    printf("Bill is %d\n", proj[2]);
		    printf("\n");
		    proj[1]+=proj[2];
		    hotelproj[2]=proj[2];
		    hotelproj[3]=1;
		    sleep(3);
		    break;
	    	}
	    	if(proj[4]==1)
	    	{
	    		flag=0;
	    		hotelproj[4]=1;
	    		break;
	    	}
	    }
    }
    
    fclose(menuFile);
    exit(EXIT_SUCCESS);

    return 0;
}
