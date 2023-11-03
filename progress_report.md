# Progress Report

Ryan Buchner

November 22, 2023

### Project Overview

My project is focused on creating a distributed authoritive DNS server. The main goal of the server must be capable of handling requests via different nodes and synchronizing the `host.txt` file. In addition, it must be able to handle downtime of different nodes, namely if parts of the DNS server, go offline, the system must be able to gracefully handle this failure, and the restart of those servers.

#### Final Report Structure

My final product will include the DNS system as well instructions to build and run it. In addition, I will outline the final design (I suspect aspects of the design described here will change slightly as I find bugs via testing), as well as the performance results of the system. Lastly, a thorough description of all of the tests run will be included.

### System Design

The general system design is outlined in the following sketch, and a in depth breakdown of the individual components as well as their API's are shown below. The components are designed such that they can sit on the same or different computers. The scoreboard, generator, and router should be linked together via software on the same computer. The DNS servers can be located on the same computer or elsewhere; regardless they will communicate via network. Initially, all the component will run on the same computer, but eventually I will try and run the DNS servers on different computers.

Below, I outline the individual components of my system via their header files. Please note for the following header file include extra functions that do not belong in then API to show the functionality and testing.

![Overview](/Users/personal/Desktop/dns_design.png)

#### Generator

The generator module will be the main hub for testing the DNS system. It will be responsible for generating stimulus as well as setting up the system for a given trial. It is designed to run on the same system and the scoreboard and will interact with the scoreboard locally.

`generator.h`
```c
/**
 * Generate stimulus to test the DNS system.
 *
 * This will act as then main hub for interfacing with a running tests.
 */

#pragma once

/**
 * Generate a random test of the DNS and log it.
 *
 * Under the hood, this will be responsible for generating an entire system.
 * The system will be generated randomly.
 *
 * Once generated, a simulation will be run and performance will be recorded.
 *
 * The simualtion will include all of:
 * - Generating random requests to the router
 * - Changing the host files
 * - Shutting off individual DNS servers randomly
 * - Checking the results of lookups
 *
 * All the results and statistics will be logged
 *
 * @params testname: Name of the test, to be used for log file naming.
 *
 * @results Exit code, 0 if successful
 */
uint8_t run_test(char *testname);
```

#### Router

The router is responsible for forwarding requests to an appropriate DNS server. This would normally be done by an actual router, but for the sake of the project we implemented this in software.

`router.h`
```c
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
 *ND)
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
uint64_t query_response_time(router_t *router, uint8_t dns_id);

/**
 * Forward a DNS request to a pseudo-random (accounts for routing mode) DNS server.
 *
 * Can be done either in software or via network.
 *
 * Only forwards the request if RRL allows.
 *
 * @params router: router used to forward the request.
 * @params use_network: if true, send to the DNS servers over the network.
 * @params message: dns request to forward
 *
 * @returns error code, 0 if successful
 */
uint8_t forward_request(router_t *router, bool use_network, uint8_t *message);

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
```

#### RRL

The RRL module will sit inside the Router, and when a request comes in it will be prompted by the router to decide if the request should be dropped or not. This module is not too important to the main goals of the project.
`rrl.h`
```c
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
 * @params rrl: the RRL object to lookup in
 * @param ip_address: The ip_address to be checked.
 *
 * @returns True if the requested IP address has passed its threshold
 */
bool check_lookup(rrl_t *rrl, char* ip_address);
```

### DNS

This is the library I created to give me basic DNS functionality. It allows for easy creation and parsing of DNS messages. It doesn't deal with any of the socket programming though, see `dns_server.h` for that.

`dns.h`
```c
/**
 * Library for basic DNS message crafting and parsing.
 *
 * Tailored for the specific needs of this project.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**
 * A header for an DNS message.
 */
typedef struct {
  uint16_t id : 16;
  uint8_t rd : 1;
  uint8_t tc : 1;
  uint8_t aa : 1;
  uint8_t opcode : 4;
  uint8_t qr : 1;
  uint8_t rcode : 4;
  uint8_t z : 3;
  uint8_t ra : 1;
  uint16_t qdcount : 16;
  uint16_t ancount : 16;
  uint16_t nscount : 16;
  uint16_t arcount : 16;
} header_t;

/**
 * A single question section for a DNS message.
 */
typedef struct {
  char *domain;
  uint16_t qtype;
  uint16_t qclass;
} question_t;

/**
 * A single answer section for a DNS message.
 *
 * Technically works for all resourse records.
 */
typedef struct {
  char *ip;
  uint16_t type;
  uint16_t class;
  uint32_t ttl;
  uint8_t rdlength;
  uint8_t *rdata;
} answer_t;

/**
 * Format a message from the given information.
 *
 * @param buffer: buffer to fill with the message
 * @param query: is this a query or a response
 * @param id: ID of the message
 * @param domain: domain name of the message
 * @param ip: IP address if a response
 *
 * @returns Length of message, 0 if error
 */
uint8_t craft_message(uint8_t *buffer, bool query, uint16_t id, char *domain, char *ip);

/**
 * Parse a DNS message.
 *
 * @param message: DNS message to parse
 * @param header: Found header information
 * @param question: Found question information
 * @param answer: Found answer information
 *
 * @returns Error code, 0 if successful
 */
uint8_t parse_message(uint8_t *message, header_t *header, question_t *question, answer_t *answer);
```

