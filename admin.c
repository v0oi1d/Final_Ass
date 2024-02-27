#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define SHM_KEY 12345

int main()
{
    // Shared memory setup
    key_t key = ftok("admin.c", SHM_KEY);
    int shmid = shmget(key, sizeof(char), 0666 | IPC_CREAT);
    int *shm_ptr ;
    shm_ptr = shmat(shmid, NULL, 0);
    shm_ptr[0]=-2;
    if (shmid == -1 || shm_ptr == (void *)-1)
    {
        perror("Failed to create shared memory");
        exit(1);
    }

    
    
    // Main admin loop
    while (1)
    {
        printf("Do you want to close the hotel? Enter Y for Yes and N for No: ");
        char input;
        scanf(" %c", &input);

        if (input == 'Y' || input == 'y')
        {
            // Inform hotel manager to close the hotel
            shm_ptr[0] = 1;
            printf("Hotel closure requested. Admin process terminating.\n");
            break;
        }
        else if (input == 'N' || input == 'n')
        {
            // Continue running as usual
            printf("Admin process continuing.\n");
        }
        else
        {
            printf("Invalid input. Please enter Y or N.\n");
        }
    }

    // Cleanup shared memory
    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}

