#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "json.h"
#include "strbuf.h"
#include "string.h"

static const char* __ERROR_NAMES[] = {
	"success",
	"unterminated string",
	"expected string",
	"invalid string escape",
	"expected value",
	"unterminated structure",
	"expected colon",
};

const char* json_strerror(int code) {
	if (code > 6) return "unknown error";
	else return __ERROR_NAMES[code];
}

static JsonValue* __parse(const char* string, size_t* length) {
	*length = strspn(string, "\t\n\r ");

	JsonValue* value = malloc(sizeof(JsonValue));

	switch (string[*length]) {
		case 0:
			RETURN_ERR(JSON_ERROR_EXPECTED_VALUE);
		case '"': {}
			size_t string_length;
			char* string_data = json_string_parse(string + *length, &string_length);

			if (string_data == NULL) return NULL;

			*length += string_length;

			value->type = JSON_TYPE_STRING;
			value->data = (JsonData) { .string = string_data };
			break;
		case '{': {}
			size_t item_count = 0;
			JsonItem** items = malloc(sizeof(JsonItem*) * 8);

			*length += 1;
			*length += strspn(string + *length, "\t\n\r ");

			for (; string[*length] != '}'; item_count++) {
				*length += strspn(string + *length, "\t\n\r ");

				size_t name_length;
				char* name = json_string_parse(string + *length, &name_length);

				if (name == NULL) return NULL;

				*length += name_length;
				*length += strspn(string + *length, "\t\n\r ");

				if (string[*length] != ':')
					RETURN_ERR(JSON_ERROR_EXPECTED_COLON);
				*length += 1;

				size_t value_length;
				JsonValue* ovalue = __parse(string + *length, &value_length);

				*length += value_length;
				*length += strspn(string + *length, "\t\n\r ");

				JsonItem* item = malloc(sizeof(JsonItem));
				item->name = name;
				item->value = ovalue;

				items[item_count] = item;

				if (item_count % 8 == 7)
					items = realloc(items, sizeof(JsonItem*) * (item_count + 9));

				if (string[*length] == ',') *length += 1;
				else { item_count++; break; }
			}

			if (string[*length] != '}')
				RETURN_ERR(JSON_ERROR_UNTERMINATED_STRUCTURE);
			*length += 1;

			items[item_count] = NULL;

			value->type = JSON_TYPE_OBJECT;
			value->data = (JsonData) { .object = items };
			break;
		case '[': {}
			size_t value_count = 0;
			JsonValue** values = malloc(sizeof(JsonValue*) * 8);

			*length += 1;
			*length += strspn(string + *length, "\t\n\r ");

			for (; string[*length] != ']'; value_count++) {
				*length += strspn(string + *length, "\t\n\r ");

				size_t value_length;
				JsonValue* avalue = __parse(string + *length, &value_length);

				*length += value_length;
				*length += strspn(string + *length, "\t\n\r ");

				values[value_count] = avalue;

				if (value_count % 8 == 7)
					values = realloc(values, sizeof(JsonValue*) * (value_count + 9));
				
				if (string[*length] == ',') *length += 1;
				else { value_count++; break; }
			}

			if (string[*length] != ']')
				RETURN_ERR(JSON_ERROR_UNTERMINATED_STRUCTURE);
			*length += 1;

			values[value_count] = NULL;

			value->type = JSON_TYPE_ARRAY;
			value->data = (JsonData) { .array = values };
			break;
		case 'n':
			if (strncmp(string + *length, "null", 4) == 0) {
				*length += 4;

				value->type = JSON_TYPE_NULL;
				break;
			} else RETURN_ERR(JSON_ERROR_EXPECTED_VALUE);
		case 't':
			if (strncmp(string + *length, "true", 4) == 0) {
				*length += 4;

				value->type = JSON_TYPE_BOOL;
				value->data = (JsonData) { .boolean = 1 };
				break;
			} else RETURN_ERR(JSON_ERROR_EXPECTED_VALUE);
		case 'f':
			if (strncmp(string + *length, "false", 5) == 0) {
				*length += 5;

				value->type = JSON_TYPE_BOOL;
				value->data = (JsonData) { .boolean = 0 };
				break;
			} else RETURN_ERR(JSON_ERROR_EXPECTED_VALUE);
		default:
			if (
				string[*length] == '-' ||
				(string[*length] >= '0' && string[*length] <= '9')
			) {
				size_t num_length = strcspn(string + *length, "\t\n\r ,}]");

				char* num_str = malloc(num_length + 1);
				num_str[num_length] = 0;
				strncpy(num_str, string + *length, num_length);

				*length += num_length;

				value->type = JSON_TYPE_NUMBER;
				value->data = (JsonData) { .number = atof(num_str) };

				free(num_str);
			} else RETURN_ERR(JSON_ERROR_EXPECTED_VALUE);
	}

	return value;
}

JsonValue* json_parse(const char* string) {
	size_t _;
	return __parse(string, &_);
}

char* json_serialize(JsonValue* value) {
	String out;
	json_strbuf_init(&out);

	switch (value->type) {
		case JSON_TYPE_NULL:
			json_strbuf_append_str(&out, "null");
			break;
		case JSON_TYPE_BOOL: {}
			char* tf = value->data.boolean ? "true" : "false";
			json_strbuf_append_str(&out, tf);
			break;
		case JSON_TYPE_NUMBER: {}
			char buf[256] = { 0 };
			snprintf(buf, 256, "%g", value->data.number);
			json_strbuf_append_str(&out, buf);
			break;
		case JSON_TYPE_STRING: {}
			char* string = json_string_serialize(value->data.string);
			json_strbuf_append_str(&out, string);
			free(string);
			break;
		case JSON_TYPE_OBJECT:
			json_strbuf_push(&out, '{');

			for (JsonItem** ji = value->data.object; *ji; ji++) {
				if (ji != value->data.object)
					json_strbuf_push(&out, ',');
				
				char* name = json_string_serialize((*ji)->name);
				json_strbuf_append_str(&out, name);
				free(name);

				json_strbuf_push(&out, ':');

				char* value = json_serialize((*ji)->value);
				json_strbuf_append_str(&out, value);
				free(value);
			}

			json_strbuf_push(&out, '}');
			break;
		case JSON_TYPE_ARRAY:
			json_strbuf_push(&out, '[');

			for (JsonValue** jv = value->data.array; *jv; jv++) {
				if (jv != value->data.array)
					json_strbuf_push(&out, ',');
				
				char* value = json_serialize(*jv);
				json_strbuf_append_str(&out, value);
				free(value);
			}

			json_strbuf_push(&out, ']');
			break;
	}

	return json_strbuf_deref(&out);
}

void json_free(JsonValue* value) {
	switch (value->type) {
		case JSON_TYPE_STRING:
			free(value->data.string);
			break;
		case JSON_TYPE_OBJECT:
			for (JsonItem** ji = value->data.object; *ji; ji++) {
				json_free((*ji)->value);
				free((*ji)->name);
			}

			free(value->data.object);
			break;
		case JSON_TYPE_ARRAY:
			for (JsonValue** jv = value->data.array; *jv; jv++)
				json_free(*jv);

			free(value->data.array);
			break;
		default:
			break;
	}

	free(value);
}
