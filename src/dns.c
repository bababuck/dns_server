#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/dns.h"

uint8_t craft_message(uint8_t *buffer, bool query, uint16_t id, char *domain, const char *ip) {
  uint8_t *curr = buffer;
  uint8_t bytes = 0;

  // Header
  header_t h = {
    .id = htons(id),
    .qr = !query,
    .opcode = 0,
    .aa = 0,
    .tc = 0,
    .rd = 1,
    .ra = 0,
    .z = 0,
    .rcode = !query && ip == NULL ? 0x3 : 0x0,
    .qdcount = 0x100,
    .ancount = 0,
    .nscount = 0,
    .arcount = 0};
  memcpy(buffer, &h, sizeof(header_t));
  //  printf("%x %x %x %x %x %x\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
  curr += sizeof(header_t);
  bytes += sizeof(header_t);

  // Question
  memcpy(curr, domain, sizeof(char) * (strlen(domain) + 1));
  curr += strlen(domain) + 1;
  bytes += strlen(domain) + 1;
  curr[1] = 1;
  curr[3] = 1;
  //  printf("%s\n", curr);
  curr += 4;
  bytes += 4;

  // Answer
  /*
    char *ip;
  uint16_t type;
  uint16_t dclass;
  uint32_t ttl;
  uint8_t rdlength;
  uint8_t *rdata;
  */
  return bytes;
}

void flip_header_endian(header_t *h) {
  h->id = ntohs(h->id);
  h->qdcount = ntohs(h->qdcount);
  h->ancount = ntohs(h->ancount);
  h->nscount = ntohs(h->nscount);
  h->arcount = ntohs(h->arcount);
}

uint8_t parse_message(uint8_t *buffer, message_t *message, uint8_t message_bytes) {
  if (message == NULL) {
    return 1;
  }
  if (message_bytes < sizeof(header_t)) { // Header is 12 bytes
    return 2;
  }
  memcpy(&(message->header), buffer, sizeof(header_t));
  flip_header_endian(&(message->header));
  buffer += sizeof(header_t);

  uint8_t char_cnt = 0;
  while (*(buffer + char_cnt) != 0 && char_cnt < (message_bytes - 12)) {
    ++char_cnt;
  }
  if (char_cnt + 4 > (message_bytes - 12)) {
    return 3;
  }

  message->question.domain = (char*) malloc((char_cnt + 1) * sizeof(char));
  memcpy(message->question.domain, buffer, (char_cnt + 1) * sizeof(char));
  buffer += char_cnt + 1;
  message->question.qtype = ntohs(*((uint16_t*) buffer));
  message->question.qclass = ntohs(*((uint16_t*) buffer));
  // TODO: Repeat if multiple questions

  // TODO: Get Answer

  return 0;
}
