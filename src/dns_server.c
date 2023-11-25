#include <string.h>
#include <unistd.h>

#include "../include/dns_server.h"
#include "../include/dns.h"

dns_server_t *create_dns_server(char *scoreboard_ip, uint16_t scoreboard_port, uint16_t recieving_port) {
  dns_server_t *dns_server = (dns_server_t*) malloc(sizeof(dns_server_t));
  dns_server->socket = setup_server(recieving_port, SOCK_DGRAM);
  dns_server->scoreboard_port = scoreboard_port;
  dns_server->scoreboard_ip = strdup(scoreboard_ip);
  return dns_server;
}

uint8_t destroy_dns_server(dns_server_t *dns_server) {
  free(dns_server->scoreboard_ip);
  close(dns_server->socket);
  free(dns_server);
  return 0;
}

uint8_t recieve_request(dns_server_t *dns_server, uint8_t *message, uint8_t message_bytes, sockaddr_in_t *src_info) {
  message_t dns_query;
  bool error = false;
  if (parse_message(message, &dns_query, message_bytes)) {
    return 1;
  }

  if (dns_query.header.qr == 1) {
    return 2;
  }

  const char* result_ip = translate_ip(dns_server->coms, dns_query.question.domain);

  uint8_t buffer[MAX_DNS_BYTES];
  message_bytes = craft_message(buffer, false, dns_query.header.id, dns_query.question.domain, result_ip);

  send_packet(dns_server->scoreboard_port, dns_server->scoreboard_ip, dns_server->socket, buffer, message_bytes);
  return 0;
}