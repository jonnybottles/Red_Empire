#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdarg.h> /* for variadic function */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h> /* basic socket definitions */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <curl/curl.h>

#define SERVER_PORT 9000

#define MAXLINE 4096
#define SA struct sockaddr

// web cli ref https://www.youtube.com/watch?v=bdIiTxtMaKA&list=PL9IEJIKnBJjH_zM5LnovnoaKlXML5qh17&index=1





int main(int argc, char **argv)
{
    int sockfd, n;
    int sendbytes;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE];
    char recvline[MAXLINE];

    // if (argc != 2) {
    //     err_n_die("Usage: %s <server address> ", argv[0]);
    // }

    const char *server_ip = "127.0.0.1";

    // AF = Address Family, INET = Internet
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_n_die("Error while creating the socket!");
    }

    // Zeros out the server address.
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    // Converts string representation of the IP address to a binary representation of the IP address.
    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) {
        err_n_die("inet_pton error for %s ", server_ip);
    }

    // Connect to the server.
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr))< 0) {
        err_n_die("Connect failed!");
    }

    // char request[] = "GET /tasklist HTTP/1.1\r\n\r\n";

    // We're connected. Prepare the message.
    sprintf(sendline, "GET /tasklist HTTP/1.1\r\n\r\n");
    sendbytes = strlen(sendline);

    // Send the request -- making sure you send it all.
    // This code is a bit fragile, since it bails if only some of the bytes are sent.
    // Normally, you would want to retry, unless the return value was -1.

    if (write(sockfd, sendline, sendbytes) != sendbytes) {
        err_n_die("write error");
    }

    memset(recvline, 0, MAXLINE);

    //Now read the server' response.
    while ( (n = read(sockfd, recvline, MAXLINE -1)) > 0) {
        printf("%s", recvline);
        // this null terminates the recvline after every iteration
        memset(recvline, 0, MAXLINE);
    }

    if (n < 0) {
        err_n_die("read error");
    }
    return 0;
}

void err_n_die(const char *fmt, ...) {
    // ** IF I USE THIS I NEED LIAM'S HELP TO UNDERSTAND ALL OF THIS **
    int errno_save;
    va_list ap;

    // all system calls can set errno, so we need to save it now.
    errno_save = errno;

    // print ouf the fmt+args to standard out
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);

    // print out error message if errno was set.
    if (errno_save != 0) {
        fprintf(stdout, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
        fprintf(stdout, "\n");
        fflush(stdout);
    }
    va_end(ap);

    //this is the .. and die part. Terminate with an error.
    exit(1);
}
