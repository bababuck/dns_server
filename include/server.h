 /**
 * Base code for the server to interface.
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
 * @param port_num: port number to use
 * @param protocol: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
 *
 * @returns socket descriptor
 */
int setup_server(uint16_t port_num, int protocol);

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

uint8_t send_packet(uint16_t port_num, char *ip, int socket_info, uint8_t *message, uint8_t message_len);

/**
 * Setup a new response thread for handling incoming messages.
 *
 * @param thread: For tracking the created thread information
 * @param receive_routine: Function to run to respond to incoming messages
 * @param arg: argument for the response routine
 *
 * @returns Error code, 0 if successful
 */
uint8_t setup_response_thread(pthread_t *thread, void *(receive_routine)(void*), void *arg);

/**
 * Kill response thread.
 *
 * No deallocation done.
 *
 * @param thread: Thread to destroy
 *
 * @returns Error code, 0 if successful
 */
uint8_t kill_response_thread(pthread_t *thread);

/**
 * Wait for a message on a specified socket.
 *
 * @param buffer: Location to store message data
 * @param length: Max message length
 * @param socket_info: File descriptor of socket to use
 *
 * @returns Lenght of message recieve
 */
uint8_t recieve_message(uint8_t *buffer, uint8_t length, int socket_info);

char* get_ip();
