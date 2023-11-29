#include "../include/router.h"
#include "../include/server.h"

#define ROUTER_PORT_NUM 1055

router_t* create_router(router_mode_t mode, rrl_t *rrl) {
  router_t *r = malloc(sizeof(router_t));
  r->mode = mode;
  r->rrl = rrl;
  r->servers = NULL;
  r->server_cnt = 0;
  r->curr_server = 0;
  r->socket = setup_server(ROUTER_PORT_NUM, SOCK_DGRAM);
  return r;
}

uint8_t destroy_router(router_t *router) {
  for (int i = 0; i < router->server_cnt; ++i) {
    destroy_dns_server(router->servers[i]);
  }
  free(router->servers);
  free(router);
  return 0;
}

uint8_t add_dns_server(router_t *router, dns_server_t *dns) {
  ++(router->server_cnt);
  router->servers = realloc(router->servers, router->server_cnt);
  router->servers[router->server_cnt - 1] = dns;
  return 0;
}

/**
 * Send a test to each DNS server to get it's response time
 *
 * Used both internally to know when a server is down, and externally
 * to gather statistics.
 *
 * Conducted by sending a fake query (recognized as fake by the DNS), which
 * the DNS will recognize, do a random lookup, and then ping the router on
 * completion.
 *
 * @param router: the router to request from.
 * @param dns_id: which server number to query
 *
 * @returns time of completion in milliseconds, maximum of 6000 ms, -1 on error
 */
uint64_t query_response_time(router_t *router, uint8_t dns_id) {
  return 0;
}

uint8_t forward_request(router_t *router, uint8_t *message, uint8_t message_len) {
  return send_packet(/*port_num=*/DNS_PORT_NUM, /*ip=*/get_ip(), /*socket_info=*/router->socket, message, message_len);
}

/**
 * Remove a DNS server from the rotation is response time is too slow.
 *
 * Will have to re-add itself later.
 *
 * @param router: the router to remove the server from.
 * @param dns_id: which server number to remove
 *
 * @returns error code, 0 if successful
 */
uint8_t remove_server(router_t *router, uint8_t dns_id) {
  return 0;
}