### DNS Server

Each DNS server is responsible for it's own `host.txt` file, a `changelog` and `DNSSEC` information (I have not yet implemented DNSSEC). In addition, it is accessible via the network to be requested to make changes to it's host file. The changes are logged and forwarded to the other servers via a 3-stage commit. The zone transfer functionality is not implemented yet. In addition, other servers that recently came online may request for either the most recent changes or the entire `hosts.txt`, depending when they came online. All of the data is stored in the communication module, including the hash table used for DNS lookups.

`dns_server.h`
```c
/**@params src_info: Information about where to respond to, only used for router queries.
 * A DNS server capable of handling requests via network.
 * @returns Error code, 0 if successful
 * Responses sent out via network to the scoreboard.
 */

#pragma once

#include <stdint.h>

#include "dns.h"
#include "coms.h"
#include "server.h"

/**
 * A DNS server.
 *
 * coms holds all the needed translation information.
 *
 * Socket and destaddr and used for networking with router and scoreboard.
 */
typedef struct {
  coms_t *coms;
  int socket;
  sockaddr_in_t *destaddr;
} dns_server_t;

/**
 * Create a new DNS server.
 *
 * Setups needed sockets and threads.
 *
 * @params scoreboard_ip: IP address of the scoreboard
 * @params scoreboard_port: Port to send data to scoreboard
 * @params recieving_port: Port to recieve requests from router
 *
 * @returns newly created DNS server
 */
dns_server_t *create_dns_server(char *scoreboard_ip, uint16_t scoreboard_port, uint16_t recieving_port);

/**
 * Deallocate a new DNS server.
 *
 * Closes associated sockets and kills threads.
 *
 * @params dns: DNS server to destroy
 *
 * @returns Error code, 0 if successful
 */
uint8_t destroy_dns_server(dns_server_t *dns);

/**
 * Recieve a request from the router.
 *
 * @params dns: DNS server to handle the request
 * @params message: DNS message to parse
 * @params src_info: Information about where to respond to, only used for router queries.
 *
 * @returns Error code, 0 if successful
 */
uint8_t recieve_request(dns_server_t *dns, uint8_t *message, sockaddr_in_t *src_info);
```

### Communications

Associated with a DNS server, handles, the files and hash tables for lookups, as well as for communicating with the other servers communications modules.

`coms.h`
```c
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
coms_t *create_dns_server(uint8_t id);

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
 *
 * @returns Error code, 0 if successful
 */
uint8_t add_coms(coms_t *coms, char *ip, uint8_t id);

/**
 * Delete a coms module.
 *
 * @params coms: coms object to use
 * @params id: ID to delete
 *
 * @returns Error code, 0 if successful
 */
uint8_t add_coms(coms_t *coms, uint8_t id);

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
char* translate_ip(coms_t *coms, char* domain);
```

### Scoreboard

Matches up requests from the generator with response from the DNS servers to measure correctness and performance.

`scoreboard.h`
```c
/**
 * Module for testing for correctness and performance of DNS servers.
 *
 * Initially just checking to make sure all ID's are recieved.
 */

#pramga once

#include <stdint.h>

#include "server.h"

/**
 * Contains all information needed by the scoreboard.
 */
typedef struct {
  void *queue;
  char *testname;
  int socket;
  sockaddr_in_t *destaddr;
} scoreboard_t;

/**
 * Recieve a request from the generator to add to queue.
 *
 * Will now have to wait for ID to come in from network.
 *
 * @param id: ID of the request
 *
 * @returns Error code, 0 if successful
 */
uint8_t recieve_generated_req(uint8_t id);

/**
 * Create a new scoreboard to be used for testing.
 *
 * Creates needed threads and sockets.
 *
 * @param testname: Name of the test for logging
 *
 * @results The newly created scoreboard
 */
scoreboard_t create_scoreboard(char *testname);
```

### Current Status

As of today, I have completed a base implementation almost all of the individual components of the above system, including the generator and scoreboard modules, and I'm still in the process of testing those components at the individual level. I have run any full scale tests yet, but I have brainstormed test ideas and implemented the generator for some of them.

I have layed out a schedule for the completion of the remaineder of the project:

Tasks:  
- [] Working system with single DNS server, Nov. 25  
- [] Working system with multiple DNS server, Nov. 27  
- [] Implement security extensions (need to ask questions at office hours), Dec 1  
- [] Run on multiple computers, Dec 5  
- [] Gather results, Dec 7
