// protocol.c

#include <stdio.h>

#include "protocol.h"

// The phrases to send as a response. Written as templates to be filled with sprintf.
const char* PHRASES[3][2] = {
    { "Today's date is %s %d, %d", "The current time is %d:%d" },
    { "Ko te ra o tenei ra ko %s %d, %d", "Ko te wa o tenei wa %d:%d" },
    { "Heute ist der %d. %s %d", "Die Uhrzeit ist %d:%d" }
};

// The names of the months as strings. Some UTF codes are required for Maori and German.
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
 * @return The length of the packet.
 * */
size_t dtReq(uint8_t pkt[], size_t n, uint16_t reqType)
{

    if (validReqType(reqType) && n == REQ_PKT_LEN) {

        pkt[0] = (uint8_t)(MAGIC_NO >> 8);
        pkt[1] = (uint8_t)(MAGIC_NO & 0xFF);
        pkt[2] = (uint8_t)(PACKET_REQ >> 8);
        pkt[3] = (uint8_t)(PACKET_REQ & 0xFF);
        pkt[4] = (uint8_t)(reqType >> 8);
        pkt[5] = (uint8_t)(reqType & 0xFF);

        return n;

    }

    return 0;
}

/**
 * Returns the request type of a DT Request packet.
 * No checking is performed beforehand.
 * 
 * @param pkt An array of uint8 values making up the packet.
 * @param n The size of the array. Must be REQ_PKT_LEN.
 * @return The request type of the packet.
 * */
uint16_t dtReqType(uint8_t pkt[], size_t n)
{
    return ((pkt[4] << 8) | pkt[5]);
}

/**
 * Returns true if the packet is a valid DT Request packet.
 * 
 * @param pkt A pointer to the packet.
 * @param n The number of items in the packet.
 * @return True if the packet is valid. False otherwise.
 * */
bool dtReqValid(uint8_t pkt[], size_t n)
{
    if (n != REQ_PKT_LEN) {
        return false;
    }

    if (dtPktMagicNo(pkt, n) != MAGIC_NO) {
        return false;
    }

    if (dtPktType(pkt, n) != PACKET_REQ) {
        return false;
    }

    if (!validReqType(dtReqType(pkt, n))) {
        return false;
    }

    return true;
}

/**
 * Returns the magic number from the packet.
 * No checking is done beforehand.
 * 
 * @param pkt The packet.
 * @param n The size of the packet.
 * @return The magic number of the packet.
 * */
uint16_t dtPktMagicNo(uint8_t pkt[], size_t n)
{
    return ((pkt[0] << 8) | pkt[1]);
}

/**
 * Returns the type of packet.
 * No checking is done beforehand.
 * 
 * @param pkt The packet.
 * @param n The size of the packet.
 * @return The type of the packet.
 * */
uint16_t dtPktType(uint8_t pkt[], size_t n)
{
    return ((pkt[2] << 8) | pkt[3]);
}

/**
 * Returns true if reqType is a valid type of request.
 * 
 * @param reqType The request type.
 * @return True if reqType is either REQ_DATE or REQ_TIME.
 * */
bool validReqType(uint16_t reqType)
{
    return (reqType == REQ_DATE || reqType == REQ_TIME);
}

/**
 * Returns true if langCode denotes a valid language.
 * Valid languages codes are LANG_ENG for English,
 * LANG_MAO for Maori or LANG_GER for German.
 * 
 * @param langCode The language code.
 * @return True if langCode is valid.
 * */
bool validLangCode(uint16_t langCode)
{
    return (langCode == LANG_ENG || langCode == LANG_GER || langCode == LANG_MAO);
}

/**
 * Constructs a DT Response packet.
 * 
 * @param pkt A pointer to the packet.
 * @param n The size of the packet. Must be equal to RES_PKT_LEN.
 * @param reqType The type of request. Must be either REQ_DATE or REQ_TIME.
 * @param langCode The language to respond in. Must be valid.
 * @param year The year to return.
 * @param month The month to return.
 * @param day The day to return.
 * @param hour The hour to return.
 * @param minute The minute to return.
 * @return The length of the packet.
 * */
