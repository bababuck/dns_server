#include "../include/generator.h"
#include "../include/router.h"
#include "../include/scoreboard.h"
#include "../include/generator.h"
#include "../include/dns_server.h"
#include "../include/dns.h"
#include "../include/server.h"

#include "cli_parser.c"

#define CLIENT_PORT 1050

typedef struct {
  router_t *router;
  scoreboard_t *scoreboard;
  dns_server_t** dns_servers;
  uint8_t dns_server_cnt;
} generator_t;

generator_t* create_generator(char* testname);
uint8_t destroy_generator(generator_t *generator);
uint8_t send_single_test(generator_t *g, uint8_t id);
uint8_t send_to_router(generator_t *generator, uint8_t* message, uint8_t message_len);

int main(int argc, char **argv) {
  arguments_t arguments;
  parse_cli(argc, argv, &arguments);
  return run_test(strdup("my_test"));
}

generator_t* create_generator(char* testname) {
  generator_t *g = malloc(sizeof(generator_t));
  g->scoreboard = create_scoreboard(testname, (uint16_t) CLIENT_PORT);
  g->router = create_router(ROUND_ROBIN, NULL);
  g->dns_server_cnt = 2;
  g->dns_servers = malloc(g->dns_server_cnt * sizeof(dns_server_t*));
  g->dns_servers[0] = create_dns_server(get_ip(), CLIENT_PORT, DNS_PORT_NUM + 1);
  g->dns_servers[1] = create_dns_server(get_ip(), CLIENT_PORT, DNS_PORT_NUM);
  add_dns_server(g->router, g->dns_servers[0]);
  add_dns_server(g->router, g->dns_servers[1]);
  return g;
}

uint8_t destroy_generator(generator_t *generator) {
  uint8_t rtn_code;
  if ((rtn_code = destroy_scoreboard(generator->scoreboard))) {
    return rtn_code;
  }
  if ((rtn_code = destroy_router(generator->router))) {
    return rtn_code;
  }
  for (int i = 0; i < generator->dns_server_cnt; ++i) {
    if ((rtn_code = destroy_dns_server(generator->dns_servers[i]))) {
      return rtn_code;
    }
  }
  free(generator);
  return 0;
}

uint8_t run_test(char *testname) {
  generator_t *generator = create_generator(testname);
  send_single_test(generator, 0);
  send_single_test(generator, 1);
  send_single_test(generator, 2);
  sleep(5);
  destroy_generator(generator);
  return 0;
}

uint8_t send_single_test(generator_t *g, uint8_t id) {
  recieve_generated_req(g->scoreboard, id);
  uint8_t buffer[MAX_DNS_BYTES];
  char domain[] = "google";
  uint8_t message_len = craft_message(buffer, /*query=*/true, id, /*domain=*/domain, /*ip=*/NULL);
  send_to_router(g, buffer, message_len);
  return 0;
}

uint8_t send_to_router(generator_t *generator, uint8_t* message, uint8_t message_len) {
  return forward_request(generator->router, message, message_len);
}
