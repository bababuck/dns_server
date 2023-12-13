/**
 * Generate stimulus to test the DNS system.
 *
 * This will act as then main hub for interfacing with a running tests.
 */

#pragma once

#include <stdint.h>

#include "cli_parser.h"

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
 * @params arguments: Arguments to generate test for
 *
 * @results Exit code, 0 if successful
 */
uint8_t run_test(arguments_t *arguments);
