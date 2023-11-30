#include "../include/generator.h"
#include "../include/router.h"
#include "../include/scoreboard.h"
#include "../include/generator.h"
#include "../include/dns_server.h"
#include "../include/dns.h"
#include "../include/server.h"

#define CLIENT_PORT 1054

typedef struct {
  router_t *router;
  scoreboard_t *scoreboard;
  dns_server_t* dns_server;
} generator_t;

generator_t* create_generator(char* testname);
uint8_t destroy_generator(generator_t *generator);
uint8_t send_single_test(generator_t *g, uint8_t id);
uint8_t send_single_test(generator_t *g, uint8_t id);
uint8_t send_to_router(generator_t *generator, uint8_t* message, uint8_t message_len);

int main(int argc, char **argv) {
  return run_test(strdup("my_test"));
}

generator_t* create_generator(char* testname) {
  generator_t *g = malloc(sizeof(generator_t));
  g->scoreboard = create_scoreboard(testname, (uint16_t) CLIENT_PORT);
  printf("SCOREBOARD\n");
  g->router = create_router(ROUND_ROBIN, NULL);
  printf("ROUTER\n");
  g->dns_server = create_dns_server(get_ip(), CLIENT_PORT, DNS_PORT_NUM);
  printf("DNS\n");
  add_dns_server(g->router, g->dns_server);
  return g;
}

uint8_t destroy_generator(generator_t *generator) {
  uint8_t rtn_code;
  if ((rtn_code = destroy_scoreboard(generator->scoreboard))) {
    return rtn_code;
  }
  printf("SCOREBOARD\n");
  if ((rtn_code = destroy_router(generator->router))) {
    return rtn_code;
  }
  printf("ROUTER\n");
  if ((rtn_code = destroy_dns_server(generator->dns_server))) {
    return rtn_code;
  }
  printf("DNS\n");
  free(generator);
  return 0;
}

uint8_t run_test(char *testname) {
  generator_t *generator = create_generator(testname);
  printf("CREATED\n");
  send_single_test(generator, 0);
  printf("TEST SENT\n");
  destroy_generator(generator);
  return 0;
}

uint8_t send_single_test(generator_t *g, uint8_t id) {
  recieve_generated_req(g->scoreboard, id);
  uint8_t buffer[MAX_DNS_BYTES];
  char domain[] = "";
  uint8_t message_len = craft_message(buffer, /*query=*/true, id, /*domain=*/domain, /*ip=*/NULL);
  send_to_router(g, buffer, message_len);
  return 0;
}

uint8_t send_to_router(generator_t *generator, uint8_t* message, uint8_t message_len) {
  return forward_request(generator->router, message, message_len);
}
