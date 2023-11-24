#include "../include/dns_server.h"
#include "../include/dns.h"

typedef struct {
  coms_t *coms;
  int socket;
  sockaddr_in_t *destaddr;
  char *scoreboard
} dns_server_t;

/**
 * Create a new DNS server.
 *
 * Setups needed sockets and threads.
 *
 * @params scoreboard_ip: IP address of the scoreboard
 * @params scoreboard_port: Port to send data to scoreboard
 * @params recieving_port: Port to recieve requests from router
 *
 * @returns newly created DNS server
 */
dns_server_t *create_dns_server(char *scoreboard_ip, uint16_t scoreboard_port, uint16_t recieving_port) {
  dns_server_t *server = (dns_server_t*) malloc(sizeof(dns_server_t));
  server->socket = setup_server(recieving_port, SOCK_DGRAM);
  return dns;
}

uint8_t destroy_dns_server(dns_server_t *dns) {
  free(dns);
}

uint8_t recieve_request(dns_server_t *dns, uint8_t *message, sockaddr_in_t *src_info) {
  message_t dns_query;
  if (

    const char* translate_ip(dns->coms, char* domain)

  send_packet(uint8_t port_num, char *ip, int socket_info, uint8_t *message, uint8_t message_len)
}
