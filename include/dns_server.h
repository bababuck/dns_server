/**
 * A DNS server capable of handling requests via network.
 *
 * Responses sent out via network to the scoreboard.
 */

#pragma once

#include <stdint.h>

#include "dns.h"
#include "coms.h"
#include "server.h"

/**
 * A DNS server.
 *
 * coms holds all the needed translation information.
 *
 * Socket and destaddr and used for networking with router and scoreboard.
 */
typedef struct {
  coms_t *coms;
  int socket;
  int tcp_socket;
  char *scoreboard_ip;
  uint16_t scoreboard_port;
  char *router_ip;
  uint16_t router_port;
  uint16_t id;
  uint16_t port_num;
  uint16_t tcp_port_num;
  char *ip;
  pthread_t *response_thread;
  bool pause;
} dns_server_t;

/**
 * Create a new DNS server.
 *
 * Setups needed sockets and threads.
 *
 * @params scoreboard_ip: IP address of the scoreboard
 * @params scoreboard_port: Port to send data to scoreboard
 * @params recieving_port: Port to recieve requests from router
 * @params read_host: Read from host file rather than requesting from other server
 *
 * @returns newly created DNS server
 */
dns_server_t *create_dns_server(char *scoreboard_ip, uint16_t scoreboard_port, uint16_t recieving_port, bool read_host);

/**
 * Deallocate a new DNS server.
 *
 * Closes associated sockets and kills threads.
 *
 * @params dns: DNS server to destroy
 *
 * @returns Error code, 0 if successful
 */
uint8_t destroy_dns_server(dns_server_t *dns);

/**
 * Recieve a request from the router.
 *
 * @params dns: DNS server to handle the request
 * @params message: DNS message to parse
 * @params message_bytes: Length of recieved message
 * @params src_info: Information about where to respond to, only used for router queries.
 *
 * @returns Error code, 0 if successful
 */
uint8_t recieve_request(dns_server_t *dns, uint8_t *message, uint8_t message_bytes);

/**
 * Update a DNS server and add it to the rotation to be used by the router.
 */
uint8_t update_and_online(dns_server_t *dns_server);
