#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>

#include "buffer_internal.h"

/*
 * Create a fixed buffer with the contents in buffer.
 *
 * Reads history characters left and right of the buffer index for error messages.
 */
JsonBuffer * json_bufferFixed_create(char * buffer, int bufferSize, int history, JsonError * error) {
    JsonBuffer * fixedBuffer = (JsonBuffer *) malloc(sizeof(JsonBuffer));

    if(fixedBuffer == NULL) {
        *error = JSON_ERROR_MALLOC;
        return NULL;
    }

    fixedBuffer->bufferType = JSON_BUFFER_FIXED;

    fixedBuffer->buffer = buffer;
    fixedBuffer->bufferSize = bufferSize;

    fixedBuffer->index = 0;
    fixedBuffer->read = bufferSize;

    fixedBuffer->history = history;

    *error = JSON_SUCCESS;

    return fixedBuffer;
}

/*
 * Allocates a buffer with the contents read from the file passed.
 *
 * Maintains history characters behind the buffer index for error messages.
 *
 * The history must be at least 1, otherwise memory errors can occur.
 */
JsonBuffer * json_bufferedFile_open(char * file, int bufferSize, int history, JsonError * error) {
    BufferedFile * buffer = (BufferedFile *) malloc(sizeof(BufferedFile) + bufferSize);

    if(buffer == NULL) {
        *error = JSON_ERROR_MALLOC;
        return NULL;
    }

    buffer->buffer.bufferType = JSON_BUFFER_FILE;

    buffer->buffer.buffer = (char *) &buffer[1];
    buffer->buffer.bufferSize = bufferSize;

    buffer->buffer.index = 0;
    buffer->buffer.read = 0;

    buffer->buffer.history = history;

    buffer->file = open(file, O_RDONLY);

    if(buffer->file == -1) {
        free(buffer);

        *error = JSON_ERROR_OPEN_FILE;
        return NULL;
    }

    *error = JSON_SUCCESS;

    return (JsonBuffer *) buffer;
}

/*
 * Frees the resources created for the buffer. Does not free the buffer passed when creating a fixed buffer.
 */
JsonError json_buffer_destroy(JsonBuffer * buffer) {
    int file = -1;

    if(buffer->bufferType == JSON_BUFFER_FILE) {
        file = ((BufferedFile *) buffer)->file;
    }

    free(buffer);

    if(file != -1 && close(file) == -1) {
        return JSON_ERROR_CLOSE_FILE;
    }

    return JSON_SUCCESS;
}

/*
 * Attempts to fill the buffer with more data.
 */
JsonError json_buffer_fill(JsonBuffer * buffer) {
    if(buffer->bufferType != JSON_BUFFER_FILE) {
        return JSON_ERROR_EOF;
    }

    BufferedFile * stream = (BufferedFile *) buffer;

    int readFrom = buffer->read;

    if(buffer->read > buffer->history) {
        readFrom = buffer->history;

        int copyFrom = buffer->read - readFrom;

        if(buffer->read >= buffer->history * 2) {
            memcpy(buffer->buffer, &buffer->buffer[copyFrom], buffer->history);
        } else {
            memmove(buffer->buffer, &buffer->buffer[copyFrom], buffer->history);
        }
    }

    int charsRead = (int) read(stream->file, &buffer->buffer[readFrom], (size_t) (buffer->bufferSize - readFrom));

    if(charsRead == -1) {
        return JSON_ERROR_READ_FILE;
    } else if(charsRead == 0) {
        return JSON_ERROR_EOF;
    }

    buffer->index -= buffer->read - readFrom;
    buffer->read = readFrom + charsRead;

    return JSON_SUCCESS;
}

/*
 * Gets the character before the current buffer index, or the
 * first character in the buffer if the buffer index is at 0.
 */
char json_buffer_getLastCharacter(JsonBuffer * buffer) {
    if(buffer->index > 0) {
        return buffer->buffer[buffer->index - 1];
    } else {
        return buffer->buffer[buffer->index];
    }
}

/*
 * Gets a string for the characters around the character at the buffer index.
 */
char * json_buffer_getCharactersAroundCurrent(JsonBuffer * buffer, int * currentCharIndex, JsonError * error) {
    const int charsToLeft = (buffer->index > buffer->history ? buffer->history : buffer->index);
    const int size = charsToLeft + 1 + buffer->history;

    char * around = (char *) malloc((size_t) (size + 1));

    if(around == NULL) {
        *error = JSON_ERROR_MALLOC;
        return NULL;
    }

    for(int index = 0; index < size; index++) {
        int bufferIndex = buffer->index - charsToLeft + index;

        if(bufferIndex >= buffer->read) {
            JsonError fillError = json_buffer_fill(buffer);

            if(fillError == JSON_ERROR_EOF) {
                around[index] = '\0';

                break;
            } else if(fillError != JSON_SUCCESS) {
                *error = fillError;

                free(around);

                return NULL;
            }

            bufferIndex = buffer->index - charsToLeft + index;
        }

        char character = buffer->buffer[bufferIndex];

        if(json_char_isWhitespace(character)) {
            character = ' ';
        }

        around[index] = character;
    }

    around[size] = '\0';

    *currentCharIndex = charsToLeft;
    *error = JSON_SUCCESS;

    return around;
}