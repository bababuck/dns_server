#include <time.h>

#include "../include/generator.h"
#include "../include/cli_parser.h"
#include "../include/router.h"
#include "../include/scoreboard.h"
#include "../include/generator.h"
#include "../include/dns_server.h"
#include "../include/dns.h"
#include "../include/server.h"
#include "../include/file_utils_c.h"

#define CLIENT_PORT 1050

/**
 * A test generator, stores information about the test to be run.
 */
typedef struct {
  router_t *router;
  scoreboard_t *scoreboard;
  dns_server_t** dns_servers;
  uint8_t dns_server_cnt;
  arguments_t *arguments;
} generator_t;

/**
 * Create a single generator object.
 *
 * @params arguments: Object containing the test parameters
 *
 * @returns newly created generator
 */
generator_t* create_generator(arguments_t *arguments);

/**
 * Create a single generator object.
 *
 * @params generator: object to destroy
 *
 * @returns Error code, 0 if successful
 */
uint8_t destroy_generator(generator_t *generator);

/**
 * Create a DNS message and send to router/scoreboard
 *
 * @params g: Generator to send from
 * @params id: Id to use to track the test
 *
 * @returns Error code, 0 if successful
 */
uint8_t send_single_test(generator_t *g, uint16_t id);

/**
 * Send a request to the router.
 *
 * Also forwards the reuqest/server used to the scoreboard
 *
 * @params generator: Generator object
 * @params message: message being sent
 * @params message_len: length of the message
 * @params id: for forwardng to scoreboard
 *
 * @returns Error code, 0 if successful
 */
uint8_t send_to_router(generator_t *generator, uint8_t* message, uint8_t message_len, uint16_t id);

static char **domains;
static uint8_t domain_cnt;

int main(int argc, char **argv) {
  arguments_t arguments;
  srand(time(NULL));
  void init_scoreboard();
  domain_cnt = get_domains("hosts.txt", &domains);
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
  g->arguments = arguments;
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
  ++(generator->dns_server_cnt);
  generator->dns_servers = realloc(generator->dns_servers, generator->dns_server_cnt);
  generator->dns_servers[generator->dns_server_cnt - 1] = create_dns_server(get_ip(), CLIENT_PORT, DNS_PORT_NUM + generator->dns_server_cnt - 1, false);
  printf("enter\n");
  printf("ADDRESS %0x\n", (uint8_t*) generator->dns_servers[generator->dns_server_cnt - 1]);
  update_and_online(generator->dns_servers[generator->dns_server_cnt - 1]);
  send_single_test(generator, 0);
  send_single_test(generator, 1);
  send_single_test(generator, 2);
  sleep(10);
  destroy_generator(generator);
  return 0;
}

uint8_t send_single_test(generator_t *g, uint16_t id) {
  uint8_t buffer[MAX_DNS_BYTES];
  char *domain = domains[((uint8_t) rand()) % domain_cnt];
  uint8_t message_len = craft_message(buffer, /*query=*/true, id, /*domain=*/domain, /*ip=*/NULL);
  send_to_router(g, buffer, message_len, id);
  return 0;
}

uint8_t send_to_router(generator_t *generator, uint8_t* message, uint8_t message_len, uint16_t id) {
  uint8_t dns_id, rrl_removed;
  double request_time = get_time_elapsed();
  forward_request(generator->router, message, message_len, &dns_id, &rrl_removed);
  recieve_generated_req(generator->scoreboard, id, dns_id, rrl_removed, request_time);
  return 0;
}
