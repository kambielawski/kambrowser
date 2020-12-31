#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define MAX_WAIT 128
#define MAX_REQ_LEN 1024

void print_protocol (struct addrinfo *ai) {
  printf ("protocol: ");
  switch (ai->ai_protocol) {
    case 0:
      printf("default\n");
      break;
    case IPPROTO_TCP:
      printf("tcp\n");
      break;
    case IPPROTO_UDP:
      printf("udp\n");
      break;
    case IPPROTO_RAW:
      printf("raw\n");
      break;
    default:
      printf("unknown: (%d)", ai->ai_protocol);
  }
}

void print_family (struct addrinfo *ai) {
  printf("family: ");
  switch (ai->ai_family) {
    case AF_INET:
      printf("IPv4");
      break;
    case AF_INET6:
      printf("IPv6");
      break;
    case AF_UNIX:
      printf("UNIX (local)");
      break;
    case AF_UNSPEC:
      printf("unspecified");
      break;
    default:
      printf("unknown");
  }
  printf(" (%d)\n", ai->ai_family);
}

void print_address(struct addrinfo *ai) {
  char ipv4_abuf[INET_ADDRSTRLEN];
  char ipv6_abuf[INET6_ADDRSTRLEN];
  const char *addr;
  struct sockaddr_in  *sinp;
  struct sockaddr_in6 *sin6p;

  if (ai->ai_family == AF_INET) {
    sinp = (struct sockaddr_in*)ai->ai_addr; // cast sockaddr struct to sockaddr_in
    addr = inet_ntop(AF_INET, &sinp->sin_addr, ipv4_abuf, INET_ADDRSTRLEN);
    printf("address: %s\n", addr?addr:"unknown");
  }
  if (ai->ai_family == AF_INET6) {
    sin6p = (struct sockaddr_in6*)ai->ai_addr; // cast sockaddr struct to sockaddr_in6
    addr = inet_ntop(AF_INET6, &sin6p->sin6_addr, ipv6_abuf, INET6_ADDRSTRLEN);
    printf("address: %s\n", addr?addr:"unknown");
  }

}

void print_address_length(struct addrinfo *ai) {
  printf("address length: %d bytes\n", (int) ai->ai_addrlen);
}

int connect_retry(int domain, int type, int protocol, struct sockaddr* sockaddr, socklen_t alen) {
  int numsec, socket_fd;
  for (numsec = 1; numsec < MAX_WAIT; numsec <<= 1) {
    // create socket
    if ((socket_fd = socket(domain, type, protocol)) < 0) {
      return (-1);
    }

    if (connect(socket_fd, sockaddr, alen) == 0) {
      // connection accepted
      return (socket_fd);
    }
    close(socket_fd);

    // wait for next try
    if (numsec <= MAX_WAIT/2)
      sleep(numsec);
  }
  return (-1);
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "usage: %s hostname [port]\n", argv[0]);
    exit(1);
  }

  // socket/connection
  int ai_res;
  int socket_fd; // socket file descriptor
  struct addrinfo     *addrinfo;
  struct sockaddr     *sockaddr;
  struct sockaddr_in  *sockaddr_in;
  struct sockaddr_in6 *sockaddr_in6;
  
  // http request 
  int nbytes_total, nbytes_last;
  int request_len;
  char buffer[BUFSIZ];
  char request[MAX_REQ_LEN];
  char request_template[] = "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";

  if (argc > 2) {
    ai_res = getaddrinfo(argv[1], argv[2], NULL, &addrinfo);
  } else {
    ai_res = getaddrinfo(argv[1], "80", NULL, &addrinfo);
  }
  // get address info
  if (ai_res != 0) {
    fprintf(stderr, "error: getaddrinfo\n");
    exit(1);
  }

  // establish connection
  if ((socket_fd = connect_retry(addrinfo->ai_family, SOCK_STREAM, 0, addrinfo->ai_addr, addrinfo->ai_addrlen)) == -1) {
    fprintf(stderr, "connection timed out");
    exit(1);
  }

  // setup request
  if ((request_len = snprintf(request, MAX_REQ_LEN, request_template, argv[1])) >= MAX_REQ_LEN) {
    fprintf(stderr, "error: hostname too long\n");
    exit(1);
  }

  // write http request to socket
  nbytes_total = 0;
  while (nbytes_total < request_len) {
    nbytes_last = write(socket_fd, request + nbytes_total, request_len - nbytes_total);
    if (nbytes_last == -1) {
      fprintf(stderr, "error: write");
      exit(1);
    }
    nbytes_total += nbytes_last;
  }

  int filed = creat("response.txt", S_IRWXU);
  
  // read response from socket
  while ((nbytes_total = read(socket_fd, buffer, BUFSIZ)) > 0) {
    write(filed, buffer, nbytes_total);
  }

  if (nbytes_total == -1) {
    fprintf(stderr, "error: read");
    exit(1);
  }

  close(socket_fd);

  return 0;
}
