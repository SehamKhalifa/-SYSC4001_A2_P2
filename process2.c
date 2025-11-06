#include <stdio.h>      
#include <unistd.h>     
#include <sys/types.h>  
#include <stdlib.h>   

int main(void) {
    printf("Process 2 (child, PID %d) started\n", getpid());
    long cycle = 0;
    
    while (1) {
        // Check if cycle is a multiple of 3
        if (cycle % 3 == 0) {
            printf("Process 2 - Cycle number: %ld - %ld is a multiple of 3\n", cycle, cycle);
        } else {
            printf("Process 2 - Cycle number: %ld\n", cycle);
        }

        // Check if reached the exit condition
        if (cycle <= -500) {
            printf("\nProcess 2: Reached %ld (lower than -500)\n", cycle);
            printf("Process 2: Exiting now...\n");
            exit(0);  // Exit with status 0 (success)
        }

        cycle--;
        usleep(500000);   // sleep 0.5s to slow output
    }

    return 0;
}