size_t dtRes(uint8_t pkt[], size_t n, uint16_t reqType, uint16_t langCode, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute)
{

    // check reqType, langCode and n
    if (!validReqType(reqType) || !validLangCode(langCode) || n != RES_PKT_LEN) {
        return 0;
    }

    // write most of the data to the packet
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

    // get the template phrase and the month as a string
    char* phrase = (char*)PHRASES[langCode - 1][reqType - 1];
    
    char* monthStr = (char*)MONTHS[langCode - 1][month - 1];

    char text[RES_TEXT_LEN] = {0};
    int length = 0;

    if (reqType == REQ_DATE) {
        // german has its values out of order, so handle it seperately
        if (langCode == LANG_GER) {
            length = sprintf(text, phrase, day, monthStr, year);
        } else {
            length = sprintf(text, phrase, monthStr, day, year);
        }
    } else {
        length = sprintf(text, phrase, hour, minute);
    }
    
    // an error occurred during sprintf
    if (length < 0) {
        return 0;
    }

    // we ignore the null terminator
    length--;

    // write the length to the packet
    pkt[12] = (uint8_t)length;

    // fill the rest of the packet with the text
    for (int i = 0; i < length; i++) {
        pkt[13 + i] = text[i];
    }

    return 13 + length;

}

/**
 * Returns true if the DT Response packet is valid.
 * 
 * @param pkt The packet.
 * @param n The size of the packet.
 * @return True if the packet is valid.
 * */
bool dtResValid(uint8_t pkt[], size_t n)
{
    if (n < 13) {
        return false;
    }

    if (dtPktMagicNo(pkt, n) != MAGIC_NO) {
        return false;
    }

    if (dtPktType(pkt, n) != PACKET_RES) {
        return false;
    }

    if (dtResYear(pkt, n) >= 2100) {
        return false;
    }

    if (dtResMonth(pkt, n) < 1 ||
        dtResMonth(pkt, n) > 12) {
        return false;
    }

    if (dtResDay(pkt, n) < 1 ||
        dtResDay(pkt, n) > 31) {
        return false;
    }

    if (dtResHour(pkt, n) < 0 ||
        dtResHour(pkt, n) > 23) {
        return false;
    }

    if (dtResMinute(pkt, n) < 0 ||
        dtResMinute(pkt, n) > 59) {
        return false;
    }

    if (dtResLength(pkt, n) + 13 != n) {
        return false;
    }

    return true;
}

/**
 * Returns the language code in the packet.
 * No checking is done beforehand.
 * 
 * @param pkt The packet.
 * @param n The length of the packet.
 * @return The language code.
 * */
uint16_t dtResLangCode(uint8_t pkt[], size_t n)
{
    return ((pkt[4] << 8) | pkt[5]);
}

/**
 * Returns the year defined in the packet.
 * No checking is done beforehand.
 * 
 * @param pkt The packet.
 * @param n The length of the packet.
 * @return The year.
 * */
uint16_t dtResYear(uint8_t pkt[], size_t n)
{
    return ((pkt[6] << 8) | pkt[7]);
}

/**
 * Returns the month defined in the packet.
 * No checking is done beforehand.
 * 
 * @param The packet.
 * @param The length of the packet.
 * @return The month.
 * */
uint8_t dtResMonth(uint8_t pkt[], size_t n)
{
    return (pkt[8]);
}

/**
 * Returns the day defined in the packet.
 * No checking is done beforehand.
 * 
 * @param pkt The packet.
 * @param n The length of the packet.
 * @return The day.
 * */
uint8_t dtResDay(uint8_t pkt[], size_t n)
{
    return (pkt[9]);
}

/**
 * Returns the hour defined in the packet.
 * No checking is done beforehand.
 * 
 * @param pkt The packet.
 * @param n The length of the packet.
 * @return The hour.
 * */
uint8_t dtResHour(uint8_t pkt[], size_t n)
{
    return (pkt[10]);
}

/**
 * Returns the minute defined in the packet.
 * No checking is done beforehand.
 * 
 * @param pkt The packet.
 * @param n The length of the packet.
 * @return The minute.
 * */
uint8_t dtResMinute(uint8_t pkt[], size_t n)
{
    return (pkt[11]);
}

/**
 * Returns the length of the text in the packet.
 * No checking is done beforehand.
 * 
 * @param pkt The packet.
 * @param n The length of the packet.
 * @return The length of the text.
 * */
uint8_t dtResLength(uint8_t pkt[], size_t n)
{
    return (pkt[12]);
}

/**
 * Copies the text from the packet and puts it into the char array text.
 * No checking is done beforehand.
 * 
 * @param pkt The packet.
 * @param n The length of the packet.
 * @param text The char array to store the text in.
 * @param textLen A pointer where the length of the text is to be stored.
 * */
void dtResText(uint8_t pkt[], size_t n, char text[], size_t* textLen)
{
    *textLen = dtResLength(pkt, n);
    for (int i = 0; i < *textLen; i++) {
        text[i] = pkt[13 + i];
    }
    text[*textLen] = 0;
}

