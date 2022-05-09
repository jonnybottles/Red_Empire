#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>

// the function passed to CURLOPT_WRITEFUNCTION use the signature below.
// the function will return the size of the data received.
// It takes in a buffer, a itemsize is the size of each item that came in
// which is always going to be one, nitems, void * is set to "ignore this"
// in this example, but you can use it to pass in other objects if required.
size_t got_data(char *buffer, size_t itemsize, size_t nitems, void* ignorethis)
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


int main(void)
{   
    // Curl pointer to CURL struct will be used as a handle as subsequent calls
    // to the curl library.
    CURL *curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Curl init failed\n");
        return 1;
    }
    // set options / perfour action is the typical pattern seen with curl in C.
    // Param 1 is the curl handle created before.
    // Param 2 is CURLOPT_URL, which then allows you to specify the URL in
    // param 3 that you want download. 
    curl_easy_setopt(curl, CURLOPT_URL, "https://jacobsorber.com");

    // This line below specifies what to do with the data when we receive it,
    // as opposed to printing it to stdout. We will pass in a ptr to our own function
    // called got_data in this example. This is known as a call back function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, got_data);

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