#ifndef JSON_UTIL_H
#define JSON_UTIL_H

#include <stdbool.h>
#include <stdlib.h>

#include "json.h"

JsonValue* json_null_new();
JsonValue* json_bool_new(bool value);
JsonValue* json_number_new(double value);
JsonValue* json_string_new(const char* value);

JsonValue* json_object_new();
JsonValue* json_object_set(JsonValue* this, const char* name, JsonValue* value);
JsonValue* json_object_get(const JsonValue* this, const char* name);

JsonValue* json_array_new();
size_t json_array_length(const JsonValue* this);
size_t json_array_push(JsonValue* this, JsonValue* value);
JsonValue* json_array_set(JsonValue* this, size_t index, JsonValue* value);
JsonValue* json_array_get(const JsonValue* this, size_t index);

#endif // ifndef JSON_UTIL_H
