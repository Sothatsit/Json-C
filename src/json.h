#include <stdio.h>
#include <stdbool.h>

//
// Json Errors
//

typedef enum JsonError JsonError;

enum JsonError {
    JSON_SUCCESS,

    JSON_ERROR_OPEN_FILE,
    JSON_ERROR_CLOSE_FILE,
    JSON_ERROR_READ_FILE,
    JSON_ERROR_EOF,
    JSON_ERROR_MALLOC,
    JSON_ERROR_REALLOC,
    JSON_ERROR_UNEXPECTED_CHAR,
    JSON_ERROR_EXPECTED_DIGIT,
    JSON_ERROR_EXPECTED_DIGIT_OR_SIGN,
    JSON_ERROR_ILLEGAL_TEXT_CHAR,
    JSON_ERROR_INVALID_ESCAPE,
    JSON_ERROR_INVALID_UNICODE_ESCAPED_CHAR,
    JSON_ERROR_EXPECTED_TRUE,
    JSON_ERROR_EXPECTED_FALSE,
    JSON_ERROR_EXPECTED_NULL
};

char * json_error_name(JsonError error);

//
// Json Buffers
//

typedef struct JsonBuffer JsonBuffer;

JsonBuffer * json_bufferFixed_create(char * buffer, int bufferSize, int history, JsonError * error);

JsonBuffer * json_bufferedFile_open(char * file, int bufferSize, int history, JsonError * error);

JsonError json_buffer_destroy(JsonBuffer * buffer);

JsonError json_buffer_fill(JsonBuffer * buffer);

char json_buffer_getLastCharacter(JsonBuffer * buffer);

char * json_buffer_getCharactersAroundCurrent(JsonBuffer * buffer, int * currentCharIndex, JsonError * error);

//
// Json Error Logging
//

void json_error_log(JsonError error, JsonBuffer * buffer);

void json_error_logReason(JsonError error);

void json_error_print(FILE * stream, JsonError error, JsonBuffer * buffer);

void json_error_printReason(FILE * stream, JsonError error);

void json_error_printContext(FILE * stream, JsonBuffer * buffer);

//
// Characters
//

bool json_char_isWhitespace(char c);

bool json_char_isControlCharacter(char c);

bool json_char_isValidTextChar(char c);

int json_char_UCSCodepointToUTF8(int codepoint, char * buffer);

//
// Json Tokenizer
//

typedef struct TokenizerHandle TokenizerHandle;

typedef enum TokenType TokenType;

enum TokenType {
    JSON_TOKEN_ERROR,

    JSON_TOKEN_OBJECT_START,
    JSON_TOKEN_OBJECT_END,
    JSON_TOKEN_ARRAY_START,
    JSON_TOKEN_ARRAY_END,
    JSON_TOKEN_COMMA,
    JSON_TOKEN_COLON,
    JSON_TOKEN_TEXT,
    JSON_TOKEN_NUMBER_DECIMAL,
    JSON_TOKEN_NUMBER_INTEGER,
    JSON_TOKEN_NUMBER_BIG,
    JSON_TOKEN_TRUE,
    JSON_TOKEN_FALSE,
    JSON_TOKEN_NULL,
    JSON_TOKEN_EOF
};

char * json_token_name(TokenType token);

TokenizerHandle * json_tokenizer_openFile(char * file, int bufferSize, int history, JsonError * error);

TokenizerHandle * json_tokenizer_create(JsonBuffer * buffer, JsonError * error);

JsonError json_tokenizer_destroy(TokenizerHandle * tokenizer);

TokenType json_tokenizer_readNextToken(TokenizerHandle * tokenizer);

char * json_tokenizer_getStringValue(TokenizerHandle * tokenizer);

char * json_tokenizer_getNumberValue(TokenizerHandle * tokenizer);

double json_tokenizer_getDecimalValue(TokenizerHandle * tokenizer);

long json_tokenizer_getIntegerValue(TokenizerHandle * tokenizer);

JsonError json_tokenizer_getError(TokenizerHandle * tokenizer);

JsonBuffer * json_tokenizer_getBuffer(TokenizerHandle * tokenizer);

void json_tokenizer_logError(TokenizerHandle * tokenizer);