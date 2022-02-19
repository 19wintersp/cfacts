#include <stdlib.h>
#include <string.h>

#include "json.h"
#include "util.h"

JsonValue* json_null_new() {
	JsonValue* out = malloc(sizeof(JsonValue));
	out->type = JSON_TYPE_NULL;

	return out;
}

JsonValue* json_bool_new(bool value) {
	JsonValue* out = malloc(sizeof(JsonValue));
	out->type = JSON_TYPE_BOOL;
	out->data = (JsonData) { .boolean = value };

	return out;
}

JsonValue* json_number_new(double value) {
	JsonValue* out = malloc(sizeof(JsonValue));
	out->type = JSON_TYPE_NUMBER;
	out->data = (JsonData) { .number = value };

	return out;
}

JsonValue* json_string_new(const char* value) {
	char* string = malloc(strlen(value) + 1);
	strcpy(string, value);

	JsonValue* out = malloc(sizeof(JsonValue));
	out->type = JSON_TYPE_STRING;
	out->data = (JsonData) { .string = string };

	return out;
}

JsonValue* json_object_new() {
	JsonItem** items = malloc(sizeof(JsonItem*) * 8);
	items[0] = NULL;

	JsonValue* out = malloc(sizeof(JsonValue));
	out->type = JSON_TYPE_OBJECT;
	out->data = (JsonData) { .object = items };

	return out;
}

JsonValue* json_object_set(JsonValue* this, const char* name, JsonValue* value) {
	JsonItem** ji = this->data.object;
	for (; *ji; ji++)
		if (strcmp((*ji)->name, name) == 0) {
			JsonValue* old = (*ji)->value;
			(*ji)->value = value;

			return old;
		}
	
	char* string = malloc(strlen(name) + 1);
	strcpy(string, name);

	JsonItem* item = malloc(sizeof(JsonItem));
	item->name = string;
	item->value = value;

	size_t item_count = ji - this->data.object;

	this->data.object[item_count] = item;

	if (item_count % 8 == 7)
		this->data.object = realloc(
			this->data.object, sizeof(JsonItem*) * (item_count + 9)
		);

	this->data.object[item_count + 1] = NULL;

	return NULL;
}

JsonValue* json_object_get(const JsonValue* this, const char* name) {
	for (JsonItem** ji = this->data.object; *ji; ji++)
		if (strcmp((*ji)->name, name) == 0) return (*ji)->value;
	
	return NULL;
}

JsonValue* json_array_new() {
	JsonValue** values = malloc(sizeof(JsonValue*) * 8);
	values[0] = NULL;

	JsonValue* out = malloc(sizeof(JsonValue));
	out->type = JSON_TYPE_ARRAY;
	out->data = (JsonData) { .array = values };

	return out;
}

size_t json_array_length(const JsonValue* this) {
	JsonValue** jv = this->data.array;
	for (; *jv; jv++);

	return jv - this->data.array;
}

size_t json_array_push(JsonValue* this, JsonValue* value) {
	size_t length = json_array_length(this);

	this->data.array[length] = value;

	if (length % 8 == 7)
		this->data.array = realloc(
			this->data.array, sizeof(JsonValue*) * (length + 9)
		);

	this->data.array[length + 1] = NULL;

	return length + 1;
}

JsonValue* json_array_set(JsonValue* this, size_t index, JsonValue* value) {
	// this is ub according to the docs, perhaps we should abort()?
	if (index >= json_array_length(this)) return NULL;
	
	JsonValue* old = this->data.array[index];
	this->data.array[index] = value;

	return old;
}

JsonValue* json_array_get(const JsonValue* this, size_t index) {
	if (index >= json_array_length(this)) return NULL;

	return this->data.array[index];
}
