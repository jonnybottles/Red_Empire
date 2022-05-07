#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>



#ifndef INITIATE_H
#define INITIATE_H

int reg(void);
void add_curl_field(curl_mime **form, curl_mimepart **field, const char *name, const char *data, const char *msg);

#endif