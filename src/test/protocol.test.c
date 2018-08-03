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
    char smallResPkt[smallResPktLen] = {0};
    
    if (dtReq(smallResPkt, smallResPktLen, REQ_DATE) != 0) {
        failures += 1;
        fail("dtReq", "packet is too small");
    }

    // create a request packet with a size too large

    // create a request packet with an invalid reqType

    // create a valid request packet

    // ** dtReqType **
    // check that the dtReqType is returned

    // ** dtReqValid **
    // check with a packet with a size too small

    // check with a packet with a size too large

    // check with an invalid magic number

    // check with an invalid packet type

    // check with an invalid request type

    // check with a valid packet

    // ** dtPktMagicNo **
    // check that the magic number is extracted

    // ** dtPktType **
    // check that the packet type is extracted

    // ** validReqType **
    // check with an invalid request type

    // check with a valid request type

    // ** validLangCode **
    // check with an invalid lang code

    // check with valid lang codes

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