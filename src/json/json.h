#ifndef JSON_JSON_H
#define JSON_JSON_H

#include <stdbool.h>

typedef enum {
	JSON_SUCCESS,
	JSON_ERROR_UNTERMINATED_STRING,
	JSON_ERROR_EXPECTED_STRING,
	JSON_ERROR_INVALID_STRING_ESCAPE,
	JSON_ERROR_EXPECTED_VALUE,
	JSON_ERROR_UNTERMINATED_STRUCTURE,
	JSON_ERROR_EXPECTED_COLON,
} JsonError;

typedef enum {
	JSON_TYPE_NULL,
	JSON_TYPE_BOOL,
	JSON_TYPE_NUMBER,
	JSON_TYPE_STRING,
	JSON_TYPE_OBJECT,
	JSON_TYPE_ARRAY,
} JsonType;

struct JsonValue_;
typedef struct JsonValue_ JsonValue;

struct JsonItem_;
typedef struct JsonItem_ JsonItem;

typedef union {
	bool boolean;
	double number;
	char* string;
	JsonItem** object;
	JsonValue** array;
} JsonData;

struct JsonValue_ {
	JsonType type;
	JsonData data;
};

struct JsonItem_ {
	char* name;
	JsonValue* value;
};

JsonValue* json_parse(const char* string);
char* json_serialize(JsonValue* value);
void json_free(JsonValue* value);

JsonError json_last_error();
const char* json_strerror(int code);

#endif // ifndef JSON_JSON_H
