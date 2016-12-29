#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "errors_internal.h"

/*
 * Used as a prefix for all log messages
 */
#define JSON_LOG_PREFIX "[Json] "

/*
 * Get a string with a short description of the given error
 */
char * json_error_name(JsonError error) {
    switch(error) {
        case JSON_SUCCESS:
            return "Success";
        case JSON_ERROR_OPEN_FILE:
            return "Error opening file";
        case JSON_ERROR_CLOSE_FILE:
            return "Error closing file";
        case JSON_ERROR_READ_FILE:
            return "Error reading from a file";
        case JSON_ERROR_EOF:
            return "End of file";
        case JSON_ERROR_MALLOC:
            return "Unable to allocate memory block";
        case JSON_ERROR_REALLOC:
            return "Unable to re-allocate memory block";
        case JSON_ERROR_UNEXPECTED_CHAR:
            return "Unexpected character";
        case JSON_ERROR_ILLEGAL_TEXT_CHAR:
            return "Illegal character in text";
        case JSON_ERROR_INVALID_ESCAPE:
            return "Invalid escape sequence";
        case JSON_ERROR_INVALID_UNICODE_ESCAPED_CHAR:
            return "Invalid hex character in escaped unicode character";
        case JSON_ERROR_DUPLICATE_DECIMAL_POINT:
            return "Duplicate decimal point";
        case JSON_ERROR_DUPLICATE_EXPONENT:
            return "Duplicate exponent";
        case JSON_ERROR_EXPECTED_TRUE:
            return "Expected true";
        case JSON_ERROR_EXPECTED_FALSE:
            return "Expected false";
        case JSON_ERROR_EXPECTED_NULL:
            return "Expected null";
        default:
            return "Unknown error code";
    }
}

/*
 * Logs an error recieved from json to stderr as well as the characters around the current buffer index.
 */
void json_error_log(JsonError error, JsonBuffer * buffer) {
    json_error_printReason(stderr, error);

    fprintf(stderr, JSON_LOG_PREFIX "\n");
    fprintf(stderr, JSON_LOG_PREFIX "Error occured around:\n");

    json_error_printContext_internal(stderr, buffer, "   ");
}

/*
 * Logs the reason for the error to stderr.
 */
void json_error_logReason(JsonError error) {
    json_error_printReason(stderr, error);
}

/*
 * Logs an error recieved from json to stream as well as the characters around the current buffer index.
 */
void json_error_print(FILE * stream, JsonError error, JsonBuffer * buffer) {
    json_error_printReason(stderr, error);

    fprintf(stderr, JSON_LOG_PREFIX "\n");
    fprintf(stderr, JSON_LOG_PREFIX "Error occured around:\n");

    json_error_printContext_internal(stderr, buffer, "   ");
}

/*
 * Prints a short description of the error passed to the stream.
 */
void json_error_printReason(FILE * stream, JsonError error) {
    fprintf(stream, JSON_LOG_PREFIX "Json has ran into an error:\n");
    json_error_printReason_internal(stream, error, "   ");
}

/*
 * Prints the characters around the current buffer index to the stream.
 */
void json_error_printContext(FILE * stream, JsonBuffer * buffer) {
    json_error_printContext_internal(stream, buffer, "");
}

/*
 * Prints the characters around the current buffer index to the stream with a prefix put on each line.
 */
void json_error_printContext_internal(FILE * stream, JsonBuffer * buffer, char * prefix) {
    int currentCharIndex;
    JsonError error;

    char * charactersAround = json_buffer_getCharactersAroundCurrent(buffer, &currentCharIndex, &error);

    if(charactersAround == NULL) {
        fprintf(stream, JSON_LOG_PREFIX "%sError getting characters around buffer index.\n", prefix);

        json_error_printReason_internal(stream, error, prefix);
        return;
    }

    fprintf(stream, JSON_LOG_PREFIX "%s%s\n", prefix, charactersAround);

    char * currentCharacterIndicator = (char *) malloc((size_t) (currentCharIndex + 2));

    if(currentCharacterIndicator == NULL) {
        fprintf(stream, JSON_LOG_PREFIX "%sError allocating string for current character indicator.\n", prefix);

        free(charactersAround);
        return;
    }

    memset(currentCharacterIndicator, ' ', currentCharIndex);

    currentCharacterIndicator[currentCharIndex] = '^';
    currentCharacterIndicator[currentCharIndex + 1] = '\0';

    fprintf(stream, JSON_LOG_PREFIX "%s%s\n", prefix, currentCharacterIndicator);

    free(charactersAround);
    free(currentCharacterIndicator);
}

/*
 * Prints a short description of the error passed to the stream with a prefix put on each line.
 */
void json_error_printReason_internal(FILE * stream, JsonError error, char * prefix) {
    char * jsonErrorName = json_error_name(error);

    fprintf(stream, JSON_LOG_PREFIX "%s%s (Error Code %i)\n", prefix, jsonErrorName, error);

    if(errno != 0) {
        char * errorString = strerror(errno);

        if(errorString == NULL) {
            errorString = "Unknown error or error in getting the error string from strerror.";
        }

        fprintf(stream, JSON_LOG_PREFIX "%sC error in error number: %s\n", prefix, errorString);
    }
}