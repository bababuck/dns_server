/**
 * Route requests from the outside world to the appropriate DNS server.
 *
 * This library emulates a router in software to distribute the load of the DNS
 * requests among the servers in the system.
 *
 * See "https://en.wikipedia.org/wiki/Anycast"
 *
 * Responsible for removing servers from the routing if their response time is
 * too slow.
 */

#pragma once

#define ROUTER_PORT_NUM 2055

#include <pthread.h>
#include <stdint.h>

#include "rrl.h"
#include "dns_server.h"

/**
 * Determines the mode in which the router will be operating.
 */
typedef enum {
  OVERLOAD, // Target most of the requests at one DNS server (until it fails)
  ROUND_ROBIN // Circle through all available DNS nodes when distributing requests
} router_mode_t;

/**
 * A router object that distributes requests among its subjects.
 */
typedef struct {
  router_mode_t mode;
  rrl_t *rrl;
  dns_server_t **servers;
  uint8_t server_cnt;
  uint8_t curr_server;
  int socket;
  uint16_t query_counter;
  pthread_t *resp_thread;
  pthread_mutex_t *mutex;
} router_t;

/**
 * Create a new router with a given rrl module.
 *
 * @param mode: desired routing mode for the router.
 * @param rrl: rrl object to use, if NULL don't use RRL
 *
 * @returns A dynamically created and initialized router
 */
router_t* create_router(router_mode_t mode, rrl_t *rrl);

/**
 * Free the memory associated with a given router.
 *
 * Does not delete the associated servers.
 * Does delete the RRL object.
 *
 * @returns Error code, 0 is successful
 */
uint8_t destroy_router(router_t *router);

/**
 * Add a new DNS server to the rotation.
 *
 * @param router: router to add the server to
 * @param dns: DNS server to add to the router
 *
 * @returns Error code, 0 is successful
 */
uint8_t add_dns_server(router_t *router, dns_server_t *dns);

/**
 * Send a test to each DNS server to get it's response time
 *
 * If doesn't return within allowed time, then remove
 *
 * @param router: the router to request from.
 * @param allowed_seconds: time given to prove life
 *
 * @returns error code, 0 if successful
 */
uint8_t query_response_time(router_t *router, uint8_t allowed_seconds);

/**
 * Forward a DNS request to a pseudo-random (accounts for routing mode) DNS server.
 *
 * Can be done either in software or via network.
 *
 * Only forwards the request if RRL allows.
 *
 * @params router: router used to forward the request.
 * @params message: dns request to forward
 * @params message_len: length of message
 * @params dns_id: DNS server used
 * @params rrl_removed: True if removed by RRL
 *
 * @returns Error code, 0 if successful
 */
uint8_t forward_request(router_t *router, uint8_t *message, uint8_t message_len, uint8_t *dns_id, bool *rrl_removed);

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
uint8_t remove_server(router_t *router, uint8_t dns_id);
