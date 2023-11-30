#include <stdint.h>
#include <chrono>
#include <mutex>
#include <deque>

extern "C" {

#include "../include/dns.h"
#include "../include/scoreboard.h"


typedef struct {
  double time;
  uint16_t id;
} results_t;

std::chrono::time_point<std::chrono::steady_clock> start_time;

uint8_t recieve_dns_answer(scoreboard_t *s, uint8_t id);

void init_scoreboard() {
  start_time = std::chrono::steady_clock::now();
}

uint8_t recieve_generated_req(scoreboard_t *s, uint16_t id) {
  printf("SCOREBOARD GENERATED=%d\n", id);
  const std::chrono::time_point<std::chrono::steady_clock>  curr_time = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = curr_time - start_time;

  const std::lock_guard<std::mutex> lock(*((std::mutex*) s->lock));
  ((std::deque<results_t>*) (s->queue))->push_back({elapsed_seconds.count(), id});
  return 0;
}

/**
 * Response handler to be run on the response handling thread.
 *
 * @params _scoreboard: Scoreboard to update results to
 */
void* dns_response_handler(void *_scoreboard) {
  scoreboard_t *scoreboard = (scoreboard_t*) _scoreboard;
  message_t dns_message;
  uint8_t buffer[MAX_DNS_BYTES];
  while (true) { // Run until thread killed
    uint8_t message_len = recieve_message(buffer, MAX_DNS_BYTES, scoreboard->socket);
    uint8_t error;
    if ((error = parse_message(buffer, &dns_message, message_len)) != 0) {
      perror("Parsing message failed.");
      exit(1);
    }
    printf("SCOREBOARD RECIEVED=%d\n", dns_message.header.id);
    recieve_dns_answer(scoreboard, dns_message.header.id);
  }
  return NULL;
}

uint8_t recieve_dns_answer(scoreboard_t *s, uint8_t id) {
  const std::chrono::time_point<std::chrono::steady_clock>  curr_time = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = curr_time - start_time;
  const std::lock_guard<std::mutex> lock(*((std::mutex*) s->lock));
  for (auto it = ((std::deque<results_t>*) (s->queue))->begin(); it != ((std::deque<results_t>*) (s->queue))->end(); ++it) {
    if (it->id == id) {
      it->time = elapsed_seconds.count() - it->time;
      return 0;
    }
  }
  return 1;
}

scoreboard_t* create_scoreboard(char *testname, uint16_t dns_port) {
  scoreboard_t* s = (scoreboard_t*) malloc(sizeof(scoreboard_t));
  s->testname = testname;
  s->lock = (void*) new std::mutex();
  s->queue = (void*) new std::deque<results_t>();
  s->dns_response_thread = (pthread_t*) malloc(sizeof(pthread_t));
  s->socket = setup_server(dns_port, SOCK_DGRAM);

  // Starting response thread must be done last
  setup_response_thread(s->dns_response_thread, &dns_response_handler, s);
  return s;
}

uint8_t destroy_scoreboard(scoreboard_t *s) {
  free(s->testname);
  //  free(s->destaddr);
  delete (std::mutex*) s->lock;
  delete (std::deque<results_t>*) s->queue;
  kill_response_thread(s->dns_response_thread);
  free(s->dns_response_thread);
  return 0;
}

}
