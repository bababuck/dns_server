#include "../include/generator.h"
#include "../include/scoreboard.h"
#include "../include/generator.h"
#include "../include/dns_server.h"
#include "../include/dns.h"

#define CLIENT_PORT 1054

uint8_t run_test(char *testname) {
  scoreboard_t *scoreboard = create_scoreboard("my_test", (uint16_t) CLIENT_PORT);
  dns_server_t *dns_server = create_dns_server(char *scoreboard_ip, (uint16_t) CLIENT_PORT, (uint16_t) DNS_PORT_NUM);

  destroy_dns_server(dns_server);
  destroy_scoreboard(scoreboard);
}
