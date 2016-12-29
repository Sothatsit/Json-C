#ifndef JSON
#define JSON
#include "json.h"
#endif

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