#include <vector>
#include <string>
#include <unordered_map>
#include <unistd.h>

#include "../include/coms.h"
#include "../include/router.h"
#include "../include/file_utils_cpp.h"

extern "C" {

typedef std::unordered_map<std::string, std::string> hash_t;

uint8_t process_next_request(coms_t *coms);
uint8_t send_entire_file(int socket, coms_t *coms);
uint8_t recieve_update(coms_t *coms, int socket);
uint8_t actually_update(coms_t *coms, bool remove, char *domain, char *ip, uint8_t id);

/**
 * Write out ip_hash to hosts.txt.
 *
 * Will do every-time on update (rather than finding the line to modify)
 *
 * @params coms: Coms object that needs to write
 *
 * @returns Error code, 0 if successful
 */
uint8_t write_hosts_file(coms_t *coms);

void* check_hosts_requests(void* _coms) {
  coms_t *coms = (coms_t*) _coms;
  while (true) {
    process_next_request(coms);
  }
  return NULL;
}

uint8_t process_next_request(coms_t *coms) {
  int new_socket = create_tcp_connections(coms->socket);

  // Recive request type
  uint8_t request_code;
  recv(new_socket, &request_code, 1, 0);

  if (request_code == 1) {
    send_entire_file(new_socket, coms);
  } else if (request_code == 2) {
    recieve_update(coms, new_socket);
  }
  close(new_socket);
  return 0;
}

uint8_t send_entire_file(int socket, coms_t *coms) {
  uint8_t trans_cnt = ((hash_t*) coms->ip_hash)->size();
  if (send(socket, &(trans_cnt), 1, 0) < 0) {
    //    perror("Send()");
    // exit(7);
  }
  for (const auto & [ domain, ip ] : *((hash_t*) coms->ip_hash)) {
    uint8_t ack;
    if (recv(socket, &ack, 1, 0) < 0) return 1;
    if (send(socket, (uint8_t*) (domain + " " + ip).c_str(), (domain + " " + ip).length() + 1, 0) < 0) {
      perror("Send()");
      exit(7);
    }
  }
  uint8_t ack;
  if (recv(socket, &ack, 1, 0) < 0) return 1;
  send(socket, &(coms->version_num), 1, 0);
  return 0;
}

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
  }
  c->version_num = 0;
  c->tcp_resp_thread = (pthread_t*) malloc(sizeof(pthread_t));
  setup_response_thread(c->tcp_resp_thread, &check_hosts_requests, (void*) c);
  return c;
}

uint8_t destroy_coms(coms_t *coms) {
  kill_response_thread(coms->tcp_resp_thread);
  sleep(3);
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

uint8_t request_hosts(coms_t *coms, uint16_t port, char *ip) {
  ((hash_t*) coms->ip_hash)->clear();
  int new_socket = setup_server(0, SOCK_STREAM, false);
  connect_to_tcp(new_socket, ip, port);

  uint8_t request_code = 1; // Saying whole file
  if (send(new_socket, &request_code, 1, 0) < 0) {
    perror("Send()");
    exit(7);
  }

  // Recieve all data
  uint8_t trans_cnt;
  recv(new_socket, &trans_cnt, 1, 0);
  uint8_t buffer[128];
  for (int i = 0; i < trans_cnt; ++i) {
    uint8_t ack = 1;
    if (send(new_socket, &(ack), 1, 0) < 0) {
      perror("Send()");
      exit(7);
    }
    uint8_t *curr_loc = buffer - 1;
    do {
      int bytes;
      if ((bytes = recv(new_socket, buffer, sizeof(buffer), 0)) < 0) {
        close(new_socket);
        return 1;
      }
      curr_loc += bytes;
    } while (*curr_loc != 0);

    char *domain = (char*) buffer;
    char *ip = (char*) buffer;
    do {
      ++ip;
    } while (*ip != ' ');
    *ip = (char) 0;
    ++ip;
    (*((hash_t*) coms->ip_hash))[std::string(domain)] = std::string(ip);
  }
  uint8_t ack = 1;
  if (send(new_socket, &(ack), 1, 0) < 0) {
    perror("Send()");
    exit(7);
  }

  uint8_t ver;
  recv(new_socket, &ver, 1, 0);
  coms->version_num = ver;
  close(new_socket);
  return 0;
}

uint8_t recieve_update(coms_t *coms, int socket) {
  // Ack the request
  uint8_t ack = coms->version_num;
  if (send(socket, &(ack), 1, 0) < 0) {
    perror("Send()");
    exit(7);
  }
  uint8_t buffer[128];
  uint8_t *curr_loc = buffer - 1;
  struct timeval tv;
  tv.tv_sec = 2;
  tv.tv_usec = 0;
  printf("%d\n", setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv));
  do {
    int bytes;
    if ((bytes = recv(socket, buffer, sizeof(buffer), 0)) < 0) {
      close(socket);
      printf("cacneed\n");
      return 1;
    }
    curr_loc += bytes;
  } while (*curr_loc != 0);
  bool remove = *buffer == '1';
  char *domain = (char*) buffer;
  do {
    ++domain;
  } while (*domain != ' ');
  *domain = 0;
  ++domain;
  char *ip =  domain;
  do {
    ++ip;
  } while (*ip != ' ');
  *ip = 0;
  ++ip;
  char *id = ip;
  do {
    ++id;
  } while (*id != ' ');
  *id = 0;
  ++id;

  return actually_update(coms, remove, domain, ip, *id);
}

