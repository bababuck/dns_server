#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  uint16_t id : 16;
  uint8_t rd : 1;
  uint8_t tc : 1;
  uint8_t aa : 1;
  uint8_t opcode : 4;
  uint8_t qr : 1;
  uint8_t rcode : 4;
  uint8_t z : 3;
  uint8_t ra : 1;
  uint16_t qdcount : 16;
  uint16_t ancount : 16;
  uint16_t nscount : 16;
  uint16_t arcount : 16;
} header_t;

typedef struct {
  char *domain;
  uint16_t qtype;
  uint16_t qclass;
} question_t;

void craft_message(uint8_t *buffer) {
  //  uint8_t buffer[64];  // 512 bits
  uint8_t *curr = buffer;
  header_t h = {
    .id = 0x69,
    .qr = 0,
    .opcode = 0,
    .aa = 0,
    .tc = 0,
    .rd = 1,
    .ra = 0,
    .z = 0,
    .qdcount = 0x100,
    .ancount = 0,
    .nscount = 0,
    .arcount = 0};
  memcpy(buffer, &h, sizeof(header_t));
  printf("%x %x %x %x %x %x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
  char domain[] = {(char) 6, 'g', 'o', 'o', 'g', 'l', 'e', (char) 3, 'c', 'o', 'm', (char) 0};
  curr += sizeof(header_t);
  memcpy(curr, domain, sizeof(domain));
  question_t q = {domain, 1, 1};
  curr += sizeof(domain);
  curr[1] = 1;
  curr[3] = 1;
  printf("%s\n", curr);
  return ;
}
