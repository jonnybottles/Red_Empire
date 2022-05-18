#include <stdbool.h>
#include <stdio.h>

#include "initiate.h"
#include "host_utils.h"

// Checks to see if a command exits on host. This should be* portable across
// all versions of Linux
// Ref: https://stackoverflow.com/questions/41230547/check-if-program-is-
// installed-in-c
bool can_run_cmd(const char *cmd) 
{
    if(strchr(cmd, '/')) {
        // if cmd includes a slash, no path search must be performed,
        // go straight to checking if it's executable
        return access(cmd, X_OK)==0;
    }
    const char *path = getenv("PATH");
    if(!path) return false; // something is horribly wrong...
    // we are sure we won't need a buffer any longer
    char *buf = malloc(strlen(path)+strlen(cmd)+3);
    if(!buf) return false; // actually useless, see comment
    // loop as long as we have stuff to examine in path
    for(; *path; ++path) {
        // start from the beginning of the buffer
        char *p = buf;
        // copy in buf the current path element
        for(; *path && *path!=':'; ++path,++p) {
            *p = *path;
        }
        // empty path entries are treated like "."
        if(p==buf) *p++='.';
        // slash and command name
        if(p[-1]!='/') *p++='/';
        strcpy(p, cmd);
        // check if we can execute it
        if(access(buf, X_OK)==0) {
            free(buf);
            return true;
        }
        // quit at last cycle
        if(!*path) break;
    }
    // not found
    free(buf);
    return false;
}

// char * parse_uuid(char *resp)
// {   
//     char *uuid = malloc(sizeof(*uuid) * 37);
//     if(!uuid) {
// 		perror("Unable to create space for uuid.\n");
// 		free(uuid);
// 		return NULL;
//     }

//     uuid = strchr(resp, '@');
//     puts("made it past strch\n");
//     printf("UUID is: %s", uuid +1);
    
//     return uuid;
// }





