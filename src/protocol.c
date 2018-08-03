// protocol.c

#include "protocol.h"

const char* PHRASES[3][2] = {
    { "Today's date is %s %d, %d", "The current time is %d:%d" },
    { "Ko te ra o tenei ra ko %s %d, %d", "Ko te wa o tenei wa %d:%d" },
    { "Heute ist der %d. %s %d", "Die Uhrzeit ist %d:%d" }
};

const char* MONTHS[3][12] = {
    { "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"},
    { "Kohit\u0101tea", "Hui-tanguru", "Pout\u016B-te-rangi", "Paenga-wh\u0101wh\u0101", "Haratua", "Pipiri",
        "H\u014Dngongoi", "Here-turi-k\u014Dk\u0101", "Mahuru", "Whiringa-\u0101-nuku", "Whiringa-\u0101-rangi", "Hakihea" },
    { "Januar", "Februar", "M\u00E4rz", "April", "Mai", "Juni",
        "Juli", "August", "September", "Oktober", "November", "Dezember" }
};

/**
 * Creates a DT Request packet and puts it into a uint8_t array.
 * 
 * The packet array must be REQ_PKT_LEN long otherwise
 * you risk a buffer overflow.
 * 
 * @param pkt A pointer to the packet.
 * @param n The size of the array. Must be REQ_PKT_LEN.
 * @param reqType Must be REQ_DATE or REQ_TIME.
 * @return true if successful.
 * */
bool dtReq(uint8_t pkt[], size_t n, uint16_t reqType)
{

    if (validReqType(reqType) && n == REQ_PKT_LEN) {

        pkt[0] = (uint8_t)(MAGIC_NO >> 8);
        pkt[1] = (uint8_t)(MAGIC_NO & 0xFF);
        pkt[2] = (uint8_t)(PACKET_REQ >> 8);
        pkt[3] = (uint8_t)(PACKET_REQ & 0xFF);
        pkt[4] = (uint8_t)(reqType >> 8);
        pkt[5] = (uint8_t)(reqType & 0xFF);

        return true;

    }

    return false;
}

/**
 * Returns the request type of a DT Request packet.
 * 
 * @param pkt An array of uint8 values making up the packet.
 * @param n The size of the array. Must be REQ_PKT_LEN.
 * @return The request type of the packet, or 0 for an error.
 * */
uint16_t dtReqType(uint8_t pkt[], size_t n)
{
    if (n != REQ_PKT_LEN || !validPkt(pkt, n)) {
        return 0x0000;
    }

    uint16_t reqType = (pkt[4] << 8) | pkt[5];

    if (reqType != REQ_DATE || reqType != REQ_TIME) {
        reqType = 0x0000;
    }

    return reqType;
}

/**
 * Checks the magic number of a packet.
 * 
 * @param pkt The packet.
 * @param n The length of the packet.
 * */
bool validPkt(uint8_t  pkt[], size_t n)
{
    if (n < 2) {
        return false;
    }
    return ((pkt[0] << 8) | pkt[1] == MAGIC_NO);
}

bool validReqType(uint16_t reqType)
{
    return (reqType == REQ_DATE || reqType == REQ_TIME);
}

bool validLangCode(uint16_t langCode)
{
    return (langCode == LANG_ENG || langCode == LANG_GER || langCode == LANG_MAO);
}

bool dtRes(uint8_t pkt[], size_t n, uint16_t reqType, uint16_t langCode, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t length)
{
    // check the size n

    // check reqType and langCode
    if (!validReqType(reqType) || !validLangCode(langCode)) {
        return false;
    }

    pkt[0] = (uint8_t)(MAGIC_NO >> 8);
    pkt[1] = (uint8_t)(MAGIC_NO & 0xFF);
    pkt[2] = (uint8_t)(PACKET_RES >> 8);
    pkt[3] = (uint8_t)(PACKET_RES & 0xFF);
    pkt[4] = (uint8_t)(langCode >> 8);
    pkt[5] = (uint8_t)(langCode & 0xFF);
    pkt[6] = (uint8_t)(year >> 8);
    pkt[7] = (uint8_t)(year & 0xFF);
    pkt[8] = month;
    pkt[9] = day;
    pkt[10] = hour;
    pkt[11] = minute;
    pkt[12] = length;

    char phrase[] = PHRASES[langCode - 1][reqType - 1];
    char monthStr[] = MONTHS[langCode - 1][month - 1];

    

}