#include <curl/curl.h>
#ifndef CURL_UTILS_H
#define CURL_UTILS_H

struct web_comms {
	CURL *curl;
	CURLcode res;
	curl_mime *form;
};

// This function is used to reduce the amounts of curl_mime_addpart,
// curl_mine_name, and curl_mime_data calls.
void add_curl_field(curl_mime * form, const char *name, const char *data);

// Prepares curl for use across the various follow on functions that use the
// curl library.
bool curl_prep(struct web_comms *web);

#endif