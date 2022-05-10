#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include<sys/utsname.h>
#include "initiate.h"

//ref https://curl.se/libcurl/c/postit2.html

static size_t mem_cb(void *contents, size_t size, size_t nmemb, void *userp);

bool reg(void)
{

	CURL *curl;
	CURLcode res;

	curl_mime *form = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";

	if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
		perror("curl_global_init error\n");
		return false;
	}

	curl = curl_easy_init();
	if (!curl) {
		perror("curl_easy_init error.\n");
		return false;

	} else {


		/* Create the form */
		// returns null if failure.
		form = curl_mime_init(curl);
		if (!form) {
			perror("curl_mime_init error\n");
			return false;
		}
		// Get target hostname.
		// TO DO figure out how to import HOST_NAME_MAX
		char hostbuf[256];
		if (gethostname(hostbuf, sizeof(hostbuf)) == -1) {
			perror("Error acquiring host name.\n");
			return false;
		}

		add_curl_field(form, "hostname", hostbuf);

		// Get target OS and version and pass to add_curl_field.
		struct utsname buf1;
		errno = 0;
		if (uname(&buf1) != 0) {
			perror("uname error\n");
			return false;
		}

		add_curl_field(form, "os type", buf1.nodename);
		add_curl_field(form, "os version", buf1.version);

		// Add submit options to curl field data.
		add_curl_field(form, "submit", "send");

		// initialize custom header list
		headerlist = curl_slist_append(headerlist, buf);
		if (!headerlist) {
			perror("curl_slist_append error\n");
			return false;
		}
		//Registration URL
		const char regurl[19] = "127.0.0.1:9000/reg";

		curl_easy_setopt(curl, CURLOPT_URL, regurl);

		curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

		struct response chunk = {.memory = malloc(0), .size = 0};

		// Send data to this function as opposed to writing to stdout.
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mem_cb);

		// Pass chunk to callback function.
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

  		printf("The data returning from the function is %s\n", chunk.memory);

		// Perform the request, res will get the return code
		res = curl_easy_perform(curl);

		// Check for errors
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
			return false;
		}


		// Always cleanup.
		curl_easy_cleanup(curl);

		free(chunk.memory);

		// Then cleanup the form.
		curl_mime_free(form);

		// Free slist.
		curl_slist_free_all(headerlist);

		return true;
	}
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

// This function is used to reduce the amounts of curl_mime_addpart,
// curl_mine_name, and curl_mime_data calls.
void add_curl_field(curl_mime * form, const char *name, const char *data)
{
	// Begin code to set options for posting hostname / os to /reg.
	curl_mimepart *field = curl_mime_addpart(form);
	if (!field) {
		perror("curl_mime_addpart error\n");
	}

	if (curl_mime_name(field, name) != CURLE_OK) {
		fprintf(stderr, "error mime name%s\n", name);
	}

	if (curl_mime_data(field, data, CURL_ZERO_TERMINATED) != CURLE_OK) {
		fprintf(stderr, "error mime data%s\n", name);
	}

}

bool check_tasks(void)
{
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
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_tasks);

	// set options

	// perform our action
	// curl_easy_perform accepts the options set above and in this case will
	// download the content from the URL.
	// CURLcode resul saves the return code from curl_easy_perform.
	CURLcode result = curl_easy_perform(curl);

	if (result != CURLE_OK) {
		fprintf(stderr, "download problem: %s\n",
			curl_easy_strerror(result));
	}

	curl_easy_cleanup(curl);
	return 0;
}

// // The base of this code is in the "got_data" function in js_libcurl.c
// size_t is_registered(char *buffer, size_t itemsize, size_t nitems,
// 		     void *ignorethis)
// {
// 	// This computes the number of bytes that was received in the response body.
// 	size_t bytes = itemsize * nitems;
// 	int linenumber = 1;
// 	const char *resp_code = "201";
// 	strstr(buffer, resp_code);

