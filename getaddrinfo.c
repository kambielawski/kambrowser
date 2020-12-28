#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr, "usage: %s nodename port\n", argv[0]);
    exit(1);
  }

  int res;
  struct addrinfo     *result;
  struct addrinfo     hints;
  struct sockaddr     *sockaddr;

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_CANONNAME;
  hints.ai_family = 0;
  hints.ai_socktype = 0;
  hints.ai_protocol = 0;
  hints.ai_addrlen = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  if ((res = getaddrinfo(argv[1], argv[2], NULL, &result)) != 0) {
    fprintf(stderr, "error: getaddrinfo %d\n", res);
    exit(1);
  }
  printf("res: %d\n", res);
  print_protocol(result);
  print_family(result);
  print_address(result);

  return 0;
}
