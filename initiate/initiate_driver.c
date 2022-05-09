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
            break;
        } else {
            puts("Not Registered, trying again\n");
            sleep(1);
            continue;
        }

    }

    while(true) {
        puts("Checking tasks\n");
        check_tasks();
        sleep(5);
    }
        
}