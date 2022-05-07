#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "initiate.h"
#include "utils.h"

//ref https://curl.se/libcurl/c/postit2.html

int reg(void)
{
  CURL *curl;
  CURLcode res;
 
  curl_mime *form = NULL;
  curl_mimepart *field = NULL;
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
  
      // Begin code to set options for posting hostname / os to /reg.
      field = curl_mime_addpart(form);
      if (!field) {
        perror("curl_mime_addpart error\n");
      }

      // Get target hostname.
      // TO DO figure out how to import HOST_NAME_MAX
      char hostbuf[256];
      if (gethostname(hostbuf, sizeof(hostbuf)) == -1) {
        perror("Error acquiring host name.\n");
      }

      if (curl_mime_name(field, "hostname") != CURLE_OK) {
        perror("Error curl_mime_name hostname\n");
      }
    
      if (curl_mime_data(field, hostbuf, CURL_ZERO_TERMINATED) != CURLE_OK) {
        perror("Error curl_mime_data hostname\n");
      }
  

      field = curl_mime_addpart(form);
      if (!field) {
        perror("curl_mime_addpart error\n");
      }

      if (curl_mime_name(field, "os") != CURLE_OK) {
        perror("Error curl_mime_name OS\n");
      }

      if (curl_mime_data(field, "UBUNTU 20.04", CURL_ZERO_TERMINATED) != CURLE_OK) {
        perror("Error curl_mime_data OS\n");
      }
  
      // Fill in submit field options.
      field = curl_mime_addpart(form);
      if (!field) {
        perror("curl_mime_addpart error\n");
      }

      if (curl_mime_name(field, "submit") != CURLE_OK) {
        perror("Error curl_mime_name submit");
      }

      if (curl_mime_data(field, "send", CURL_ZERO_TERMINATED) != CURLE_OK) {
        perror("Error curl_mime_data send");
      }
  
      /* initialize custom header list (stating that Expect: 100-continue is not
        wanted */
      headerlist = curl_slist_append(headerlist, buf);
      if (!headerlist) {
        perror("curl_slist_append error\n");
      }

      const char regurl[19] = "127.0.0.1:9000/reg";
      /* what URL that receives this POST */
      curl_easy_setopt(curl, CURLOPT_URL, regurl);

      curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
  
      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);
      /* Check for errors */
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
                return 1;
  
      /* always cleanup */
      curl_easy_cleanup(curl);
      
      /* then cleanup the form */
      curl_mime_free(form);
      /* free slist */
      curl_slist_free_all(headerlist);
    }
  return 0;
}

// char *get_host_name(void)
// {
//   char *hostname = malloc(sizeof(*hostname));
//   int ret;
//   ret = gethostname(hostname, sizeof(hostname));
//   if (ret == -1)
//   {
//     perror("Error getting hostname");
//   }
//   // printf("Host name is %s\n", hostname);
//   return hostname;

// }