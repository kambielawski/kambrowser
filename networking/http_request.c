/* 
initial code from: https://stackoverflow.com/questions/11208299/how-to-make-an-http-get-request-in-c-without-libcurl
*/

#define _XOPEN_SOURCE 700
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h> /* getprotobyname */
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h> /* open() flag option O_RDWR */

/*
 * connect with exponential back-off
 */
int connect_retry(int domain, int type, int protocol, const struct sockaddr *addr, socklen_t alen) {
  int numsec, socket_fd;
  for (numsec = 1; numsec <= 128; numsec <<= 1) {
    // create a socket
    if ((socket_fd = socket(domain, type, protocol)) < 0) {
      return (-1);
    }
    // make the connection
    if (connect(socket_fd, addr, alen) == 0) {
      /* connection accepted */
      printf("connection accepted\n");
      return (socket_fd);
    }
    close(socket_fd);
    
    /* delay before trying again */
    printf("delay: %d\n", numsec);
    if (numsec < 128) 
      sleep(numsec);
  }
  return (-1);
}

int main(int argc, char** argv) {
    char buffer[BUFSIZ];
    enum CONSTEXPR { MAX_REQUEST_LEN = 1024};
    char request[MAX_REQUEST_LEN];
    char request_template[] = "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";
    struct protoent *protoent;
    char *hostname = "example.com";
    in_addr_t in_addr;
    int request_len;
    int socket_file_descriptor;
    ssize_t nbytes_total, nbytes_last;
    struct hostent *hostent;
    struct sockaddr_in sockaddr_in;
    unsigned short server_port = 80;

    if (argc > 1)
        hostname = argv[1];
    if (argc > 2)
        // if port is given in cmdline, str to unsigned long it
        server_port = strtoul(argv[2], NULL, 10);

    request_len = snprintf(request, MAX_REQUEST_LEN, request_template, hostname);
    if (request_len >= MAX_REQUEST_LEN) {
        fprintf(stderr, "request length large: %d\n", request_len);
        exit(EXIT_FAILURE);
    }

    /* Build the socket. */
    // getprotobyname will return a protoent struct, defined as:
    // struct protoent {
    //    char *p_name;       // string of the name of protocol
    //    char **p_aliases;   // NULL-terminated list of other names for the protocol
    //    int  p_proto;       // protocol number
    // }
    protoent = getprotobyname("tcp");
    if (protoent == NULL) {
        perror("getprotobyname");
        exit(EXIT_FAILURE);
    }

    /* Build the address. */
    // struct hostent {
    //    cha    *h_name;       // official name of host
    //    char  **h_aliases;    // host aliases
    //    int     h_addrtype;   // host address type
    //    int     h_length;     // length of address
    //    char  **h_addr_list;  // list of addresses
    // }
    hostent = gethostbyname(hostname);
    if (hostent == NULL) {
        fprintf(stderr, "error: gethostbyname(\"%s\")\n", hostname);
        exit(EXIT_FAILURE);
    }
    // in_addr is a struct containing a 32 bit int specifying an IP address
    // if in format a.b.c.d, first 8 bits are a, second 8 are b, etc.
    in_addr = inet_addr(inet_ntoa(*(struct in_addr*)*(hostent->h_addr_list)));
    if (in_addr == (in_addr_t)-1) {
        fprintf(stderr, "error: inet_addr(\"%s\")\n", *(hostent->h_addr_list));
        exit(EXIT_FAILURE);
    }
    // struct sockaddr_in {
    //    sa_family_y     sin_family;   // address family: AF_INET
    //    in_port_y       sin_port;     // port in network byte order
    //    struct in_addr  sin_addr;     // internet address
    // }
    sockaddr_in.sin_addr.s_addr = in_addr;
    sockaddr_in.sin_family = AF_INET;
    sockaddr_in.sin_port = htons(server_port);

    /* connect with exponential backoff */
    // AF_INET specifies IPv4 Internet domain
    // SOCK_STREAM is a socket that "provides a byte-stream service; applications are unaware of message boundaries"
    if ((socket_file_descriptor = connect_retry(AF_INET, SOCK_STREAM, protoent->p_proto, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in))) == -1) {
      perror("connection timed out");
      exit(EXIT_FAILURE);
    }

    // socket_file_descriptor = socket(AF_INET, SOCK_STREAM, protoent->p_proto);
    // if (socket_file_descriptor == -1) {
    //    perror("socket");
    //    exit(EXIT_FAILURE);
    // }

    /* Actually connect. */
    // if (connect(socket_file_descriptor, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == -1) {
    //    perror("connect");
    //    exit(EXIT_FAILURE);
    // }

    /* Send HTTP request. */
    nbytes_total = 0;
    while (nbytes_total < request_len) {
        nbytes_last = write(socket_file_descriptor, request + nbytes_total, request_len - nbytes_total);
        if (nbytes_last == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        nbytes_total += nbytes_last;
    }

    // int filed = creat("response.txt", S_IRUSR | S_IWUSR | S_IXUSR);

    /* Read the response. */
    fprintf(stderr, "debug: before first read\n");
    while ((nbytes_total = read(socket_file_descriptor, buffer, BUFSIZ)) > 0) {
        fprintf(stderr, "debug: after a read\n");
        write(STDOUT_FILENO, buffer, nbytes_total);
    }
    fprintf(stderr, "debug: after last read\n");
    if (nbytes_total == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(socket_file_descriptor);
    exit(EXIT_SUCCESS);
}