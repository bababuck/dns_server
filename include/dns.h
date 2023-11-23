/**
 *
 */

#pragma once

typdef struct {
  uint16_t id : 16;
  uint8_t qr : 1;
  uint8_t opcode : 4;
  uint8_t aa : 1;
  uint8_t tc : 1;
  uint8_t rd : 1;
  uint8_t ra : 1;
  uint8_t z : 3;
  uint8_t rcode : 4;
  uint16_t qdcount : 16;
  uint16_t ancount : 16;
  uint16_t nscount : 16;
  uint16_t arcount : 16;
} header_t;

typedef struct {
  uint8_t len;
  char* name;
  uint16_t type;
  uint16_t class;
} question_t;

typedef struct {
  question_t base;
  uint32_t ttl;
  uint16_t rdlength;
  uint8_t rdata;
} resource_t;

/**
 * Stores all information contained in DNS message
 */
typedef struct {
  header_t header;
  question_t question;
  answer_t answer;
  authority_t authority;
  additional_t additional;
} message_t;

/**
 * Function to parse a raw DNS message.
 */
bool parse_message(const uint8_t raw_message[8], message_t *message);

bool parse_header(const uint8_t raw_message[8], header_t *header) {
  header->id = ((uint16_t) raw_message][1]) & (((uint16_t) raw_message[0]) << 8);
  header->qr = raw_message[2] & 1;
if (header->qr) {
  return false;
  }
  uint8_t opcode : 4;
  uint8_t aa : 1;
  uint8_t tc : 1;
  uint8_t rd : 1;
  uint8_t ra : 1;
  uint8_t z : 3;
  uint8_t rcode : 4;
  uint16_t qdcount : 16;
  uint16_t ancount : 16;
  uint16_t nscount : 16;
  uint16_t arcount : 16;
}

bool write_header(uint8_t raw_message[8], header_t *header) {
  ((uint16_t*) raw_message) = header->id;
  raw_message[2] =
    header->qr << 7 &
    header->opcode << 3 &
    header->aa << 2 &
    header->tc << 1 &
    header->rd << 0;
  raw_message[3] =
    header->ra << 7 &
    header->z << 4 &
    header->rcode;
    ((uint16_t*) raw_message)[0] = header->id;
  raw_message
}

typedef char* ip_t;

typedef struct {

} dns_t;

/**
 * Perform a lookup into the dns for a domain name.
 *
 * If not found return NULL.
 */
ip_t lookup(dns_t *dns, domain_t name);
