#include <stdlib.h>
#include <string.h>

#include "strbuf.h"

void json_strbuf_init(String* buf) {
	buf->capacity = JSON_STRBUF_BLOCK_SIZE;
	buf->data = calloc(buf->capacity, 1);
	buf->length = 0;
}

static void __append(String* buf, const char* src, size_t src_length) {
	size_t new_size = buf->length + src_length + 1;
	if (buf->capacity < new_size) {
		int blocks = (new_size / JSON_STRBUF_BLOCK_SIZE + 1);
		
		buf->data = realloc(buf->data, blocks * JSON_STRBUF_BLOCK_SIZE);
		buf->capacity = blocks * JSON_STRBUF_BLOCK_SIZE;
	}

	memcpy(buf->data + buf->length, src, src_length + 1);

	buf->length += src_length;
}

void json_strbuf_append(String* buf, String* src) {
	__append(buf, src->data, src->length);
}

void json_strbuf_append_str(String* buf, const char* src) {
	__append(buf, src, strlen(src));
}

void json_strbuf_push(String* buf, char ch) {
	char src[2] = { ch, 0 };
	__append(buf, src, 1);
}

void json_strbuf_free(String* buf) {
	free(buf->data);
}

char* json_strbuf_deref(String* buf) {
	return buf->data;
}
