#include <chrono>
#include <mutex>
#include <deque>

//#include "dns.h"
#include "../include/scoreboard.h"

typedef struct {
  double time;
  uint8_t id;
} results_t;

const std::chrono::time_point<std::chrono::steady_clock> start_time;

void init_scoreboard() {
  start_time = std::chrono::steady_clock::now();
}

uint8_t recieve_generated_req(scoreboard_t *s, uint8_t id) {
  auto curr_time = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = curr_time - start_time;

  const std::lock_guard<std::mutex> lock(*((std::mutex) s->mutex));
  ((std::deque<results_t>) (s->queue)).push_back({elapsed_seconds.count(), id});
  return 0;
}

uint8_t recieve_dns_answer(scoreboard_t *s, uint8_t *message) {
  auto curr_time = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = curr_time - start_time;
  const std::lock_guard<std::mutex> lock(*((std::mutex) s->mutex));
  for (auto it = deque.cbegin(); it != deque.cend(); ++it) {
    //    if (it->id == header->id) {
      it->time = elapsed_seconds.count() - it->time;
      return 0;
    }
  //  }
  return 1;
}

scoreboard_t* create_scoreboard(char *testname) {
  scoreboard_t* s = malloc(sizeof(scoreboard_t));
  s->testname = testname;
  s->lock = (void*) new std::mutex();
  s->queue = (void*) new std::queue<results_t>();
}

uint8_t destroy_scoreboard(scoreboard_t *s) {
  free(s->testname);
  free(s->destaddr);
  delete s->mutex;
  delete s->queue;
}
