#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Protocol definitions
#define MAGIC_NO 0x497E
#define PACKET_REQ 0x0001
#define PACKET_RES 0x0002

#define REQ_DATE 0x0001
#define REQ_TIME 0x0002
#define REQ_PKT_LEN 6

#define RES_TEXT_LEN 64
#define RES_PKT_LEN (13 + RES_TEXT_LEN)

// Language code definitions
#define LANG_ENG 0x0001
#define LANG_MAO 0x0002
#define LANG_GER 0x0003

// Helper functions
bool validLangCode(uint16_t langCode);
bool validReqType(uint16_t reqType);

// DT Request functions
bool dtReq(uint8_t pkt[], size_t n, uint16_t reqType);
uint16_t dtReqType(uint8_t pkt[], size_t n);
bool dtReqValid(uint8_t pkt[], size_t n);

// DT Response functions
bool dtRes(uint8_t pkt[], size_t n, uint16_t reqType, uint16_t langCode, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute);

#endif