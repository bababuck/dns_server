#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unistd.h>

#include "../include/coms.h"

extern "C" {

typedef std::unordered_map<std::string, std::string> hash_t;

coms_t *create_coms(uint8_t id, bool read_hosts) {
  coms_t *c = (coms_t*) malloc(sizeof(coms_t));
  c->coms = NULL;
  c->server_cnt = 0;
  c->socket = -1; // Socket will be needed for TCP
  c->id = id;
  c->ip_hash = (void*) new hash_t();
  if (read_hosts) {
    std::ifstream infile("hosts.txt");
    std::string line;
    while (std::getline(infile, line)) {
      std::istringstream iss(line);
      std::string domain, ip;
      if (!(iss >> domain >> ip)) {
        fprintf(stderr, "Invalid host file");
        free(c);
        return NULL;
      }
      (*((hash_t*) c->ip_hash))[domain] = ip;
    }
  } else {
    // Get from other host
    // First, ask router for other IPs / Ports
    // Then request for entire file
  }
  // create hosts[id].txt
  return c;
}

uint8_t destroy_coms(coms_t *coms) {
  // close hosts[id].txt
  close(coms->socket);
  delete (hash_t*) coms->ip_hash;
  free(coms);
  return 0;
}

uint8_t add_coms(coms_t *coms, char *ip, uint8_t id, uint16_t port_num) {
  // TODO
  return 0;
}
uint8_t request_update(coms_t *coms) {
  // TODO
  return 0;
}

uint8_t handle_update_request(coms_t *coms, uint8_t id) {
  // TODO
  return 0;
}

const char* translate_ip(coms_t *coms, char* domain) {
  hash_t *ip_hash  = (hash_t *) coms->ip_hash;
  if (ip_hash->count(domain) != 0) {
    return ((*ip_hash)[domain]).c_str();
  }
  return NULL;
}

}
