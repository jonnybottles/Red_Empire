#include <stdio.h>
#include "utils.h"


int check(int ret_code, const char *msg)
{
    if (ret_code != 0) {
        perror(msg);
        return ret_code;
    }
    return 0;
}