uint8_t update_hosts(coms_t *coms, char *router_ip, char *server_ip, bool remove, char *domain, char *ip, uint16_t own_port, uint8_t id) {
  int router_socket = setup_server(0, SOCK_STREAM, false);
  connect_to_tcp(router_socket, router_ip, ROUTER_TCP_PORT_NUM);

  // Get list of all other servers
  uint8_t port_cnt;
  recv(router_socket, &port_cnt, 1, 0);
  uint16_t *ports = (uint16_t*) malloc(port_cnt * sizeof(uint16_t));
  for (int i = 0; i < port_cnt; ++i) {
    uint8_t ack = 1;
    if (send(router_socket, &(ack), 1, 0) < 0) {
      perror("Send()");
      exit(7);
    }
    if (recv(router_socket, (uint8_t*) &(ports[i]), sizeof(uint16_t), 0) == 1) {
      recv(router_socket, ((uint8_t*) &(ports[i])) + 1, 1, 0);
    }
  }

  bool some_stale = false;
  bool some_fresh = false;
  bool this_stale = !(coms->version_num == id);
  // Send out req to all, if don't get ack from all, abort
  for (int i = 0; i < port_cnt; ++i) {
    if (ports[i] == own_port) continue;
    int new_socket = setup_server(0, SOCK_STREAM, false);
    connect_to_tcp(new_socket, server_ip, ports[i]);

    uint8_t request_code = 2;
    send(new_socket, (uint8_t*) &request_code, 1, 0);
    // Let this timeout, if so, continue but return
    uint8_t ack;
    if (recv(new_socket, &ack, 1, 0) < 0) return 1;
    if (ack == id) {
      some_fresh = true;
      close(new_socket);
      continue;
    } else {
      some_stale = true;
    }

    std::string id_str = std::to_string(id);
    send(new_socket, (uint8_t*) ((remove ? "1" : "0") + std::string(" ") + std::string(domain) + " " + std::string(ip) + " " + id_str).c_str(), strlen(domain) + strlen(ip) + id_str.size() + 5, 0);
    // Let this timeout, if so, continue but return
    if (recv(new_socket, &ack, 1, 0) < 0) return 1;
    close(new_socket);
  }
  if (this_stale)
    actually_update(coms, remove, domain, ip, id);

  uint8_t buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  send(router_socket, buffer, sizeof(dns_server_t*), 0);
  close(router_socket);

  if (this_stale && !some_fresh) {
    return 0;
  } else if (!this_stale && some_stale) {
    return 9;
  } else if (this_stale && some_fresh) {
    return 8;
  } else if (!this_stale && !some_stale) {
    return 6;
  }
  return 10;

  // If return fails, then go to next server, and query if everyone committed
  // If no one comitted, return false, and restart
  // If true, then have that server broadcast out change to everyone who didn't get it yet
}

uint8_t actually_update(coms_t *coms, bool remove, char *domain, char *ip, uint8_t id) {
  coms->version_num = id;
  if (remove) {
    ((hash_t*) coms->ip_hash)->erase(std::string(domain));
  } else {
    (*((hash_t*) coms->ip_hash))[std::string(domain)] = std::string(ip);
  }
  return write_hosts_file(coms);
}

/**
 * Write out ip_hash to hosts.txt.
 *
 * Will do every-time on update (rather than finding the line to modify)
 */
uint8_t write_hosts_file(coms_t *coms) {
  hash_t *translations = (hash_t*) coms->ip_hash;
  std::ofstream outfile("hosts" + std::to_string(coms->id) + ".txt");
  for (const auto & [ domain, ip ] : *((hash_t*) coms->ip_hash)) {
    outfile << domain << " " << ip << "\n";
  }
  return 0;
}

}