// 	printf("New chunk(%zu)\n", bytes);
// 	printf("%d:\t", linenumber);
// 	printf("%s\n", buffer);
// 	// for (int i = 0; i < bytes; i++) {
// 	//     printf("%c", buffer[i]);
// 	//     if (buffer[i] == '\n') {
// 	//         linenumber++;
// 	//         // int this case each line number resets after each chunk of data.
// 	//         printf("%d:\t", linenumber);
// 	//     }
// 	// }
// 	// This adds some separation between each chunk of data.
// 	printf("\n\n");
// 	return bytes;
// }

// The base of this code is in the "got_data" function in js_libcurl.c
size_t get_tasks(char *buffer, size_t itemsize, size_t nitems, void *ignorethis)
{
	// Cast to void to get rid of compiler warnings.
	(void)ignorethis;

	// This computes the number of bytes that was received in the response body.
	size_t bytes = itemsize * nitems;
	int linenumber = 1;

	printf("New chunk(%zu)\n", bytes);
	printf("%d:\t", linenumber);

	for (size_t i = 0; i < bytes; i++) {
		printf("%c", buffer[i]);
		if (buffer[i] == '\n') {
			linenumber++;
			// int this case each line number resets after each chunk of data.
			printf("%d:\t", linenumber);
		}
	}
	// This adds some separation between each chunk of data.
	printf("\n\n");
	return bytes;
}

void execute_tasks(void)
{
	const char *cmd = "pwd";
	FILE *cmd_results = NULL;
	int cmd_ret = 0;

	if(!can_run_command(cmd)) {
		puts("Command does not exist\n");
	} else {
		puts("Command exists\n");
		if ((cmd_results = popen(cmd, "r")) != NULL) {
			while (fgets(buf, BUFSIZ, cmd_results) != NULL) {
				(void) printf("%s", buf);
				cmd_ret = pclose(cmd_results);
				printf("The exit status is: %d\n", WEXITSTATUS(cmd_results));
			}

		}

	}
}

// Checks to see if a command exits on host.
// Ref: https://stackoverflow.com/questions/41230547/check-if-program-is-
// installed-in-c
bool can_run_command(const char *cmd) 
{
    if(strchr(cmd, '/')) {
        // if cmd includes a slash, no path search must be performed,
        // go straight to checking if it's executable
        return access(cmd, X_OK)==0;
    }
    const char *path = getenv("PATH");
    if(!path) return false; // something is horribly wrong...
    // we are sure we won't need a buffer any longer
    char *buf = malloc(strlen(path)+strlen(cmd)+3);
    if(!buf) return false; // actually useless, see comment
    // loop as long as we have stuff to examine in path
    for(; *path; ++path) {
        // start from the beginning of the buffer
        char *p = buf;
        // copy in buf the current path element
        for(; *path && *path!=':'; ++path,++p) {
            *p = *path;
        }
        // empty path entries are treated like "."
        if(p==buf) *p++='.';
        // slash and command name
        if(p[-1]!='/') *p++='/';
        strcpy(p, cmd);
        // check if we can execute it
        if(access(buf, X_OK)==0) {
            free(buf);
            return true;
        }
        // quit at last cycle
        if(!*path) break;
    }
    // not found
    free(buf);
    return false;
}

// // The base of this code is in the "got_data" function in js_libcurl.c
// char  *get_uuid(char *buffer, size_t itemsize, size_t nitems, void* ignorethis)
// {   
//     // This computes the number of bytes that was received in the response body.
//     size_t bytes = itemsize * nitems;
//     int linenumber = 1;

//     printf("New chunk(%zu)\n", bytes);
//     printf("%d:\t", linenumber);

//     for (int i = 0; i < bytes; i++) {
//         printf("%c", buffer[i]);
//         if (buffer[i] == '\n') {
//             linenumber++;
//             // int this case each line number resets after each chunk of data.
//             printf("%d:\t", linenumber);
//         }
//     }
//     // This adds some separation between each chunk of data.
//     printf("\n\n");
//     return bytes;
// }
