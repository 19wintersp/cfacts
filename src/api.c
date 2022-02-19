#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "api.h"
#include "json/json.h"
#include "json/util.h"

typedef struct {
	char* data;
	size_t length;
} DataBuffer;

static size_t write_callback(
	char* data, size_t one, size_t length, DataBuffer* user
) {
	if (length == 0 || one != 1) return 0;

	size_t old_length = user->length;

	user->length += length;
	user->data = realloc(user->data, user->length + 1);

	memcpy(user->data + old_length - 1, data, length);
	user->data[user->length] = 0;

	return length;
}

char* api_fetch(char* url) {
	DataBuffer data = { calloc(1, 1), 1 };

	CURL* curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10);
	curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_NONE);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1);

	CURLcode res = curl_easy_perform(curl);

	long code;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

	curl_easy_cleanup(curl);

	if (res == CURLE_OK && (code >= 200 && code < 300)) {
		return data.data;
	} else {
		free(data.data);
		return NULL;
	}
}

static char* format_fact(const char* res) {
	JsonValue* json = json_parse(res);

	if (json == NULL) return NULL;

	JsonValue* json_fact = json_object_get(json, "fact");
	JsonValue* json_index = json_object_get(json, "index");

	if (
		json_fact == NULL || json_fact->type != JSON_TYPE_STRING ||
		json_index == NULL || json_index->type != JSON_TYPE_NUMBER
	) {
		json_free(json);
		return NULL;
	}

	char* fact = json_fact->data.string;
	double index = json_index->data.number + 1;

	size_t length = snprintf(NULL, 0, "%.0f: %s", index, fact);

	char* buf = malloc(length);
	sprintf(buf, "%.0f: %s", index, fact);

	json_free(json);

	return buf;
}

char* api_today(char* base) {
	size_t length = snprintf(NULL, 0, API_TODAY_PATH, base);

	char buf[length];
	sprintf(buf, API_TODAY_PATH, base);

	char* res = api_fetch(buf);
	char* fact = format_fact(res);
	free(res);

	return fact;
}

char* api_ago(char* base, int days) {
	size_t length = snprintf(NULL, 0, API_AGO_PATH, base, days);

	char buf[length];
	sprintf(buf, API_AGO_PATH, base, days);

	char* res = api_fetch(buf);
	char* fact = format_fact(res);
	free(res);

	return fact;
}

char* api_index(char* base, int index) {
	size_t length = snprintf(NULL, 0, API_INDEX_PATH, base, index);

	char buf[length];
	sprintf(buf, API_INDEX_PATH, base, index);

	char* res = api_fetch(buf);
	char* fact = format_fact(res);
	free(res);

	return fact;
}

char* api_random(char* base) {
	size_t length = snprintf(NULL, 0, API_RANDOM_PATH, base);

	char buf[length];
	sprintf(buf, API_RANDOM_PATH, base);

	char* res = api_fetch(buf);
	char* fact = format_fact(res);
	free(res);

	return fact;
}
