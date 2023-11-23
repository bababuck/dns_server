/**
 * Base code for the server to interface with DNS requests.
 *
 * Use UDP protocol to communicate DNS responses.
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "./dns.c"

#define PORT_NUM 1053 // 53 is priviliged
#define MAX_DNS_SIZE 512
typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr sockaddr_t;

void setup_server() {
  int socket_info = socket(AF_INET, SOCK_DGRAM, 0); // UDP Socket
  if (socket_info < 0) {
    perror("socket");
    exit(1);
  }
  sockaddr_in_t server;
  server.sin_addr.s_addr = INADDR_ANY; // Any addresses
  server.sin_family = AF_INET; // Internet Protocol v4 addresses
  server.sin_port = htons(PORT_NUM);

  // Bind socket to requested port
  if (bind(socket_info, (sockaddr_t *) &server, sizeof(server)) < 0) {
    perror("bind");
    exit(2);
  }

  socklen_t length = sizeof(sockaddr_in_t);
  if (getsockname(socket_info, (sockaddr_t *) &server, &length) < 0) {
    perror("getsockname");
    exit(3);
  }

  printf("Port assigned: %d\n", server.sin_port);

  if (server.sin_port != htons(PORT_NUM)) {
    perror("PORT_NUM unavailable");
    exit(4);
  }

  sockaddr_in_t dns_server;
  dns_server.sin_family  = AF_INET;  // Internet Domain
  dns_server.sin_port = htons(53); // Server Port
  dns_server.sin_addr.s_addr = inet_addr("8.8.8.8"); /* Server's Address   */

  uint8_t request[64];
  craft_message(request);
  for (int i=0; i<30; ++i) {
    printf("%02x ",request[i]);
  }
  printf("\n");
  if (sendto(socket_info, request, 30, 0, (sockaddr_t *) &dns_server, sizeof(dns_server)) < 0) {
    perror("sendto()");
    exit(5);
  }

  uint8_t dns_message[MAX_DNS_SIZE];
  if(recvfrom(socket_info, dns_message, MAX_DNS_SIZE, 0, NULL, NULL) < 0) {
    perror("recvfrom");
    exit(5);
  }
  for (int i=0; i<30; ++i) {
    printf("%02x ", dns_message[i]);
  }
  printf("\n");

  close(socket_info);
}

int main(int argc, char *argv[]) {
  setup_server();
  return 0;
}
