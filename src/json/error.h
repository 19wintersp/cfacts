#ifndef JSON_ERROR_H
#define JSON_ERROR_H

#include "json.h"

#define RETURN_ERR(err) { json_set_error(err); return NULL; }

void json_set_error(JsonError err);
JsonError json_last_error();

#endif // ifndef JSON_ERROR_H
