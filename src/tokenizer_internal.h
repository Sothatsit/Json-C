#ifndef JSON
#define JSON
#include "json.h"
#endif

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