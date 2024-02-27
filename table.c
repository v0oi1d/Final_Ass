    //table.c
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <fcntl.h>
    #include <string.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>

    #define MAX_CUSTOMERS 5
    #define MAX_ORDERS (MAX_CUSTOMERS * 10) // Maximum number of orders
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

        printf("Table Process\n");

        int numCustomers;
        int tableNumber;
        int countLines = 0;
        while (fgets(menuLine, sizeof(menuLine), menuFile) != NULL)
        {
            countLines++;
        }

        // Ask for the table number
        printf("Enter Table Number: ");
        printf("\n");
        scanf("%d", &tableNumber);
	key_t shared = ftok("waiter.c", tableNumber);
            int shmid = shmget(shared, 1024, 0666 | IPC_CREAT); // Printing the orders
            int *proj;
            int bill;
            proj = shmat(shmid, NULL, 0);
            for(int i=0;i<256;i++)
            {
            	proj[i]=0;
            }
        while(1){
            // Ask for the number of customers
            int orders[MAX_ORDERS]; // 1D array to store all orders
            int orderIndex = 0;     // Index for the orders array
            printf("Enter Number of Customers at Table (maximum no. of customers can be %d): or enter -1 \n", MAX_CUSTOMERS);
            scanf("%d", &numCustomers);
            if(numCustomers == -1){
            	proj[4]=1;
            	sleep(1);
            	printf("Total bill is : %d\n",proj[1]);
                printf("Thank you for joining our hotel today.\n");
                break;
            }
            
            while(1){
            if(numCustomers == -1){
            	proj[4]=1;
            	sleep(1);
            	printf("Total bill is : %d\n",proj[1]);
                printf("Thank you for joining our hotel today.\n");
                break;} 
            else if(numCustomers < 1 || numCustomers >5)
            {
            printf("Customer number should be between 1 and 5, enter again: \n");
            scanf("%d", &numCustomers);
            continue;
            }
            else
            	break;
            }

            // Display menu
            printf("Menu:\n");
            rewind(menuFile);
            while (fgets(menuLine, sizeof(menuLine), menuFile) != NULL)
            {	
                printf("%s", menuLine);
            }
            // Create pipes for communication with customers
            int customerPipes[numCustomers][2];
            int flag = 0;


            for (int i = 0; i < numCustomers; i++)
            {
                if (pipe(customerPipes[i]) == -1)
                {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }

                pid_t pid = fork();
                if (pid < 0)
                {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0)
                {     
                                         // Child process (customer)
                    close(customerPipes[i][0]); // Close read end
                    int flag = 0;

                    printf("Customer %d: Enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done:\n", i + 1);
                    int item;
                    while (1)
                    {
                        scanf("%d", &item);
                        if (item == -1 || (item >= 1 && item <= countLines))
                        {
                            if (item == -1)
                            {
                                printf("Customer %d has finished ordering.\n", i + 1);
                                break;
                            }
                            else
                            // Store the order
                            write(customerPipes[i][1], &item, sizeof(item));
                        }
                        else
                        {
                            printf("Invalid serial number. Please enter -1 or any number from 1 to %d\n", countLines);
                        }
                    }
                    close(customerPipes[i][1]); // Close write end
                    exit(EXIT_SUCCESS);
                }
                else
                {
                                //Parent process (table)
                    close(customerPipes[i][1]); // Close write end
                    printf("Pipe created with customer %d\n", i + 1);
                    int item;
                    while (read(customerPipes[i][0], &item, sizeof(item)) > 0)
                    {
                        orders[orderIndex++] = item;
                        printf("Received order from Customer %d: Item %d\n", i + 1, item);
                        printf("Continue entering the serial number(s) of the item(s) for Customer %d. Enter -1 when done:\n", i+1);
                    }
                    close(customerPipes[i][0]); // Close read end

                        
                }
            }

            // Wait for all customer processes to finish
            for (int i = 0; i < numCustomers; i++)
            {
                wait(NULL);
            }

            printf("\nOrders: ");
            proj[0] = orderIndex;
            for (int i = 0; i < orderIndex; i++)
            {
                // printf("%d ", orders[i]);
                proj[i+10] = orders[i];
            }  
            proj[3]=1;
            sleep(1);
            printf("\nFinal bill is :%d\n", proj[2]); 
            proj[3]=0;

        }
        shmdt(proj);

        fclose(menuFile);
        return 0;
    }
