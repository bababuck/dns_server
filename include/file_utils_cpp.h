/**
 * Header files for CPP file utilities.
 *
 * A little hacky, but I want to have just one .c file, so I created 2 separate
 * headers to include depending if c or cpp interface.
 *
 * Not all all efficient, but just need something simple for small files.
 */

#include <fstream>
#include <string>
#include <sstream>
#include <vector>

/**
 * Parse a hosts.txt file and report back DOMAIN:IP pairs.
 *
 * @params filename: hosts.txt file name
 *
 * @returns vector of domain to IP translations.
 */
std::vector<std::pair<std::string, std::string>> parse_hosts_file(const char *filename);
