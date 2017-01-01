#ifndef JSON
#define JSON
#include "json.h"
#endif

/*
 * Get the character at the buffer index.
 */
#define json_buffer_get(jsonBuffer) (jsonBuffer->buffer[jsonBuffer->index])

/*
 * Get and consume the character at the buffer index.
 */
#define json_buffer_get_consume(jsonBuffer) (jsonBuffer->buffer[jsonBuffer->index++])

/*
 * Consume the character at the buffer index.
 */
#define json_buffer_consume(jsonBuffer) (jsonBuffer->index++)

/*
 * Move the buffer index back to undo consuming a character.
 */
#define json_buffer_unconsume(jsonBuffer) (jsonBuffer->index--)

/*
 * Ensure there is at least one character in the buffer, reading if necessary.
 *
 * Resolves to a JsonError.
 */
#define json_buffer_ensureAvailable(jsonBuffer) (jsonBuffer->index < jsonBuffer->read ? JSON_SUCCESS : json_buffer_fill(jsonBuffer))

typedef enum BufferType BufferType;

/*
 * The type of the buffer.
 *
 * JSON_BUFFER_FIXED: The user populates the buffer.
 * JSON_BUFFER_FILE: The buffer is filled from a file.
 */
enum BufferType {
    JSON_BUFFER_FIXED,
    JSON_BUFFER_FILE
};

/*
 * The base buffer struct.
 */
struct JsonBuffer {
    BufferType bufferType;

    char *buffer;
    int bufferSize;

    int index;
    int read;

    int history;
};

typedef struct BufferedFile BufferedFile;

/*
 * The buffer struct for buffered streams.
 */
struct BufferedFile {
    JsonBuffer buffer;

    int file;
};