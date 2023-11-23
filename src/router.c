/**
 * Route requests incoming requests to a given DNS module.
 *
 * This will be the common route for incoming messages.
 *
 * Outgoing messages will be sent to the internet directly bypassing this router.
 *
 * Each DNS module has a socket for sending outgoing messages, and one for communicating to other DNS modules.
 *
 * This router will also implement the response rate limiting module.
 */

/**
 * Create and bind socket to a 
 */
uint8_t initialize_router();

/**
 * Add a new dns_server() to the rotation.
 *
 * Need to tell the new DNS server about the other servers already available.
 *
 * 
 */
void add_dns_server();

lock on both the hosts.txt file and coms module
