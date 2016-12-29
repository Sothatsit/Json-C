#include <stdio.h>

#ifndef JSON
#define JSON
#include "json.h"
#endif

void json_error_printContext_internal(FILE * outStream, JsonBuffer * buffer, char * prefix);

void json_error_printReason_internal(FILE * outStream, JsonError error, char * prefix);