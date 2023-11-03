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
  pthread_t *tcp_resp_thread;
  uint8_t version_num;
} coms_t;

/**
 * Create a new coms module.
 *
 * Setups needed sockets and threads, creates the needed files.
 *
 * @params id: id of the created module.
 * @params read_host: Read from host file rather than requesting from other server
 * @params tcp_socket: initialized socket to use for host file transfers
 *
 * @returns newly created coms module
 */
  coms_t *create_coms(uint8_t id, bool read_host, int tcp_socket);

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
 * Translate a domain name to an IP address.
 *
 * @params coms: coms object to use
 * @params domain: Domain of interest
 *
 * @returns IP address as c-string, NULL if not found
 */
const char* translate_ip(coms_t *coms, char* domain);

/**
 * Gather hosts.txt information from other sever.
 *
 * @params coms: coms object to use
 * @params port: port to connect with
 * @params ip: Ip to send to
 *
 * @returns Error code, 0 if successful
 */
uint8_t request_hosts(coms_t *coms, uint16_t port, char *ip);

/**
 * Update the host file and propogate to other servers
 *
 * @params coms: coms object to use
 * @params router_ip: IP of the router
 * @params server_ip: IP of the other servers (HACK)
 * @params remove: If true remove entry, add if false
 * @params domain: Domain to add/remove
 * @params ip: Ip to use if adding
 * @params own_port: Don't want to forward to ourselves
 * @params id: ID of the update
 * @params fail: Backdoor to cause failures
 *
 * @returns Error code, 0 if successful
 */
uint8_t update_hosts(coms_t *coms, char *router_ip, char *server_ip, bool remove, char *domain, char *ip, uint16_t own_port, uint8_t id, int8_t fail);

#ifdef __cplusplus
}
#endif
