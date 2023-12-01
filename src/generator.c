#include "../include/generator.h"
#include "../include/cli_parser.h"
#include "../include/router.h"
#include "../include/scoreboard.h"
#include "../include/generator.h"
#include "../include/dns_server.h"
#include "../include/dns.h"
#include "../include/server.h"

#define CLIENT_PORT 1050

typedef struct {
  router_t *router;
  scoreboard_t *scoreboard;
  dns_server_t** dns_servers;
  uint8_t dns_server_cnt;
} generator_t;

generator_t* create_generator(arguments_t *arguments);
uint8_t destroy_generator(generator_t *generator);
uint8_t send_single_test(generator_t *g, uint8_t id);
uint8_t send_to_router(generator_t *generator, uint8_t* message, uint8_t message_len);

int main(int argc, char **argv) {
  arguments_t arguments;
  parse_cli(argc, argv, &arguments);
  return run_test(&arguments);
}

generator_t* create_generator(arguments_t *arguments) {
  generator_t *g = malloc(sizeof(generator_t));
  g->scoreboard = create_scoreboard(arguments->test_name, (uint16_t) CLIENT_PORT);
  g->router = create_router(arguments->router_mode, NULL);
  g->dns_server_cnt = arguments->starting_server_cnt;
  g->dns_servers = malloc(g->dns_server_cnt * sizeof(dns_server_t*));
  char *ip = get_ip();
  for (int i = 0; i < arguments->starting_server_cnt; ++i) {
    g->dns_servers[i] = create_dns_server(strdup(ip), CLIENT_PORT, DNS_PORT_NUM + i, true);
    add_dns_server(g->router, g->dns_servers[i]);
  }
  free(ip);
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

uint8_t run_test(arguments_t *arguments) {
  generator_t *generator = create_generator(arguments);
  send_single_test(generator, 0);
  send_single_test(generator, 1);
  send_single_test(generator, 2);
  sleep(10);
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
