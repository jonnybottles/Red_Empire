#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>



#ifndef INITIATE_H
#define INITIATE_H

bool reg(void);

bool check_tasks(void);

size_t is_registered(char *buffer, size_t itemsize, size_t nitems, void* ignorethis);

char *get_tasks(char *buffer, size_t itemsize, size_t nitems, void* ignorethis);

// static size_t mem_cb(void *contents, size_t size, size_t nmemb, void *userp);

void add_curl_field(curl_mime *form, const char *name, const char *data);

#endif