#include <stdlib.h>

#include "buffer_internal.h"
#include "tokenizer_internal.h"

/*
 * Contains data used by the tokenizer.
 */
struct TokenizerHandle {
    JsonBuffer * buffer;

    char * valueBuffer;
    int valueBufferSize;

    JsonError error;
};

/*
 * Get a string with the name of a given token.
 */
char * json_token_name(TokenType token) {
    switch(token) {
        case JSON_TOKEN_OBJECT_START:
            return "Object start";
        case JSON_TOKEN_OBJECT_END:
            return "Object end";
        case JSON_TOKEN_ARRAY_START:
            return "Array start";
        case JSON_TOKEN_ARRAY_END:
            return "Array end";
        case JSON_TOKEN_COMMA:
            return "Comma";
        case JSON_TOKEN_COLON:
            return "Colon";
        case JSON_TOKEN_TEXT:
            return "String";
        case JSON_TOKEN_NUMBER_DECIMAL:
            return "Decimal";
        case JSON_TOKEN_NUMBER_INTEGER:
            return "Integer";
        case JSON_TOKEN_TRUE:
            return "True";
        case JSON_TOKEN_FALSE:
            return "False";
        case JSON_TOKEN_NULL:
            return "Null";
        case JSON_TOKEN_EOF:
            return "End of file";
        default:
            return "Unknown token";
    }
}

/*
 * Create a tokenizer object that reads from the buffer.
 */
TokenizerHandle * json_tokenizer_create(JsonBuffer * buffer, JsonError * error) {
    TokenizerHandle * tokenizer = (TokenizerHandle *) malloc(sizeof(TokenizerHandle));

    if(tokenizer == NULL) {
        *error = JSON_ERROR_MALLOC;
        return NULL;
    }

    tokenizer->buffer = buffer;

    tokenizer->valueBuffer = malloc(32);
    tokenizer->valueBufferSize = 32;

    if(tokenizer->valueBuffer == NULL) {
        free(tokenizer);

        *error = JSON_ERROR_MALLOC;
        return NULL;
    }

    tokenizer->error = JSON_SUCCESS;

    *error = JSON_SUCCESS;

    return tokenizer;
}

/*
 * Open a buffered file to read from for the tokenizer.
 */
TokenizerHandle * json_tokenizer_openFile(char * file, int bufferSize, int history, JsonError * error) {
    JsonBuffer * buffer = json_bufferedFile_open(file, bufferSize, history, error);

    if(buffer == NULL) {
        return NULL;
    }

    return json_tokenizer_create(buffer, error);
}

/*
 * Destroy the buffer of the tokenizer and the tokenizer itself.
 */
JsonError json_tokenizer_destroy(TokenizerHandle * tokenizer) {
    JsonError error = json_buffer_destroy(tokenizer->buffer);

    free(tokenizer);

    return error;
}

/*
 * Get the value associated with a token.
 *
 * Currently only the following tokens have values:
 *   JSON_TOKEN_STRING: The string itself.
 *   JSON_TOKEN_NUMBER: The number in string form.
 */
char * json_tokenizer_getValue(TokenizerHandle * tokenizer) {
    return tokenizer->valueBuffer;
}

/*
 * If an error has occurred in the tokenizer this will return the error, otherwise will return JSON_SUCCESS.
 */
JsonError json_tokenizer_getError(TokenizerHandle * tokenizer) {
    return tokenizer->error;
}

/*
 * Gets the buffer that the tokenizer is reading from.
 */
JsonBuffer * json_tokenizer_getBuffer(TokenizerHandle * tokenizer) {
    return tokenizer->buffer;
}

/*
 * Logs the error and the context of the error in the tokenizer.
 */
void json_tokenizer_logError(TokenizerHandle * tokenizer) {
    if(tokenizer->buffer->index > 0) {
        tokenizer->buffer->index--;
    }

    json_error_log(tokenizer->error, tokenizer->buffer);
}

