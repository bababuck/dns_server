/**
 * Header files for C file utilities.
 *
 * A little hacky, but I want to have just one .c file, so I created 2 separate
 * headers to include depending if c or cpp interface.
 *
 * Not all all efficient, but just need something simple for small files.
 */

/**
 * Parse a hosts.txt file and report back DOMAIN:IP pairs.
 *
 * @params filename: hosts.txt file name
 * @params domains: array of domain names
 *
 * @returns Number of domain
 */
uint8_t get_domains(const char *filename, char ***domains);
