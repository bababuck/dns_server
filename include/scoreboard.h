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
