#include <vector>
#include <string>
#include <unordered_map>
#include <unistd.h>

#include "../include/coms.h"
#include "../include/file_utils_cpp.h"

extern "C" {

typedef std::unordered_map<std::string, std::string> hash_t;

  coms_t *create_coms(uint8_t id, bool read_hosts, int tcp_socket) {
  coms_t *c = (coms_t*) malloc(sizeof(coms_t));
  c->coms = NULL;
  c->server_cnt = 0;
  c->socket = tcp_socket;
  c->id = id;
  c->ip_hash = (void*) new hash_t();
  if (read_hosts) {
    std::vector<std::pair<std::string, std::string>> translations =  parse_hosts_file("hosts.txt");
    for (auto trans : translations) {
      std::string domain = trans.first;
      std::string ip = trans.second;
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

uint8_t request_hosts(coms_t *coms, uint16_t port) {
  ((hash_t*) coms->ip_hash)->clear();
  int new_socket = create_tcp_connections(coms->socket);

  uint8_t request_code = 1; // Saying whole file
  if (send(new_socket, &request_code, 1, 0) < 0) {
    perror("Send()");
    exit(7);
  }

  // Recieve all data
  uint8_t trans_cnt;
  recv(new_socket, &trans_cnt, 1, 0);
  uint8_t d_buffer[128];
  uint8_t i_buffer[128];
  for (int i = 0; i < trans_cnt; ++i) {
    if (recv(new_socket, d_buffer, sizeof(d_buffer), 0) < 0) return 1;
    if (recv(new_socket, i_buffer, sizeof(i_buffer), 0) < 0) return 1;
    (*((hash_t*) coms->ip_hash))[std::string((char*) d_buffer)] = std::string((char*) i_buffer);
  }
  return 0;
}

}
