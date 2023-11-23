/**
 * Response rate limititing (RRL) module tracks the frequency of requests from different IPs.
 *
 * RRL is used to protect users from DNS amplification attacks, as a given IP
 * address will only be able to recieve so many repsonses for our server.
 *
 * This RRL is implemented quite simply, using a window of 15 seconds and reseting
 * the counter every time the window closes. This is handled internally via callbacks.
 *
 * The counter is impelemented via a hash table, without any locking for multithreading.
 *
 * This module can either sit inside the router or inside individual DNS modules.
 */

#pragma once

#include <boolean.h>
#include <stdint.h>

/**
 * Contains all the information needed for RRL.
 *
 * Uses a hash table to perform the lookups.
 *
 * Locks the hash table during refresh.
 */
typedef struct {
  void *ip_hash_table;
  void *mutex;
} rrl_t;

/**
 * Creates a rrl object.
 *
 * On creation, starts a thread that will refresh the RRL every minute.
 *
 * @returns newly allocated/initialized rrl object
 */
rrl_t* create_rrl();

/**
 * Free all the memory associated with an RRL object.
 *
 * @params rrl: the RRL object to destroy
 *
 * @returns Error code, 0 is successful
 */
uint8_t destroy_rrl(rrl_t *rrl);

/**
 * Check whether a given IP has been used too frequently.
 *
 * Increments the internal counter for the requested address.
 *
 * @param ip_address: The ip_address to be checked.
 *
 * @returns True if the requested IP address has passed its threshold
 */
bool check_lookup(char* ip_address);
