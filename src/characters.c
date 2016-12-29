#include "json.h"

/*
 * Returns whether the character is considered by JSON to be whitespace.
 *
 * A character is considered whitespace if it is one of the following:
 * - Horizontal Tab '\t'
 * - New Line '\n'
 * - Carriage Return '\r'
 * - Space ' '
 */
bool json_char_isWhitespace(char c) {
    switch(c) {
        case '\t':
        case '\n':
        case '\r':
        case ' ':
            return true;
        default:
            return false;
    }
}

/*
 * Returns whether the character is considered by JSON to be a control character.
 */
bool json_char_isControlCharacter(char c) {
    return c >= 0 && c <= 31;
}

/*
 * Returns whether the character is able to be placed by itself within text.
 */
bool json_char_isValidTextChar(char c) {
    return !json_char_isControlCharacter(c) && c != '\\';
}

/*
 * Places the UCS codepoint as UTF-8 in the buffer.
 */
int json_char_UCSCodepointToUTF8(int codepoint, char * buffer) {
    if(codepoint < 0) {
        // Invalid codepoint.
        return -1;
    }

    // If the codepoint can fit into 7 bits.
    if(codepoint <= 0x007F) {
        // 0xxxxxxx

        buffer[0] = (char) codepoint;

        return 1;
    }

    // If the codepoint can fit into 11 bits.
    if(codepoint <= 0x07FF) {
        // 110xxxxx 10xxxxxx

        buffer[0] = (char) (0xC0 | (codepoint >> 6));
        buffer[1] = (char) (0x80 | (codepoint & 0x3F));

        return 2;
    }

    // If the codepoint can fit into 16 bits.
    if(codepoint <= 0xFFFF) {
        // 1110xxxx 10xxxxxx 10xxxxxx

        buffer[0] = (char) (0xE0 | (codepoint >> 12));
        buffer[1] = (char) (0x80 | ((codepoint >> 6) & 0x3F));
        buffer[2] = (char) (0x80 | (codepoint & 0x3F));

        return 3;
    }

    // If the codepoint can fit into 21 bits.
    if(codepoint <= 0x001FFFFF) {
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

        buffer[0] = (char) (0xF0 | (codepoint >> 18));
        buffer[1] = (char) (0x80 | ((codepoint >> 12) & 0x3F));
        buffer[2] = (char) (0x80 | ((codepoint >> 6) & 0x3F));
        buffer[3] = (char) (0x80 | (codepoint & 0x3F));

        return 4;
    }

    // If the codepoint can fit into 26 bits.
    if(codepoint <= 0x03FFFFFF) {
        // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

        buffer[0] = (char) (0xF8 | (codepoint >> 24));
        buffer[1] = (char) (0x80 | ((codepoint >> 18) & 0x3F));
        buffer[2] = (char) (0x80 | ((codepoint >> 12) & 0x3F));
        buffer[3] = (char) (0x80 | ((codepoint >> 6) & 0x3F));
        buffer[4] = (char) (0x80 | (codepoint & 0x3F));

        return 5;
    }

    // If the codepoint can fit into 31 bits.
    if(codepoint <= 0x7FFFFFFF) {
        // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

        buffer[0] = (char) (0xFC | (codepoint >> 30));
        buffer[1] = (char) (0x80 | ((codepoint >> 24) & 0x3F));
        buffer[2] = (char) (0x80 | ((codepoint >> 18) & 0x3F));
        buffer[3] = (char) (0x80 | ((codepoint >> 12) & 0x3F));
        buffer[4] = (char) (0x80 | ((codepoint >> 6) & 0x3F));
        buffer[5] = (char) (0x80 | (codepoint & 0x3F));

        return 6;
    }

    // Invalid codepoint.
    return -1;
}