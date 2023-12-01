/**
 * Provide basic functionality for command line options.
 *
 * Gets needed information for generating a test-run.
 *
 * This is awful, just need some functionality without using `argp`.
 *
 * https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
 */

#pragma once

#include <stdbool.h>

#include "router.h"

/* Used by main to communicate with parse_opt. */
typedef struct
{
  char *test_name;
  bool save;
  uint8_t starting_server_cnt;
  bool randomly_disable;
  bool add_midway;
  router_mode_t router_mode;
  bool make_translation_changes;
} arguments_t;


/**
 * Parse a set of command line options.
 *
 * @param argc: count of arguments
 * @param argv: array of arguments
 * @param arguments: object containing input data
 *
 * @returns Error code, 0 if successful
 */
int parse_cli(int argc, char **argv, arguments_t *arguments);
