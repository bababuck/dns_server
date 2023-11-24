/**
 * Generate stimulus to test the DNS system.
 *
 * This will act as then main hub for interfacing with a running tests.
 */

#pragma once


#include <stdint.h>

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
