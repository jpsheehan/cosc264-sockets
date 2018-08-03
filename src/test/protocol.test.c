#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../protocol.h"
#include "../utils.h"

int main(void)
{
    uint16_t failures = 0;

    size_t pktResult = 0;

    // ** dtReq **
    // create a request packet with a size too small
    size_t smallResPktLen = REQ_PKT_LEN - 1;
    uint8_t smallResPkt[smallResPktLen] = {0};
    
    if (dtReq(smallResPkt, smallResPktLen, REQ_DATE) != 0) {
        failures++;
        fail("dtReq", "n is too small");
    }

    // create a request packet with a size too large
    size_t largeResPktLen = REQ_PKT_LEN + 1;
    uint8_t largeResPkt[largeResPktLen] = {0};

    if (dtReq(largeResPkt, largeResPktLen, REQ_DATE) != 0) {
        failures++;
        fail("dtReq", "n is too large");
    }

    // create a request packet with an invalid reqType
    uint8_t reqPktDate[REQ_PKT_LEN] = {0};

    if (dtReq(reqPktDate, REQ_PKT_LEN, 99) != 0) {
        failures++;
        fail("dtReq", "invalid reqType");
    }

    // create a valid request packet
    if (dtReq(reqPktDate, REQ_PKT_LEN, REQ_DATE) != REQ_PKT_LEN) {
        failures++;
        fail("dtReq", "reqType is valid");
    }

    uint8_t reqPktTime[REQ_PKT_LEN] = {0};

    if (dtReq(reqPktTime, REQ_PKT_LEN, REQ_TIME) != REQ_PKT_LEN) {
        failures++;
        fail("dtReq", "reqType is valid");
    }

    // ** dtReqType **
    // check that the dtReqType is returned
    if (dtReqType(reqPktDate, REQ_PKT_LEN) != REQ_DATE) {
        failures++;
        fail("dtReqType", "REQ_DATE not returned");
    }

    if (dtReqType(reqPktTime, REQ_PKT_LEN) != REQ_TIME) {
        failures++;
        fail("dtReqType", "REQ_TIME not returned");
    }

    // ** dtReqValid **
    // check with a packet with a size too small
    if (dtReqValid(smallReqPkt, smallReqPktLen)) {
        failures++;
        fail("dtReqValid", "n should be too small");
    }

    // check with a packet with a size too large
    if (dtReqValid(largeReqPkt, largeReqPktLen)) {
        failures++;
        fail("dtReqValid", "n should be too large");
    }

    // check with an invalid magic number
    uint8_t badMagicNoReqPkt[REQ_PKT_LEN] = {0xDE, 0xAD, 0x00, 0x01, 0x00, 0x01};
    if (dtReqValid(badMagicNoReqPkt, REQ_PKT_LEN)) {
        failures++;
        fail("dtReqValid", "magic no should be incorrect");
    }

    // check with an invalid packet type
    uint8_t badPktTypeReqPkt[REQ_PKT_LEN] = {0x49, 0x7E, 0x99, 0x88, 0x00, 0x00, 0x01};
    if (dtReqValid(badPktTypeReqPkt, REQ_PKT_LEN)) {
        failures++;
        fail("dtReqValid", "pktType should be incorrect");
    }

    // check with an invalid request type
    uint8_t badReqTypeReqPkt[REQ_PKT_LEN] = {0x49, 0x7E, 0x000, 0x01, 0xDE, 0xAD};
    if (dtReqValid(badReqTypeReqPkt, REQ_PKT_LEN)) {
        failures++;
        fail("dtReqValid", "reqType should be incorrect");
    }

    // check with a valid packet
    if (!dtReqValid(reqPktTime, REQ_PKT_LEN)) {
        failures++;
        fail("dtReqValid", "time packet should be correct");
    }

    if (!dtReqValid(reqPktDate, REQ_PKT_LEN)) {
        failures++;
        fail("dtReqValid", "date packet should be correct");
    }

    // ** dtPktMagicNo **
    // check that the magic number is extracted
    if (dtPktMagicNo(reqPktDate, REQ_PKT_LEN) != MAGIC_NO) {
        failures++;
        fail("dtPktMagicNo", "magic no should be correct");
    }

    // ** dtPktType **
    // check that the packet type is extracted
    if (dtPktType(reqPktDate, REQ_PKT_LEN) != PACKET_REQ) {
        failures++;
        fail("dtPktType", "packet type should be correct");
    }

    // ** validReqType **
    // check with a valid request type
    if (!validReqType(REQ_DATE)) {
        failures++;
        fail("validReqType", "REQ_DATE should be correct");
    }

    if (!validReqType(REQ_TIME)) {
        failures++;
        fail("validReqType", "REQ_TIME should be correct");
    }

    // check with an invalid request type
    if (validReqType(0xBEEF)) {
        failures++;
        fail("validReqType", "0xBEEF should be incorrect");
    }

    // ** validLangCode **
    // check with valid lang codes
    if (!validLangCode(LANG_ENG)) {
        failures++;
        fail("validLangCode", "LANG_ENG should be correct");
    }

    if (!validLangCode(LANG_GER)) {
        failures++;
        fail("validLangCode", "LANG_GER should be correct");
    }

    if (!validLangCode(LANG_MAO)) {
        failures++;
        fail("validLangCode", "LANG_MAO should be correct");
    }

    // check with an invalid lang code
    if (validLangCode(0xBEEF)) {
        failures++;
        fail("validLangCode", "0xBEEF should be incorrect");
    }

    // ** dtRes **
    // create a packet with a size too small

    // create a packet with a size too large

    // create a packet with an invalid request type

    // create a packet with an invalid language code

    // create valid packets and check that the size returned is correct

    // ** dtResValid **
    // check with a packet that is too small

    // check with a packet with an invalid magic number

    // check with a packet with an invalid packet type

    // check with a packet with an invalid year

    // check with a packet with a month too small

    // check with a packet with a month too large

    // check with a packet with a day too small

    // check with a packet with a day too large

    // check with a packet with an hour too small

    // check with a packet with an hour too large

    // check with a packet with a minute too small

    // check with a packet with a minute too large

    // check with a packet with an invalid length

    // check with valid packets

    // ** dtResLangCode **
    // check that the lang code is extracted

    // ** dtResYear **
    // check that the year is extracted

    // ** dtResMonth **
    // check that the month is extracted

    // ** dtResDay **
    // check that the day is extracted

    // ** dtResHour **
    // check that the hour is extracted

    // ** dtResMinute **
    // check that the minute is extracted

    // ** dtResLength **
    // check that the length is extracted

    // ** dtResText **
    // check that the text is extracted and that the length is ok

    return failures;
}