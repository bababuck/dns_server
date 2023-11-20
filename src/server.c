/**
 * Base code for the server to interface with DNS requests.
 *
 * Use UDP protocol to communicate DNS responses.
 */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#define PORTNUM 1053 // 53 is priviliged

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
  server.sin_port = PORT_NUM;

  // Bind socket to requested port
  if (bind(socket_info, (sockaddr_t *) &server, sizeof(server)) < 0) {
    perror("bind");
    exit(2);
  }

  socklen_t length = sizeof(sockaddr_in_t);
  if (getsockname(socket_info, (sockaddr_t *) &server, &length) < 0) {
    perror("getsockname()");
    exit(3);
  }

  printf("Port assigned: %d\n", server.sin_port);

  if (server.sin_port != PORT_NUM) {
    perror("PORT_NUM unavailable");
    exit(4);
  }
}

int main(int argc, char *argv[]) {
  setup_server();
  return 0;
}
