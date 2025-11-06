#include <stdio.h>      
#include <unistd.h>     
#include <sys/types.h>  
#include <stdlib.h>   

int main(void) {
    printf("Process 2 (child, PID %d) started\n", getpid());
    long cycle_counter = 0;
    
    while (1) {
        // Check if cycle is a multiple of 3
        if (cycle_counter % 3 == 0) {
            printf("Process 2 - Cycle number: %ld - %ld is a multiple of 3\n", cycle_counter, cycle_counter);
        } else {
            printf("Process 2 - Cycle number: %ld\n", cycle_counter);
        }

        // Check if reached the exit condition
        if (cycle_counter <= -500) {
            printf("\nProcess 2: Reached %ld (lower than -500)\n", cycle_counter);
            printf("Process 2: Exiting now...\n");
            exit(0);  // Exit with status 0 (success)
        }

        cycle_counter--;
        usleep(500000);   // sleep 0.5s to slow output
    }

    return 0;
}
