/**
 * Handle all the hosts information for a given DNS server.
 *
 * Must handle updates to the host file both and primary and secondary server.
 * Must be able to request from other coms modules new information.
 *
 * Deals with hashing the host file.
 * Handles the locks on host files and itself when updating.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "server.h"

/**
 * A single communications object.
 *
 * Contains IP address for other coms modules.
 * Maintains a socket for inter-coms communcation.
 * Has an ID to know its changelog and host.txt files
 */
typedef struct {
  void **coms;
  uint8_t server_cnt;
  int socket;
  uint8_t id;
  void *ip_hash;
} coms_t;

/**
 * Create a new coms module.
 *
 * Setups needed sockets and threads, creates the needed files.
 *
 * @params id: id of the created module.
 *
 * @returns newly created coms module
 */
coms_t *create_coms(uint8_t id);

/**
 * Deallocate a coms module
 *
 * Doesn't delete files.
 *
 * @params coms: Coms object to destory
 *
 * @returns Error code, 0 if successful
 */
uint8_t destroy_coms(coms_t *coms);

/**
 * Add the IP address of a new coms module.
 *
 * @params coms: coms object to use
 * @params ip: IP address of new module
 * @params id: ID of new module
 * @params port_num: Port to communicate with other coms module one
 *
 * @returns Error code, 0 if successful
 */
uint8_t add_coms(coms_t *coms, char *ip, uint8_t id, uint16_t port_num);

/**
 * Request host file update from other coms module.
 *
 * @params coms: coms object to use
 *
 * @returns Error code, 0 if successful
 */
uint8_t request_update(coms_t *coms);

/**
 * Handle update request from a different coms module.
 *
 * @params coms: coms object to use
 *
 * @returns Error code, 0 if successful
 */
uint8_t handle_update_request(coms_t *coms, uint8_t id);

/**
 * Translate a domain name to an IP address.
 *
 * @params coms: coms object to use
 * @params domain: Domain of interest
 *
 * @returns IP address as c-string, NULL if not found
 */
const char* translate_ip(coms_t *coms, char* domain);

#ifdef __cplusplus
}
#endif
