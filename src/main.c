#include <stdio.h>
#include <stdlib.h>

#include "json.h"

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Expected a single input file\n");
        return EXIT_FAILURE;
    }

    JsonError error;
    TokenizerHandle * tokenizer = json_tokenizer_openFile(argv[1], 1024, 10, &error);

    if(error != JSON_SUCCESS) {
        fprintf(stderr, "There was an error opening the tokenizer from file %s.\n", argv[1]);
        json_error_printReason(stderr, error);
        return EXIT_FAILURE;
    }

    for(int i = 0; i < 50; i++) {
        TokenType token = json_tokenizer_readNextToken(tokenizer);

        if(token == JSON_TOKEN_ERROR) {
            json_tokenizer_logError(tokenizer);
            return EXIT_FAILURE;
        }

        if(token == JSON_TOKEN_EOF) {
            printf("EOF\n");
            break;
        }

        char currentChar = json_buffer_getLastCharacter(json_tokenizer_getBuffer(tokenizer));

        printf("'%c' %s ", currentChar, json_token_name(token));

        switch(token) {
            case JSON_TOKEN_TEXT:
                printf("\"%s\"\n", json_tokenizer_getStringValue(tokenizer));
                break;
            case JSON_TOKEN_NUMBER_DECIMAL:
            case JSON_TOKEN_NUMBER_INTEGER:
            case JSON_TOKEN_NUMBER_BIG:
                printf("%s\n", json_tokenizer_getNumberValue(tokenizer));
                break;
            default:
                printf("\n");
                break;
        }
    }

    error = json_tokenizer_destroy(tokenizer);

    if(error != JSON_SUCCESS) {
        fprintf(stderr, "There was an error destroyng the tokenizer.\n");
        json_error_logReason(error);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}