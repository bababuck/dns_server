/**
 * Module for testing for correctness and performance of DNS servers.
 *
 * Initially just checking to make sure all ID's are recieved.
 */

#pragma once

#include <pthread.h>
#include <stdint.h>

#include "server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Contains all information needed by the scoreboard.
 */
typedef struct {
  void *queue;
  void *lock;
  char *testname;
  int socket;
  //  sockaddr_in_t *destaddr;
  pthread_t *dns_response_thread;
} scoreboard_t;

/**
 * Recieve a request from the generator to add to queue.
 *
 * Will now have to wait for ID to come in from network.
 *
 * @param scoreboard: scoreboard to check against
 * @param id: ID of the request
 * @param dns_id: ID of DNS server used
 * @param rrl_removed: Was request removed by RRL
 *
 * @returns Error code, 0 if successful
 */
  uint8_t recieve_generated_req(scoreboard_t *scoreboard, uint16_t id, uint8_t dns_id, uint8_t rrl_removed);

/**
 * Create a new scoreboard to be used for testing.
 *
 * Creates needed threads and sockets.
 *
 * @param testname: Name of the test for logging
 * @param dns_port: Port to recieve DNS responses from
 *
 * @results The newly created scoreboard
 */
scoreboard_t* create_scoreboard(char *testname, uint16_t dns_port);

/**
 * Destroy a scoreboard
 *
 * Kills and closes threads and sockets.
 *
 * @param scoreboard: Object to destroy
 *
 * @returns Error code, 0 if successful
 */
uint8_t destroy_scoreboard(scoreboard_t *scoreboard);

/**
 * Get the time elapsed since start of simulation.
 *
 * @returns time in seconds
 */
double get_time_elapsed();

#ifdef __cplusplus
}
#endif
