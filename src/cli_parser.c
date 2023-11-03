#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/cli_parser.h"
#include "../include/router.h"


int parse_cli(int argc, char **argv, arguments_t *arguments) {
  opterr = 0;
  arguments->test_name = NULL;
  arguments->starting_server_cnt = 3;
  arguments->randomly_disable = false;
  arguments->add_midway = false;
  arguments->make_translation_changes = false;
  arguments->fail_translation_changes = false;
  int c;

  while ((c = getopt(argc, argv, "adxft:c:")) != -1) {
    switch (c) {
    case 'a':
      arguments->add_midway = true;
      break;
    case 'd':
      arguments->randomly_disable = true;
      break;
    case 'x':
      arguments->make_translation_changes = true;
      break;
    case 'f':
      arguments->fail_translation_changes = true;
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

  printf ("Running with parameters: test-name = %s, starting-server-cnt = %d, randomly-disable-server = %s, add-server-midway = %s, make-translation-changes = %s, fail-translation-changes = %s\n",
          arguments->test_name,
          arguments->starting_server_cnt,
          arguments->randomly_disable ? "yes" : "no",
          arguments->add_midway ? "yes" : "no",
          arguments->make_translation_changes ? "yes" : "no",
          arguments->fail_translation_changes ? "yes" : "no");

  for (int index = optind; index < argc; index++) {
    printf ("Non-option argument %s\n", argv[index]);
  }
  return 0;
}
