/**
 * Functionality for response rate limititing.
 *
 * Drop repeated requests from the same IP address to
 * help prevent our server from being used for DNS amplification attack.
 */

#include <boolean.h>

typedef struct {

} _t

/**
 *
 */
bool check_lookup(char* ip_address);

void increment_time();
