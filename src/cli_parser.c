/**
 * Provide basic functionality for command line options.
 *
 * Gets needed information for generating a test-run
 *
 *
 *
 * https://www.gnu.org/software/libc/manual/html_node/Argp-Example-3.html
 */

#include <stdlib.h>
#include <argp.h>

const char *argp_program_version =
  "argp-ex3 1.0";
const char *argp_program_bug_address =
  "<bug-gnu-utils@gnu.org>";

/* Program documentation. */
static char doc[] =
  "Distributed DNS - Test and performance generator for a distributed DNS system";

/* A description of the arguments we accept. */
static char args_doc[] = "dns-count";

/* The options we understand. */
static struct argp_option options[] = {
  {"testname",  't', "NAME",     0,                    "What is the name of this test" },
  {"save",      's', 0,          OPTION_ARG_OPTIONAL,  "Should this test be saved" }
  { 0 }
};

/* Used by main to communicate with parse_opt. */
typedef struct
{
  char *args[2];
  char *test_name;
  int save;
} arguments_t;

/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  arguments_t *arguments = state->input;

  switch (key)
    {
    case 's':
      arguments->save = 1;
      break;
    case 't':
      arguments->test_name = arg;
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num >= 2)
        /* Too many arguments. */
        argp_usage(state);

      arguments->args[state->arg_num] = arg;
      break;

    case ARGP_KEY_END:
      if (state->arg_num < 2)
        /* Not enough arguments. */
        argp_usage (state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

int parse_cli(int argc, char **argv, arguments_t *arguments) {
  /* Default values. */
  arguments->save = 0;

  /* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
  argp_parse(&argp, argc, argv, 0, 0, arguments);

  printf ("save = %s\ntest_name = %s\n",
          arguments->save ? "yes" : "no",
          arguments->test_name);

  return 0;
}
