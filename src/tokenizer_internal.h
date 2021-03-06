#ifndef JSON
#define JSON
#include "json.h"
#endif

JsonError json_tokenizer_expandValueBuffer(TokenizerHandle * tokenizer);

JsonError json_tokenizer_appendToValueBuffer(TokenizerHandle * tokenizer, char character);

JsonError json_tokenizer_skipWhitespace(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readNumber(TokenizerHandle * tokenizer, TokenType * token);

JsonError json_tokenizer_readIntegerPart(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readString(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readEscaped(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readCodePoint(TokenizerHandle * tokenizer);

JsonError json_tokenizer_readExpected(TokenizerHandle * tokenizer, char * expected);