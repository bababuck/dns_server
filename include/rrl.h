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
