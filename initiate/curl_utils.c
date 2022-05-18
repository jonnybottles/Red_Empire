#include <stdbool.h>
#include "curl_utils.h"

// Prepares curl for use across the various follow on functions that use the
// curl library.
bool curl_prep(struct web_comms *web)
{
	if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
		perror("curl_global_init error\n");
		return false;
	}
	web->curl = curl_easy_init();
	if (!web->curl) {
		perror("curl_easy_init error.\n");
		return false;

	} else {
		/* Create the form */
		// returns null if failure.
		web->form = curl_mime_init(web->curl);
		if (!web->form) {
			perror("curl_mime_init error\n");
			return false;
		}
    }
    return true;
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

