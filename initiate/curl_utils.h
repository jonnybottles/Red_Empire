#include <curl/curl.h>
#ifndef CURL_UTILS_H
#define CURL_UTILS_H

struct web_comms {
	CURL *curl;
	CURLcode res;
	curl_mime *form;
};

void add_curl_field(curl_mime * form, const char *name, const char *data);

bool curl_prep(struct web_comms *web);

#endif