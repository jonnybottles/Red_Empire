#include <curl/curl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>
#include "initiate.h"
#include "utils.h"

//ref https://curl.se/libcurl/c/postit2.html

bool reg(void)
{
	CURL *curl;
	CURLcode res;
 
	curl_mime *form = NULL;
	curl_mimepart *field = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";
 
	if (curl_global_init(CURL_GLOBAL_ALL)!= 0) {
		perror("curl_global_init error\n");
		return false;
	}
 
	curl = curl_easy_init();
	if(!curl) {
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
	
			// Begin code to set options for posting hostname / os to /reg.
			field = curl_mime_addpart(form);
			if (!field) {
				perror("curl_mime_addpart error\n");
				return false;
			}

			// Get target hostname.
			// TO DO figure out how to import HOST_NAME_MAX
			char hostbuf[256];
			if (gethostname(hostbuf, sizeof(hostbuf)) == -1) {
				perror("Error acquiring host name.\n");
				return false;
			}

			if (curl_mime_name(field, "hostname") != CURLE_OK) {
				perror("Error curl_mime_name hostname\n");
				return false;
			}
		
			if (curl_mime_data(field, hostbuf, CURL_ZERO_TERMINATED) != CURLE_OK) {
				perror("Error curl_mime_data hostname\n");
				return false;
			}
	

			field = curl_mime_addpart(form);
			if (!field) {
				perror("curl_mime_addpart error\n");
				return false;
			}

			struct utsname buf1;
			errno =0;
			if(uname(&buf1)!=0)
			{
					perror("uname error\n");
					return false;
			}
			if (curl_mime_name(field, "os type") != CURLE_OK) {
				perror("Error curl_mime_name OS\n");
				return false;
			}

			if (curl_mime_data(field, buf1.nodename, CURL_ZERO_TERMINATED) != CURLE_OK) {
				perror("Error curl_mime_data OS\n");
				return false;
			}

	
			field = curl_mime_addpart(form);
			if (!field) {
				perror("curl_mime_addpart error\n");
				return false;
			}

			if (curl_mime_name(field, "os version") != CURLE_OK) {
				perror("Error curl_mime_name OS\n");
				return false;
			}

			if (curl_mime_data(field, buf1.version, CURL_ZERO_TERMINATED) != CURLE_OK) {
				perror("Error curl_mime_data OS\n");
				return false;
			}

			// Fill in submit field options.
			field = curl_mime_addpart(form);
			if (!field) {
				perror("curl_mime_addpart error\n");
				return false;
			}

			if (curl_mime_name(field, "submit") != CURLE_OK) {
				perror("Error curl_mime_name submit");
				return false;
			}

			if (curl_mime_data(field, "send", CURL_ZERO_TERMINATED) != CURLE_OK) {
				perror("Error curl_mime_data send");
				return false;
			}
	
			/* initialize custom header list (stating that Expect: 100-continue is not
				wanted */
			headerlist = curl_slist_append(headerlist, buf);
			if (!headerlist) {
				perror("curl_slist_append error\n");
				return false;
			}

			const char regurl[19] = "127.0.0.1:9000/reg";
			/* what URL that receives this POST */
			curl_easy_setopt(curl, CURLOPT_URL, regurl);

			curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, is_registered);
	
			/* Perform the request, res will get the return code */
			res = curl_easy_perform(curl);
			/* Check for errors */
			if(res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
								curl_easy_strerror(res));
								return false;
			}

			long code;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
			printf("return code is %ld\n", code);
			/* always cleanup */
			curl_easy_cleanup(curl);

			/* then cleanup the form */
			curl_mime_free(form);
			/* free slist */
			curl_slist_free_all(headerlist);
		}
	return true;
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
			fprintf(stderr, "download problem: %s\n", curl_easy_strerror(result));
	}

	curl_easy_cleanup(curl);
	return 0;
}


// The base of this code is in the "got_data" function in js_libcurl.c
size_t is_registered(char *buffer, size_t itemsize, size_t nitems, void* ignorethis)
{   
		// This computes the number of bytes that was received in the response body.
		size_t bytes = itemsize * nitems;
		int linenumber = 1;
		const char *resp_code = "201";
		strstr(buffer, resp_code);

		printf("New chunk(%zu)\n", bytes);
		printf("%d:\t", linenumber);
		printf("%s\n", buffer);
		// for (int i = 0; i < bytes; i++) {
		//     printf("%c", buffer[i]);
		//     if (buffer[i] == '\n') {
		//         linenumber++;
		//         // int this case each line number resets after each chunk of data.
		//         printf("%d:\t", linenumber);
		//     }
		// }
		// This adds some separation between each chunk of data.
		printf("\n\n");
		return bytes;
}

// The base of this code is in the "got_data" function in js_libcurl.c
char  *get_tasks(char *buffer, size_t itemsize, size_t nitems, void* ignorethis)
{   
		// This computes the number of bytes that was received in the response body.
		size_t bytes = itemsize * nitems;
		int linenumber = 1;

		printf("New chunk(%zu)\n", bytes);
		printf("%d:\t", linenumber);

		for (int i = 0; i < bytes; i++) {
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