/**
 * Library for basic DNS message crafting and parsing.
 *
 * Tailored for the specific needs of this project.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**
 * A header for an DNS message.
 */
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

/**
 * A single question section for a DNS message.
 */
typedef struct {
  char *domain;
  uint16_t qtype;
  uint16_t qclass;
} question_t;

/**
 * A single answer section for a DNS message.
 *
 * Technically works for all resourse records.
 */
typedef struct {
  char *ip;
  uint16_t type;
  uint16_t class;
  uint32_t ttl;
  uint8_t rdlength;
  uint8_t *rdata;
} answer_t;

/**
 * Format a message from the given information.
 *
 * @param buffer: buffer to fill with the message
 * @param query: is this a query or a response
 * @param id: ID of the message
 * @param domain: domain name of the message
 * @param domain: IP address if a response
 *
 * @returns Length of message, 0 if error
 */
uint8_t craft_message(uint8_t *buffer, bool query, uint16_t id, char *domain, char *ip);

/**
 * Parse a DNS message.
 *
 * @param message: DNS message to parse
 * @param header: header information
 * @param id: ID of the message
 * @param domain: domain name of the message
 * @param domain: IP address if a response
 *
 * @returns Error code, 0 if successful
 */
uint8_t parse_message(uint8_t *message, header_t *header, question_t *question, answer_t *answer);