/*
 * Reads the next token from the tokenizer handle.
 *
 * If an error occurs, JSON_TOKEN_ERROR will be returned and the error can be retrieved using json_tokenizer_getError.
 */
TokenType json_tokenizer_readNextToken(TokenizerHandle * tokenizer) {
    JsonError error = json_tokenizer_skipWhitespace(tokenizer);

    if(error != JSON_SUCCESS) {
        if(error == JSON_ERROR_EOF) {
            return JSON_TOKEN_EOF;
        }

        tokenizer->error = error;
        return JSON_TOKEN_ERROR;
    }

    char c = tokenizer->buffer->buffer[tokenizer->buffer->index++];

    switch(c) {
        case '{':
            return JSON_TOKEN_OBJECT_START;
        case '}':
            return JSON_TOKEN_OBJECT_END;
        case '[':
            return JSON_TOKEN_ARRAY_START;
        case ']':
            return JSON_TOKEN_ARRAY_END;
        case ':':
            return JSON_TOKEN_COLON;
        case ',':
            return JSON_TOKEN_COMMA;
        case '"':
            error = json_tokenizer_readString(tokenizer);

            if(error != JSON_SUCCESS) {
                tokenizer->error = error;
                return JSON_TOKEN_ERROR;
            }

            return JSON_TOKEN_TEXT;
        case 't':
            error = json_tokenizer_readTrue(tokenizer);

            if(error != JSON_SUCCESS) {
                tokenizer->error = error;
                return JSON_TOKEN_ERROR;
            }

            return JSON_TOKEN_TRUE;
        case 'f':
            error = json_tokenizer_readFalse(tokenizer);

            if(error != JSON_SUCCESS) {
                tokenizer->error = error;
                return JSON_TOKEN_ERROR;
            }

            return JSON_TOKEN_FALSE;
        case 'n':
            error = json_tokenizer_readNull(tokenizer);

            if(error != JSON_SUCCESS) {
                tokenizer->error = error;
                return JSON_TOKEN_ERROR;
            }

            return JSON_TOKEN_NULL;
        default:
            if(c == '-' || (c >= '0' && c <= '9')) {
                // The first character matters when reading the number
                tokenizer->buffer->index--;

                return json_tokenizer_readNumber(tokenizer);
            }

            tokenizer->error = JSON_ERROR_UNEXPECTED_CHAR;
            return JSON_TOKEN_ERROR;
    }
}

/*
 * Increases the size of the value buffer.
 */
JsonError json_tokenizer_expandValueBuffer(TokenizerHandle * tokenizer) {
    tokenizer->valueBufferSize *= 2;
    tokenizer->valueBuffer = realloc(tokenizer->valueBuffer, (size_t) tokenizer->valueBufferSize);

    return (tokenizer->valueBuffer == NULL ? JSON_ERROR_REALLOC : JSON_SUCCESS);
}

/*
 * Ensures there is at least one character in the buffer, reading if necessary.
 */
JsonError json_tokenizer_ensureCharAvailableInBuffer(TokenizerHandle *tokenizer) {
    JsonBuffer * buffer = tokenizer->buffer;

    return (buffer->index < buffer->read ? JSON_SUCCESS : json_buffer_fill(buffer));
}

/*
 * Increments forward from the character at the buffer index until a non-whitespace character is found.
 */
JsonError json_tokenizer_skipWhitespace(TokenizerHandle * tokenizer) {
    JsonBuffer * buffer = tokenizer->buffer;

    while(true) {
        while(buffer->index < buffer->read) {
            if(!json_char_isWhitespace(buffer->buffer[buffer->index])) {
                return JSON_SUCCESS;
            }

            buffer->index++;
        }

        JsonError error = json_buffer_fill(buffer);

        if(error != JSON_SUCCESS)
            return error;
    }
}

/*
 * Reads the next number in the buffer.
 *
 * Assumes that the first character of the number has not already been read.
 */
