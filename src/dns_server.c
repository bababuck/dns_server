#include <string.h>
#include <unistd.h>

#include "../include/dns_server.h"
#include "../include/dns.h"
#include "../include/coms.h"
#include "../include/router.h"

static uint16_t server_count = 0;

void* query_handler(void *_dns_server);

dns_server_t *create_dns_server(char *scoreboard_ip, uint16_t scoreboard_port, uint16_t recieving_port, bool read_host) {
  dns_server_t *dns_server = malloc(sizeof(dns_server_t));
  dns_server->socket = setup_server(recieving_port, SOCK_DGRAM);
  dns_server->tcp_socket = setup_server(recieving_port + 128, SOCK_STREAM);
  dns_server->scoreboard_port = scoreboard_port;
  dns_server->scoreboard_ip = scoreboard_ip;
  dns_server->router_port = ROUTER_PORT_NUM;
  dns_server->router_ip = get_ip();
  dns_server->id = server_count;
  ++server_count;
  dns_server->ip = get_ip();
  dns_server->port_num = recieving_port;
  dns_server->tcp_port_num = recieving_port + 128;
  dns_server->response_thread = malloc(sizeof(pthread_t));
  setup_response_thread(dns_server->response_thread, &query_handler, dns_server);
  dns_server->coms = create_coms(dns_server->id, read_host, dns_server->tcp_socket);
  return dns_server;
}

uint8_t destroy_dns_server(dns_server_t *dns_server) {
  kill_response_thread(dns_server->response_thread);
  close(dns_server->socket);
  free(dns_server->scoreboard_ip);
  free(dns_server->router_ip);
  free(dns_server->ip);
  free(dns_server->response_thread);
  free(dns_server);
  return 0;
}

void* query_handler(void *_dns_server) {
  dns_server_t *dns_server = (dns_server_t*) _dns_server;
  message_t dns_message;
  uint8_t buffer[MAX_DNS_BYTES];
  while (true) { // Run until thread killed
    uint8_t message_len = recieve_message(buffer, MAX_DNS_BYTES, dns_server->socket);
    recieve_request(dns_server, buffer, message_len);
  }
  return NULL;
}

uint8_t recieve_request(dns_server_t *dns_server, uint8_t *message, uint8_t message_bytes) {
  message_t dns_query;
  //  printf("Parsing DNS query\n");
  if (parse_message(message, &dns_query, message_bytes)) {
    //  printf("OOF %d\n",parse_message(message, &dns_query, message_bytes));
    return 1;
  }

  if (dns_query.header.qr == 1) {
    //    printf("OOF\n");
    return 2;
  }

  //  printf("Translating DNS query\n");
  const char* result_ip = translate_ip(dns_server->coms, dns_query.question.domain);

  //  printf("Responding to DNS query\n");
  if (dns_query.header.id == 1 << 15) {
    uint8_t buffer[MAX_DNS_BYTES];
    message_bytes = craft_message(buffer, false, dns_server->id, dns_query.question.domain, result_ip);

    send_packet(dns_server->router_port, dns_server->router_ip, dns_server->socket, buffer, message_bytes);
    return 0;
  } else {
    uint8_t buffer[MAX_DNS_BYTES];
    message_bytes = craft_message(buffer, false, dns_query.header.id, dns_query.question.domain, result_ip);
    send_packet(dns_server->scoreboard_port, dns_server->scoreboard_ip, dns_server->socket, buffer, message_bytes);
    return 0;
  }
}

uint8_t update_and_online(dns_server_t *dns_server) {
  // Connect to Router TCP
  int new_socket = create_tcp_connections(dns_server->tcp_socket);

  // Recieve all data
  uint8_t port_cnt;
  recv(new_socket, &port_cnt, 1, 0);
  uint16_t *ports = malloc(port_cnt * sizeof(uint16_t));
  for (int i = 0; i < port_cnt; ++i) {
    recv(new_socket, (uint8_t*) &(ports[i]), sizeof(uint16_t), 0);
  }

  // Connect with other DNS servers and recieve hosts.txt
  bool found = false;
  for (int i = 0; i < port_cnt; ++i) {
    if (request_hosts(dns_server->coms, ports[i]) == 0) {
      found = true;
      break;
    }
  }

  if (!found) {
    return 1;
  }

  // Send out own address to be added
  send(new_socket, (uint8_t*) dns_server, sizeof(dns_server_t*), 0);

  return 0;
}
