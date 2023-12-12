#include <cstdio>
#include <stdint.h>
#include <chrono>
#include <mutex>
#include <deque>
#include <stdio.h>
extern "C" {

#include "../include/dns.h"
#include "../include/scoreboard.h"


typedef struct {
  double start_time;
  uint16_t id;
  uint8_t dns_id;
  uint8_t rrl_removed;
  uint8_t recieved;
  double finish_time;
  uint8_t found;
} results_t;

std::chrono::time_point<std::chrono::steady_clock> start_time;

  uint8_t recieve_dns_answer(scoreboard_t *s, uint16_t id, uint8_t valid_trans);

/**
 * Write an transaction result to an open file.
 *
 * @param file: file descriptor to write to.
 * @param results: Results to write out.
 *
 * @returns Error code, 0 if successful
 */
uint8_t write_results(FILE *file, results_t *results);

void init_scoreboard() {
  start_time = std::chrono::steady_clock::now();
}

double get_time_elapsed() {
  const std::chrono::time_point<std::chrono::steady_clock>  curr_time = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_time = curr_time - start_time;
  return elapsed_time.count();
}

uint8_t recieve_generated_req(scoreboard_t *s, uint16_t id, uint8_t dns_id, uint8_t rrl_removed, double elapsed_time) {
  results_t results = {.start_time=elapsed_time, .id=id, .dns_id=dns_id, .recieved=0, .rrl_removed=rrl_removed, .finish_time=0.0, .found=0};
  if (rrl_removed) {
    write_results((FILE*) s->results_file, &results);
    return 0;
  }
  const std::lock_guard<std::mutex> lock(*((std::mutex*) s->lock));
  ((std::deque<results_t>*) (s->queue))->push_back(results);
  //  printf("Scoreboard recieved generated test #%d\n", id);

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
    recieve_dns_answer(scoreboard, dns_message.header.id, dns_message.header.rcode == 0x3 ? 0 : 1);
    //    printf("Scoreboard recieved dns response #%d\n", dns_message.header.id);
  }
  return NULL;
}

  uint8_t recieve_dns_answer(scoreboard_t *s, uint16_t id, uint8_t valid_trans) {
  double elapsed_time = get_time_elapsed();
  const std::lock_guard<std::mutex> lock(*((std::mutex*) s->lock));
  bool found = false;
  for (auto it = ((std::deque<results_t>*) (s->queue))->begin(); it != ((std::deque<results_t>*) (s->queue))->end(); ++it) {
    if (it->id == id) {
      it->finish_time = elapsed_time;
      it->recieved = 1;
      found = true;
      it->found = valid_trans;
    }
  }
  while (!((std::deque<results_t>*) (s->queue))->empty() && (((std::deque<results_t>*) (s->queue))->front().recieved || ((std::deque<results_t>*) (s->queue))->size() > 0xFFFF)) {
    write_results((FILE*) s->results_file, &(((std::deque<results_t>*) (s->queue))->front()));
    ((std::deque<results_t>*) (s->queue))->pop_front();
  }
  if (found) {
    return 0;
  }
  //  fprintf(stderr, "DNS answer had no corresponding request! %d\n", ((std::deque<results_t>*) (s->queue))->size());
  return 1;
}

scoreboard_t* create_scoreboard(char *testname, uint16_t dns_port) {
  scoreboard_t* s = (scoreboard_t*) malloc(sizeof(scoreboard_t));
  s->testname = testname;
  s->lock = (void*) new std::mutex();
  s->queue = (void*) new std::deque<results_t>();
  s->dns_response_thread = (pthread_t*) malloc(sizeof(pthread_t));
  s->socket = setup_server(dns_port, SOCK_DGRAM, true);
  std::string results_filename = std::string(testname) + "_results.csv";
  s->results_file = (void*) fopen(results_filename.c_str(), "w");

  // Starting response thread must be done last
  setup_response_thread(s->dns_response_thread, &dns_response_handler, s);
  return s;
}

uint8_t destroy_scoreboard(scoreboard_t *s) {
  kill_response_thread(s->dns_response_thread);
  //  free(s->destaddr);
  while (!((std::deque<results_t>*) (s->queue))->empty()) {
    write_results((FILE*) s->results_file, &(((std::deque<results_t>*) (s->queue))->front()));
    ((std::deque<results_t>*) (s->queue))->pop_front();
  }
  delete (std::mutex*) s->lock;
  delete (std::deque<results_t>*) s->queue;
  free(s->dns_response_thread);
  return 0;
}

uint8_t write_results(FILE *file, results_t *results) {
  return (uint8_t) 0 <
    fprintf(file,
            "%.6f,%d,%d,%d,%d,%.6f,%d\n",
            results->start_time,
            results->id,
            results->dns_id,
            results->rrl_removed,
            results->recieved,
            results->finish_time,
            results->found);
}

}
