#include <stdio.h>     
#include <unistd.h>  
#include <sys/types.h>  
#include <stdlib.h>  
#include <sys/wait.h>    

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

        long counter = 0;  
        long cycle = 0;    // Cycle counter
        int status;
        pid_t result;

        while (1) {
            // Check if Process 2 finished
            result = waitpid(pid, &status, WNOHANG);

            if (result > 0) {
                // Process 2 finished
                printf("\nProcess 1: Process 2 (PID %d) has finished!\n", pid);
                if (WIFEXITED(status)) {
                    printf("Process 1: Process 2 exited with status %d\n", WEXITSTATUS(status));
                }
                printf("Process 1: Ending now...\n");
                break;
            } else if (result < 0) {
                perror("waitpid failed");
                break;
            }

            // Increment counter and print only multiples of 3
            if (counter % 3 == 0) {
                printf("Cycle number: %ld — %ld is a multiple of 3\n", cycle, counter);
            } else {
                printf("Cycle number: %ld\n", cycle);
            }

            counter++;
            cycle++;
            sleep(0.5);   // 0.5 seconds delay
        }
    }

    return 0;
}
