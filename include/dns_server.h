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
#include "scoreboard.h"

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
  sockaddr_in_t *destaddr;
} dns_server_t;

/**
 * Create a new DNS server.
 *
 * Setups needed sockets and threads.
 *
 * @params scoreboard_ip
 * @params scoreboard_port
 * @params recieving_port
 *
 * @returns newly created DNS server
 */
dns_server_t *create_dns_server(char *scoreboard_ip, uint16_t scoreboard_port, uint16_t recieving_port);

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
 * @params message: DNS message to parse
 * @params src_info: Information about where to respond to, only used for router queries.
 *
 * @returns Error code, 0 if successful
 */
uint8_t recieve_request(uint8_t *message, sockaddr_in_t *src_info);
