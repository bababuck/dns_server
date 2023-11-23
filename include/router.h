/**
 *
 */

#pragma once

#include "rrl.h"
#include "dns.h"

/**
 * Determines the mode in which the router will be operating.
 */
typedef enum {
  OVERLOAD,
  ROUND_ROBIN // Circle through all available DNS nodes when distributing requests
} router_mode_t;

/**
 * 
 */
typedef struct {
  router_mode_t mode;
  rrl_t *rrl;
  dns_t **servers;
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
 * Add a new DNS server to the rotation.
 *
 * @param router: router to add the server to
 * @param dns: DNS server to add to the router
 *
 * @returns Error code, 0 is successful
 */
uint8_t add_dns_server(router_t *router, dns_t *dns);

/**
 *
 *
 */
