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
 
  // first one is only on that i cna check int on
  check(curl_global_init(CURL_GLOBAL_ALL), "Curl global init error\n");
 
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
  
      /* Fill in the file upload field */
      field = curl_mime_addpart(form);
      if (!field) {
        perror("curl_mime_addpart error\n");
      }

      char hostbuf[256];
      if (gethostname(hostbuf, sizeof(hostbuf)) == -1) {
        perror("Error getting host name.\n");
      }

      printf("Host name is %s\n", hostbuf);
      curl_mime_name(field, "hostname");
      curl_mime_data(field, hostbuf, CURL_ZERO_TERMINATED);
  
      /* Fill in the filename field */
      // This may be where I add another field......***
      field = curl_mime_addpart(form);
      curl_mime_name(field, "os");
      curl_mime_data(field, "UBUNTU 20.04", CURL_ZERO_TERMINATED);
  
      /* Fill in the submit field too, even if this is rarely needed */
      field = curl_mime_addpart(form);
      curl_mime_name(field, "submit");
      curl_mime_data(field, "send", CURL_ZERO_TERMINATED);
  
      /* initialize custom header list (stating that Expect: 100-continue is not
        wanted */
      headerlist = curl_slist_append(headerlist, buf);
      /* what URL that receives this POST */
      curl_easy_setopt(curl, CURLOPT_URL, "127.0.0.1:9000/reg");

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