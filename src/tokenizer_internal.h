#ifndef JSON
#define JSON
#include "json.h"
#endif

JsonError json_tokenizer_expandValueBuffer(TokenizerHandle * tokenizer);

JsonError json_tokenizer_appendToValueBuffer(TokenizerHandle * tokenizer, int * valueBufferIndex, char character);

JsonError json_tokenizer_skipWhitespace(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readNumber(TokenizerHandle * tokenizer, TokenType * token);

JsonError json_tokenizer_readIntegerPart(TokenizerHandle * tokenizer, int * valueBufferIndex);

JsonError json_tokenizer_readString(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readEscaped(TokenizerHandle * tokenizer, int * valueBufferIndex);

JsonError json_tokenizer_readCodePoint(TokenizerHandle * tokenizer, int * valueBufferIndex);

JsonError json_tokenizer_readNull(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readTrue(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readFalse(TokenizerHandle * tokenizer);