TokenType json_tokenizer_readNumber(TokenizerHandle * tokenizer) {
    JsonBuffer * buffer = tokenizer->buffer;

    JsonError error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS) {
        tokenizer->error = error;
        return JSON_TOKEN_ERROR;
    }

    bool negative = false;

    if(buffer->buffer[buffer->index] == '-') {
        buffer->index++;

        negative = true;
    }

    bool seenDecimal = false;
    int exponent = 0;

    long number = 0;
    
    // TODO: Parse number, placing number in value buffer

    return JSON_TOKEN_ERROR;
}

/*
 * Reads the next string in the buffer.
 *
 * Assumes that the opening quotation mark (") has already been read and the buffer index is at the character after it.
 *
 * Will consume the closing quotation mark (").
 */
JsonError json_tokenizer_readString(TokenizerHandle * tokenizer) {
    JsonBuffer * buffer = tokenizer->buffer;

    int valueBufferIndex = 0;

    while(true) {
        while(buffer->index < buffer->read) {
            char current = buffer->buffer[buffer->index++];

            if(json_char_isControlCharacter(current)) {
                return JSON_ERROR_ILLEGAL_TEXT_CHAR;
            }

            if(valueBufferIndex >= tokenizer->valueBufferSize) {
                JsonError error = json_tokenizer_expandValueBuffer(tokenizer);

                if(error != JSON_SUCCESS)
                    return error;
            }

            switch(current) {
                case '\\':
                {
                    JsonError error = json_tokenizer_readEscaped(tokenizer, &valueBufferIndex);

                    if(error != JSON_SUCCESS)
                        return error;

                    break;
                }
                case '"':
                    tokenizer->valueBuffer[valueBufferIndex++] = '\0';
                    return JSON_SUCCESS;
                default:
                    tokenizer->valueBuffer[valueBufferIndex++] = current;
                    break;
            }
        }

        JsonError error = json_buffer_fill(buffer);

        if(error != JSON_SUCCESS)
            return error;
    }
}

/*
 * Reads an escaped character.
 *
 * Assumes the escape symbol (\) has already been read.
 */
JsonError json_tokenizer_readEscaped(TokenizerHandle * tokenizer, int * valueBufferIndex) {
    JsonError error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS)
        return error;

    while(*valueBufferIndex >= tokenizer->valueBufferSize) {
        error = json_tokenizer_expandValueBuffer(tokenizer);

        if(error != JSON_SUCCESS)
            return error;
    }

    JsonBuffer * buffer = tokenizer->buffer;

    char current = buffer->buffer[buffer->index++];

    switch(current) {
        case '"':
        case '\\':
        case '/':
            tokenizer->valueBuffer[(*valueBufferIndex)++] = current;
            return JSON_SUCCESS;
        case 'b':
            tokenizer->valueBuffer[(*valueBufferIndex)++] = '\b';
            return JSON_SUCCESS;
        case 'f':
            tokenizer->valueBuffer[(*valueBufferIndex)++] = '\f';
            return JSON_SUCCESS;
        case 'n':
            tokenizer->valueBuffer[(*valueBufferIndex)++] = '\n';
            return JSON_SUCCESS;
        case 'r':
            tokenizer->valueBuffer[(*valueBufferIndex)++] = '\r';
            return JSON_SUCCESS;
        case 't':
            tokenizer->valueBuffer[(*valueBufferIndex)++] = '\t';
            return JSON_SUCCESS;
        case 'u':
            return json_tokenizer_readCodePoint(tokenizer, valueBufferIndex);
        default:
            return JSON_ERROR_INVALID_ESCAPE;
    }
}

/*
 * Reads a UCS codepoint as UTF-8.
 *
 * Assumes the escape symbol (\) and the unicode escape character (u) have already been read.
 */
