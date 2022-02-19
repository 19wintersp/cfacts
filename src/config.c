#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"

#ifdef PLATFORM_POSIX
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif // ifdef PLATFORM_POSIX

#ifdef PLATFORM_WINDOWS
#include <fileapi.h>
#include <shlwapi.h>
#endif // ifdef PLATFORM_WINDOWS

#include "config.h"
#include "json/json.h"
#include "json/util.h"

static int recursive_mkdir(const char* path) {
	int count = 0;
	char swap;

	size_t current = strcspn(path, "/\\");

	size_t length = strlen(path);
	char* buf = malloc(length);
	memcpy(buf, path, length);

	for (; current < length; count++) {
		current += strcspn(buf + current + 1, "/\\") + 1;
		swap = buf[current];
		buf[current] = 0;

#ifdef PLATFORM_POSIX
		
		DIR* dir = opendir(buf);
		if (dir) closedir(dir);
		else if (
			errno != ENOENT ||
			mkdir(buf, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1
		) {
			count = -1;
			break;
		}

#endif // ifdef PLATFORM_POSIX

#ifdef PLATFORM_WINDOWS

		if (
			!PathIsDirectoryA(buf) &&
			CreateDirectoryA(buf, NULL) == 0
		) {
			count = -1;
			break;
		}

#endif // ifdef PLATFORM_WINDOWS

		buf[current] = swap;
	}

	free(buf);
	return count;
}

static char* get_config_file() {
	const char* base = getenv(BASE_DIR);
	if (base == NULL) return NULL;

	int length = snprintf(NULL, 0, "%s/" CONFIG_FILE, base);

	char* buf = malloc(length + 1);
	sprintf(buf, "%s/" CONFIG_DIR, base);

	if (recursive_mkdir(buf) == -1) return NULL;

	sprintf(buf, "%s/" CONFIG_FILE, base);

	return buf;
}

FactSource** get_fact_sources() {
	// FIXME: this function leaks on error!

	char* config_file = get_config_file();
	if (config_file == NULL) return NULL;

	FILE* file = fopen(config_file, "r");

	free(config_file);

	if (file == NULL) {
		// if the file doesn't exist, return a ptr to a null ptr
		// if there was another error, return just a null ptr

		if (errno != ENOENT) return NULL;
		else return calloc(1, sizeof(FactSource*));
	}

	char* buf = malloc(16384);
	if (fgets(buf, 16384, file) == NULL) return NULL;

	if (fclose(file) == EOF) return NULL;

	JsonValue* json = json_parse(buf);
	if (json == NULL || json->type != JSON_TYPE_ARRAY)
		return NULL;

	size_t n = json_array_length(json);
	FactSource** srcs = malloc((n + 1) * sizeof(FactSource*));

	for (size_t i = 0; i < n; i++) {
		JsonValue* json_item = json_array_get(json, i);
		if (json_item == NULL || json_item->type != JSON_TYPE_OBJECT)
			return NULL;
		
		JsonValue* json_name = json_object_get(json_item, "name");
		if (json_name == NULL || json_name->type != JSON_TYPE_STRING)
			return NULL;
		
		JsonValue* json_url = json_object_get(json_item, "url");
		if (json_url == NULL || json_url->type != JSON_TYPE_STRING)
			return NULL;
		
		char* name = malloc(strlen(json_name->data.string) + 1);
		strcpy(name, json_name->data.string);

		char* url = malloc(strlen(json_url->data.string) + 1);
		strcpy(url, json_url->data.string);
		
		FactSource* src = malloc(sizeof(FactSource));
		src->name = name;
		src->url = url;

		srcs[i] = src;
	}

	json_free(json);

	srcs[n] = NULL;

	return srcs;
}

int update_fact_sources(FactSource** sources) {
	JsonValue* json = json_array_new();

	for (; *sources; sources++) {
		JsonValue* json_item = json_object_new();
		json_object_set(json_item, "name", json_string_new((*sources)->name));
		json_object_set(json_item, "url", json_string_new((*sources)->url));

		json_array_push(json, json_item);
	}

	char* buf = json_serialize(json);
	json_free(json);

	char* config_file = get_config_file();
	if (config_file == NULL) return -1;

	FILE* file = fopen(config_file, "w");

	free(config_file);

	if (file == NULL) return -1;

	if (fputs(buf, file) == EOF) return -1;
	if (fclose(file) == EOF) return -1;

	free(buf);

	return 0;
}

void free_fact_sources(FactSource** sources) {
	for (FactSource** source = sources; *source; source++) {
		free((*source)->name);
		free((*source)->url);
	}

	free(sources);
}
