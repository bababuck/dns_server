#include "../include/router.h"
#include "../include/server.h"

/**
 * Function to continually check if servers are active.
 */
void* check_servers(void *router);

router_t* create_router(router_mode_t mode, rrl_t *rrl) {
  router_t *r = malloc(sizeof(router_t));
  r->mode = mode;
  r->rrl = rrl;
  r->servers = NULL;
  r->server_cnt = 0;
  r->curr_server = 0;
  r->socket = setup_server(ROUTER_PORT_NUM, SOCK_DGRAM);
  r->resp_thread = malloc(sizeof(pthread_t));
  r->mutex = malloc(sizeof(pthread_mutex_t));
  setup_response_thread(r->resp_thread, &check_servers, (void*) r);
  return r;
}

uint8_t destroy_router(router_t *router) {
  kill_response_thread(router->resp_thread);
  free(router->servers);
  free(router);
  return 0;
}

uint8_t add_dns_server(router_t *router, dns_server_t *dns) {
  pthread_mutex_lock(router->mutex);
  kill_response_thread(router->resp_thread);
  ++(router->server_cnt);
  router->servers = realloc(router->servers, router->server_cnt);
  router->servers[router->server_cnt - 1] = dns;
  setup_response_thread(router->resp_thread, &check_servers, (void*) router);
  pthread_mutex_unlock(router->mutex);
  return 0;
}

void* check_servers(void *router) {
  router_t *r = (router_t*) router;
  while (true) {
    query_response_time(router, 60);
  }
  return NULL;
}

uint8_t query_response_time(router_t *router, uint8_t allowed_seconds) {
  for (int i = 0; i < router->server_cnt; ++i) {
    uint8_t buffer[MAX_DNS_BYTES];
    char domain[] = "";
    uint8_t message_bytes = craft_message(/*buffer=*/buffer, /*query=*/true, /*id=*/1 << 15, /*domain=*/domain, /*ip=*/NULL);
    send_packet(/*port_num=*/router->servers[i]->port_num, /*ip=*/router->servers[i]->ip, /*socket_info=*/router->socket, buffer, message_bytes);
  }
  sleep(allowed_seconds);
  uint8_t *offline = malloc(router->server_cnt * sizeof(uint8_t));
  uint8_t cnt = router->server_cnt;
  for (int i = 0; i < router->server_cnt; ++i) {
    offline[i] = router->servers[i]->id;
  }
  uint8_t buffer[1];
  while (true) {
    int8_t len = recvfrom(router->socket, buffer, 1, MSG_DONTWAIT, NULL, NULL);
    if (len == -1) {
      break;
    }
    printf("Recieved query respoonse id=%d\n", *buffer);
    ++cnt;
    for (int i = 0; i < cnt; ++i) {
      if (*buffer == offline[i]) {
        for (int j = i + 1; j < cnt; ++j) {
          offline[j - 1] = offline[j];
        }
        --cnt;
        offline = realloc(offline, cnt * sizeof(uint8_t));
        break;
      }
    }
  }
  for (int i = 0; i < cnt; ++i) {
    remove_server(router, offline[i]);
  }
  return 0;
}

uint8_t forward_request(router_t *router, uint8_t *message, uint8_t message_len) {
  pthread_mutex_lock(router->mutex);
  uint8_t chosen_server;
  if (router->mode == ROUND_ROBIN) {
    chosen_server = router->curr_server;
    router->curr_server = (1 + router->curr_server) % router->server_cnt;
  } else if (router->mode == OVERLOAD) {
    if (router->curr_server >= router->server_cnt) {
      chosen_server = 0;
      router->curr_server = (1 + router->curr_server) % (router->server_cnt * 2);
    } else {
      chosen_server = router->curr_server;
      ++router->curr_server;
    }
  }
  printf("Forwarded to server %d\n", router->servers[chosen_server]->id);
  uint8_t error = send_packet(/*port_num=*/router->servers[chosen_server]->port_num, /*ip=*/router->servers[chosen_server]->ip, /*socket_info=*/router->socket, message, message_len);
  pthread_mutex_unlock(router->mutex);
  return error;
}

uint8_t remove_server(router_t *router, uint8_t dns_id) {
  pthread_mutex_lock(router->mutex);
  bool deleted = false;
  for (int i = 0; i < router->server_cnt; ++i) {
    if (deleted) {
      router->servers[i] = router->servers[i + 1];
    } else if (router->servers[i]->id == dns_id) {
      --i;
      --(router->server_cnt);
      deleted = true;
    }
  }
  if (!deleted) {
    return 1;
  }
  router->servers = realloc(router->servers, router->server_cnt * sizeof(dns_server_t*));
  pthread_mutex_unlock(router->mutex);
  return 0;
}
