#ifndef JSON
#define JSON
#include "json.h"
#endif

/*
 * The larger size of double and long.
 */
#define JSON_NUMBER_MAX_BYTES (sizeof(double) > sizeof(long) ? sizeof(double) : sizeof(long))

/*
 * The minimum number of char's required to hold JSON_NUMBER_MAX_BYTES.
 */
#define JSON_NUMBER_MAX_CHARS ((JSON_NUMBER_MAX_BYTES + sizeof(char) - 1) / sizeof(char))

JsonError json_tokenizer_expandValueBuffer(TokenizerHandle * tokenizer);

JsonError json_tokenizer_ensureCharAvailableInBuffer(TokenizerHandle *tokenizer);

JsonError json_tokenizer_skipWhitespace(TokenizerHandle * tokenizer);

TokenType json_tokenizer_readNumber(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readString(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readEscaped(TokenizerHandle * tokenizer, int * valueBufferIndex);

JsonError json_tokenizer_readCodePoint(TokenizerHandle * tokenizer, int * valueBufferIndex);

JsonError json_tokenizer_readNull(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readTrue(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readFalse(TokenizerHandle * tokenizer);