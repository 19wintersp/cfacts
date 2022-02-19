#ifndef API_H
#define API_H

#include "program.h"

#define API_TODAY_PATH  "%s/fact"
#define API_AGO_PATH    "%s/fact/day/%d"
#define API_INDEX_PATH  "%s/fact/%d"
#define API_RANDOM_PATH "%s/fact/random"

#define USER_AGENT PROGRAM_NAME "/" PACKAGE_VERSION

char* api_fetch(char* url);

char* api_today(char* base);
char* api_ago(char* base, int days);
char* api_index(char* base, int index);
char* api_random(char* base);

#endif // ifndef API_H
