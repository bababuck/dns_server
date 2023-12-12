#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/cli_parser.h"
#include "../include/router.h"


int parse_cli(int argc, char **argv, arguments_t *arguments) {
  opterr = 0;
  arguments->save = false;
  arguments->test_name = NULL;
  arguments->starting_server_cnt = 1;
  arguments->randomly_disable = false;
  arguments->add_midway = false;
  arguments->router_mode = ROUND_ROBIN;
  arguments->make_translation_changes = false;
  int c;

  while ((c = getopt(argc, argv, "xst:ar:dc:")) != -1) {
    switch (c) {
    case 's':
      arguments->save = true;
      break;
    case 'a':
      arguments->add_midway = true;
      break;
    case 'd':
      arguments->randomly_disable = true;
      break;
    case 'x':
      arguments->make_translation_changes = true;
      break;
    case 'r':
      if (strcmp(optarg, "ROUND_ROBIN")) {
        arguments->router_mode = ROUND_ROBIN;
      } else if (strcmp(optarg, "OVERLOAD")) {
        arguments->router_mode = OVERLOAD;
      } else {
        fprintf(stderr, "Router mode must be ROUND_ROBIN or OVERLOAD.\n");
        abort();
      }
      break;
    case 't':
      arguments->test_name = strdup(optarg);
      break;
    case 'c':
      arguments->starting_server_cnt = atoi(optarg);
      if (arguments->starting_server_cnt < 0 || arguments->starting_server_cnt > 5) {
        fprintf(stderr, "Server count must be between 1 and 4 inclusive.\n");
        abort();
      }
      break;
    case '?':
      if (optopt == 't') {
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      } else if (isprint (optopt)) {
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      } else {
        fprintf(stderr,
                "Unknown option character `\\x%x'.\n",
                optopt);
      }
      return 1;
    default:
      abort ();
    }
  }

  if (arguments->test_name == NULL) {
    fprintf(stderr, "Testname required.\n");
    abort();
  }

  printf ("Running with parameters: save = %s, test-name = %s, starting-server-cnt = %d, randomly-disable-server = %s, add-server-midway = %s, router-mode = %s, make-translation-changes = %s\n",
          arguments->save ? "yes" : "no",
          arguments->test_name,
          arguments->starting_server_cnt,
          arguments->randomly_disable ? "yes" : "no",
          arguments->add_midway ? "yes" : "no",
          arguments->router_mode == ROUND_ROBIN ? "ROUND_ROBIN" : "OVERLOAD",
          arguments->make_translation_changes ? "yes" : "no");

  for (int index = optind; index < argc; index++) {
    printf ("Non-option argument %s\n", argv[index]);
  }
  return 0;
}
