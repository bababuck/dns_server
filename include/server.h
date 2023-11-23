/**
 * Base code for the server to interface with DNS requests.
 *
 * Use UDP protocol to communicate DNS responses.
 */

#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define DNS_PORT_NUM 1053 // 53 is priviliged
#define MAX_DNS_BYTES 64
typedef struct sockaddr_in sockaddr_in_t;
typedef struct sockaddr sockaddr_t;

/**
 * Create and bind a socket.
 *
 * @param port_num
 * @param ip
 * @param sockaddr:
 * @param protocol: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
 */
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

/**
 * Send a packet from a already bound socket.
 *
 * @param port_num: Port number of the destination
 * @param ip: IP address to send to
 * @param socket_info: Descriptor of the socket to use
 * @param message: Message to send
 * @param message_len: Length of message
 *
 * @returns Error code, 0 is successful
 */
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

/**
 * Setup a new response thread for handling incoming messages.
 *
 * @param thread: For tracking the created thread information
 * @param receive_routine: Function to run to respond to incoming messages
 * @param arg: argument for the response routine
 *
 * @returns Error code, 0 if successful
 */
uint8_t setup_response_thread(pthread_t *thread, void *(receive_routine)(void*), void *arg) {
  if (pthread_create(thread, NULL, receive_routine, arg) != 0) {
    perror("pthread_create");
    exit(6);
  }
  return 0;
}

/**
 * Kill response thread.
 *
 * No deallocation done.
 *
 * @param thread: Thread to destroy
 *
 * @returns Error code, 0 if successful
 */
uint8_t kill_response_thread(pthread_t *thread) {
  if (pthread_kill(*thread, 0) != 0) {
    perror("pthread_kil");
    exit(7);
  }
  return 0;
}


/**
 * Wait for a message on a specified socket.
 *
 * @param buffer: Location to store message data
 * @param length: Max message length
 * @param socket_info: File descriptor of socket to use
 *
 * @returns Lenght of message recieve
 */
uint8_t recieve_message(uint8_t *buffer, uint8_t length, int socket_info) {
  uint8_t bytes_recieved;
  if((bytes_recieved = recvfrom(socket_info, buffer, length, 0, NULL, NULL)) < 0) {
    perror("recvfrom");
    exit(5);
  }
  return bytes_recieved;
}
