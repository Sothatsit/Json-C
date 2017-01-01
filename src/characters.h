#include <stdbool.h>

/*
 * Returns whether the character is considered by JSON to be whitespace.
 *
 * A character is considered whitespace if it is one of the following:
 * - Horizontal Tab '\t'
 * - New Line '\n'
 * - Carriage Return '\r'
 * - Space ' '
 */
#define json_char_isWhitespace(c) (c == '\t' || c == '\n' || c == '\r' || c == ' ')

/*
 * Returns whether the character is considered by JSON to be a control character.
 */
#define json_char_isControlCharacter(c) (c >= 0 && c <= 31)

/*
 * Returns whether the character is considered by JSON to be a digit.
 */
#define json_char_isDigit(c) (c >= '0' && c <= '9')

/*
 * Places the UCS codepoint as UTF-8 in the buffer.
 */
int json_char_UCSCodepointToUTF8(int codepoint, char * buffer);