JsonError json_tokenizer_readCodePoint(TokenizerHandle * tokenizer, int * valueBufferIndex) {
    JsonBuffer * buffer = tokenizer->buffer;

    int codepoint = 0;

    for(int i=0; i < 4; i++) {
        JsonError error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

        if(error != JSON_SUCCESS) {
            return error;
        }

        codepoint = codepoint << 4;

        char current = buffer->buffer[buffer->index++];

        if(current >= '0' && current <= '9') {
            codepoint += current - '0';
        } else if(current >= 'a' && current <= 'f') {
            codepoint += current - 'a' + 10;
        } else if(current >= 'A' && current <= 'F') {
            codepoint += current - 'A' + 10;
        } else {
            return JSON_ERROR_INVALID_UNICODE_ESCAPED_CHAR;
        }
    }

    // Ensure there are at least 6 bytes available in the value buffer.
    while(tokenizer->valueBufferSize - 6 <= *valueBufferIndex) {
        JsonError error = json_tokenizer_expandValueBuffer(tokenizer);

        if(error != JSON_SUCCESS)
            return error;
    }

    int bytesWritten = json_char_UCSCodepointToUTF8(codepoint, &(tokenizer->valueBuffer[*valueBufferIndex]));

    if(bytesWritten == -1) {
        return JSON_ERROR_INVALID_UNICODE_ESCAPED_CHAR;
    }

    *valueBufferIndex += bytesWritten;

    return JSON_SUCCESS;
}

/*
 * Reads null, returning an error if null is not found.
 *
 * Assumes the first character (n) has already been read.
 */
JsonError json_tokenizer_readNull(TokenizerHandle * tokenizer) {
    JsonBuffer * buffer = tokenizer->buffer;

    JsonError error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS)
        return error;

    if(buffer->buffer[buffer->index++] != 'u')
        return JSON_ERROR_EXPECTED_NULL;

    error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS)
        return error;

    if(buffer->buffer[buffer->index++] != 'l')
        return JSON_ERROR_EXPECTED_NULL;

    error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS)
        return error;

    if(buffer->buffer[buffer->index++] != 'l')
        return JSON_ERROR_EXPECTED_NULL;

    return JSON_SUCCESS;
}

/*
 * Reads true, returning an error if true is not found.
 *
 * Assumes the first character (t) has already been read.
 */
JsonError json_tokenizer_readTrue(TokenizerHandle * tokenizer) {
    JsonBuffer * buffer = tokenizer->buffer;

    JsonError error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS)
        return error;

    if(buffer->buffer[buffer->index++] != 'r')
        return JSON_ERROR_EXPECTED_TRUE;

    error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS)
        return error;

    if(buffer->buffer[buffer->index++] != 'u')
        return JSON_ERROR_EXPECTED_TRUE;

    error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS)
        return error;

    if(buffer->buffer[buffer->index++] != 'e')
        return JSON_ERROR_EXPECTED_TRUE;

    return JSON_SUCCESS;
}

/*
 * Reads false, returning an error if false is not found.
 *
 * Assumes the first character (f) has already been read.
 */
JsonError json_tokenizer_readFalse(TokenizerHandle * tokenizer) {
    JsonBuffer * buffer = tokenizer->buffer;

    JsonError error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS)
        return error;

    if(buffer->buffer[buffer->index++] != 'a')
        return JSON_ERROR_EXPECTED_FALSE;

    error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS)
        return error;

    if(buffer->buffer[buffer->index++] != 'l')
        return JSON_ERROR_EXPECTED_FALSE;

    error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS)
        return error;

    if(buffer->buffer[buffer->index++] != 's')
        return JSON_ERROR_EXPECTED_FALSE;

    error = json_tokenizer_ensureCharAvailableInBuffer(tokenizer);

    if(error != JSON_SUCCESS)
        return error;

    if(buffer->buffer[buffer->index++] != 'e')
        return JSON_ERROR_EXPECTED_FALSE;

    return JSON_SUCCESS;
}