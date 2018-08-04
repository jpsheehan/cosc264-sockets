#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../protocol.h"
#include "../utils.h"

int main(void)
{
    uint16_t failures = 0;

    // ** dtReq **
    // create a request packet with a size too small
    size_t smallReqPktLen = REQ_PKT_LEN - 1;
    uint8_t smallReqPkt[smallReqPktLen];
    
    if (dtReq(smallReqPkt, smallReqPktLen, REQ_DATE) != 0) {
        failures++;
        fail("dtReq", "n is too small");
    }

    // create a request packet with a size too large
    size_t largeReqPktLen = REQ_PKT_LEN + 1;
    uint8_t largeReqPkt[largeReqPktLen];

    if (dtReq(largeReqPkt, largeReqPktLen, REQ_DATE) != 0) {
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
    uint8_t badPktTypeReqPkt[REQ_PKT_LEN] = {0x49, 0x7E, 0x99, 0x88, 0x00, 0x01};
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
    size_t smallResPktLen = RES_PKT_LEN - 1;
    uint8_t smallResPkt[smallResPktLen];
    if (dtRes(smallResPkt, smallResPktLen, REQ_DATE, LANG_ENG, 2018, 6, 10, 12, 45) == smallResPktLen) {
        failures++;
        fail("dtRes", "n should be too small");
    }

    // create a packet with a size too large
    size_t largeResPktLen = RES_PKT_LEN + 1;
    uint8_t largeResPkt[largeResPktLen];
    if (dtRes(largeResPkt, largeResPktLen, REQ_DATE, LANG_ENG, 2018, 6, 10, 12, 45) == largeResPktLen) {
        failures++;
        fail("dtRes", "n should be too large");
    }

    // create a packet with an invalid request type
    uint8_t badReqTypeResPkt[RES_PKT_LEN] = {0};
    if (dtRes(badReqTypeResPkt, RES_PKT_LEN, 0xBEEF, LANG_ENG, 2018, 6, 10, 12, 45) == RES_PKT_LEN) {
        failures++;
        fail("dtRes", "reqType should be invalid");
    }

    // create a packet with an invalid language code
    uint8_t badLangCodeResPkt[RES_PKT_LEN] = {0};
    if (dtRes(badLangCodeResPkt, RES_PKT_LEN, REQ_TIME, 0xBEEF, 2018, 6, 10, 12, 45) == RES_PKT_LEN) {
        failures++;
        fail("dtRes", "langCode should be invalid");
    }

    // create valid packets and check that the size returned is correct
    uint8_t dateEngResPkt[RES_PKT_LEN] = {0};
    if (dtRes(dateEngResPkt, RES_PKT_LEN, REQ_DATE, LANG_ENG, 2018, 6, 10, 12, 45) != 13 + 29) {
        failures++;
        fail("dtRes", "english date packet isn't the correct length");
        dtPktDump(dateEngResPkt, RES_PKT_LEN);
    }

    uint8_t timeEngResPkt[RES_PKT_LEN] = {0};
    if (dtRes(timeEngResPkt, RES_PKT_LEN, REQ_DATE, LANG_ENG, 2018, 6, 10, 12, 45) != 13 + 0) {
        failures++;
        fail("dtRes", "english time packet isn't the correct length");
        dtPktDump(timeEngResPkt, RES_PKT_LEN);
    }

    // ** dtResValid **
    // check with a packet that is too small
    if (dtResValid(smallResPkt, smallResPktLen)) {
        failures++;
        fail("dtResValid", "n should be too small");
    }

    // check with a packet that is too large
    if (dtResValid(largeResPkt, largeResPktLen)) {
        failures++;
        fail("dtResValid", "n should be too large");
    }

    // check with a packet with an invalid magic number
    timeEngResPkt[0] = 0xBE;
    timeEngResPkt[1] = 0xEF;
    if (dtResValid(timeEngResPkt, RES_PKT_LEN)) {
        failures++;
        fail("dtResValid", "magic number should be invalid");
    }
    timeEngResPkt[0] = (uint8_t)(MAGIC_NO >> 8);
    timeEngResPkt[1] = (uint8_t)(MAGIC_NO & 0xFF);

    // check with a packet with an invalid packet type
    timeEngResPkt[2] = 0xBE;
    timeEngResPkt[3] = 0xEF;
    if (dtResValid(timeEngResPkt, RES_PKT_LEN)) {
        failures++;
        fail("dtResValid", "packet type should be invalid");
    }
    timeEngResPkt[2] = (uint8_t)(PACKET_RES >> 8);
    timeEngResPkt[3] = (uint8_t)(PACKET_RES & 0xFF);

    // check with a packet with an invalid year
    timeEngResPkt[6] = 0xBE;
    timeEngResPkt[7] = 0xEF;
    if (dtResValid(timeEngResPkt, RES_PKT_LEN)) {
        failures++;
        fail("dtResValid", "year should be invalid");
    }
    timeEngResPkt[6] = (uint8_t)(2018 >> 8);
    timeEngResPkt[7] = (uint8_t)(2018 & 0xFF);

    // check with a packet with a month too small
    timeEngResPkt[8] = 0;
    if (dtResValid(timeEngResPkt, RES_PKT_LEN)) {
        failures++;
        fail("dtResValid", "month should be too small");
    }

    // check with a packet with a month too large
    timeEngResPkt[8] = 13;
    if (dtResValid(timeEngResPkt, RES_PKT_LEN)) {
        failures++;
        fail("dtResValid", "month should be too large");
    }
    timeEngResPkt[8] = (uint8_t)(6);

    // check with a packet with a day too small
    timeEngResPkt[9] = 0;
    if (dtResValid(timeEngResPkt, RES_PKT_LEN)) {
        failures++;
        fail("dtResValid", "day should be too small");
    }

    // check with a packet with a day too large
    timeEngResPkt[9] = 32;
    if (dtResValid(timeEngResPkt, RES_PKT_LEN)) {
        failures++;
        fail("dtResValid", "day should be too large");
    }
    timeEngResPkt[9] = (uint8_t)(10);

    // check with a packet with an hour too large
    timeEngResPkt[10] = 24;
    if (dtResValid(timeEngResPkt, RES_PKT_LEN)) {
        failures++;
        fail("dtResValid", "hour should be too large");
    }
    timeEngResPkt[10] = (uint8_t)(12);

    // check with a packet with a minute too large
    timeEngResPkt[11] = 60;
    if (dtResValid(timeEngResPkt, RES_PKT_LEN)) {
        failures++;
        fail("dtResValid", "minute should be too large");
    }
    timeEngResPkt[11] = (uint8_t)(45);

    // check with a packet with an invalid length

    // check with valid packets
    if (!dtResValid(timeEngResPkt, RES_PKT_LEN)) {
        failures++;
        fail("dtResValid", "english time packet should be valid");
    }
    
    if (!dtResValid(dateEngResPkt, RES_PKT_LEN)) {
        failures++;
        fail("dtResValid", "english date packet should be valid");
    }

    // ** dtResLangCode **
    // check that the lang code is extracted
    if (dtResLangCode(dateEngResPkt, RES_PKT_LEN) != LANG_ENG) {
        failures++;
        fail("dtResLangCode", "lang code is not extracted");
    }

    // ** dtResYear **
    // check that the year is extracted
    if (dtResYear(dateEngResPkt, RES_PKT_LEN) != 2018) {
        failures++;
        fail("dtResYear", "year is not extracted");
    }

    // ** dtResMonth **
    // check that the month is extracted
    if (dtResMonth(dateEngResPkt, RES_PKT_LEN) != 6) {
        failures++;
        fail("dtResMonth", "month is not extracted");
    }

    // ** dtResDay **
    // check that the day is extracted
    if (dtResDay(dateEngResPkt, RES_PKT_LEN) != 10) {
        failures++;
        fail("dtResDay", "day is not extracted");
    }

    // ** dtResHour **
    // check that the hour is extracted
    if (dtResHour(dateEngResPkt, RES_PKT_LEN) != 12) {
        failures++;
        fail("dtResHour", "hour is not extracted");
    }

    // ** dtResMinute **
    // check that the minute is extracted
    if (dtResMinute(dateEngResPkt, RES_PKT_LEN) != 45) {
        failures++;
        fail("dtResMinute", "minute is not extracted");
    }

    // ** dtResLength **
    // check that the length is extracted
    if (dtResLength(dateEngResPkt, RES_PKT_LEN) != 28) {
        failures++;
        fail("dtResLength", "length is not extracted");
    }

    // ** dtResText **
    // check that the text is extracted and that the length is ok
    char text[RES_TEXT_LEN] = {0};
    size_t textLen = 0;
    dtResText(dateEngResPkt, RES_PKT_LEN, text, &textLen);

    for (int i = 0; i < textLen - 1; i++) {
        if (text[i] != dateEngResPkt[13 + i]) {
            failures++;
            fail("dtResText", "text is not extracted");
        }
    }

    return failures;
}
