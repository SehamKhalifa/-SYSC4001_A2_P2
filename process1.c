#include <stdio.h>     
#include <unistd.h>  
#include <sys/types.h>  
#include <stdlib.h>  
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>

typedef struct {
    int mult;
    int count;
} shmem_t;

union semun { // Sometimes necessary for semctl
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

void sema_wait(int semaid){
    struct sembuf sema_buffer = {0, -1, 0};
    if(semop(semaid, &sema_buffer, 1) == -1){
        perror("semop wait failed");
        exit(EXIT_FAILURE);
    }
}

void sema_sig(int semaid){
    struct sembuf sema_buffer = {0, 1, 0};
    if(semop(semaid, &sema_buffer, 1) == -1){
        perror("semop signal failed");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    pid_t pid = fork();   // create new process

    if (pid < 0) {
        // fork failed
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        printf("Process 1 launching Process 2...\n");
        execl("./process2", "process2", NULL); 

        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Process 1 (parent, PID %d) started\n", getpid());
        printf("Waiting for Process 2 (PID %d) to start...\n", pid);
        
        sleep(0.1);  // Give child time to start

        int counter = 0;  
        int status;
        pid_t result;

        key_t key_mem = ftok(".", 'A'); // Generate key
        if (key_mem == -1) {
            perror("mem ftok failed");
            exit(EXIT_FAILURE);
        }

        int shmemid = shmget(key_mem, sizeof(shmem_t), IPC_CREAT | 0666); // Create shared memory space
        if (shmemid == -1) {
            perror("shmget failed");
            exit(EXIT_FAILURE);
        }

        shmem_t *shmem = (shmem_t *) shmat(shmemid, (void *) NULL, 0); // Attach to shared memory
            if (shmem == -1) {
            perror("shmat failed");
            exit(EXIT_FAILURE);
        }

        printf("Process 1: Shared memory successfully initialized\n");

        key_t key_sema = ftok(".", 'B'); // Generate key
        if (key_sema == -1) {
            perror("sema ftok failed");
            exit(EXIT_FAILURE);
        }

        int semaid = semget(key_sema, 1, IPC_CREAT | 0666); // Create semaphore
        if(semaid == -1){
            perror("semget failed");
            exit(EXIT_FAILURE);
        }

        union semun sema_union; // Initialize semaphore to be open
        sema_union.val = 1;
        if (semctl(semaid, 0, SETVAL, sema_union) == -1) {
            perror("semctl SETVAL failed");
            exit(EXIT_FAILURE);
        }

        printf("Process 1: Semaphore successfully initialized\n");
 
        sema_wait(semaid);
        shmem->mult = 5;
        shmem->count = 0;
        sema_sig(semaid);

        while (1) {
            // Check if Process 2 finished
            result = waitpid(pid, &status, WNOHANG);

            if (result > 0) {
                // Process 2 finished
                printf("\nProcess 1: Process 2 (PID %d) has finished!\n", pid);
                if (WIFEXITED(status)) {
                    printf("Process 1: Process 2 exited with status %d\n", WEXITSTATUS(status));
                }
                printf("Process 1: Exiting now...\n");
                break;
            } else if (result < 0) {
                perror("waitpid failed");
                break;
            }

            sema_wait(semaid);
            // Increment counter and print only multiples of mult chosen
            if (counter % shmem->mult == 0) {
                printf("Process No 1 - Cycle number: %d — %d is a multiple of %d\n", counter, counter, shmem->mult);
            } else {
                printf("Process No 1 - Cycle number: %d\n", counter);
            }

            if(counter > 500){
                sema_sig(semaid);
                printf("\nProcess 1: Reached 500\n");
                printf("Process 1: Exiting now...\n");
                exit(0);
            }

            counter++;
            shmem->count = counter;
            sema_sig(semaid);
            usleep(100000);   // 0.5 seconds delay
        }

        sleep(3); // To ensure process 2 finishes first

        shmdt(shmem); // Clean up
        shmctl(shmemid, IPC_RMID, (void*) NULL);
        semctl(semaid, 0, IPC_RMID);
        printf("Process 1: Shared memory and sempahore succesfully cleaned up\n");
    }

    return 0;
}
