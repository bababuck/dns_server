#include "../include/server.h"

int setup_server(uint8_t port_num, char* ip, sockaddr_in_t *server, int protocol) {
  int socket_info = socket(AF_INET, protocol, 0);
  if (socket_info < 0) {
    perror("socket");
    exit(1);
  }
  server->sin_addr.s_addr = INADDR_ANY; // Any addresses
  server->sin_family = AF_INET; // Internet Protocol v4 addresses
  server->sin_port = htons(port_num);

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

  if (server->sin_port != htons(port_num)) {
    perror("port_num unavailable");
    exit(4);
  }

  return socket_info;
}

uint8_t send_packet(uint8_t port_num, char *ip, int socket_info, uint8_t *message, uint8_t message_len) {
  sockaddr_in_t dest_server;
  dest_server.sin_family  = AF_INET;  // Internet Domain
  dest_server.sin_port = htons(port_num); // Server Port
  dest_server.sin_addr.s_addr = inet_addr(ip); // Server's Address

  if (sendto(socket_info, message, message_len, 0, (sockaddr_t *) &dest_server, sizeof(dest_server)) < 0) {
    perror("sendto()");
    exit(5);
  }
  return 0;
}

uint8_t setup_response_thread(pthread_t *thread, void *(receive_routine)(void*), void *arg) {
  if (pthread_create(thread, NULL, receive_routine, arg) != 0) {
    perror("pthread_create");
    exit(6);
  }
  return 0;
}

uint8_t kill_response_thread(pthread_t *thread) {
  if (pthread_kill(*thread, 0) != 0) {
    perror("pthread_kil");
    exit(7);
  }
  return 0;
}

uint8_t recieve_message(uint8_t *buffer, uint8_t length, int socket_info) {
  uint8_t bytes_recieved;
  if((bytes_recieved = recvfrom(socket_info, buffer, length, 0, NULL, NULL)) < 0) {
    perror("recvfrom");
    exit(5);
  }
  return bytes_recieved;
}
