#ifndef JSON_STRBUF_H
#define JSON_STRBUF_H

#define JSON_STRBUF_BLOCK_SIZE 64

typedef struct {
	char* data;
	size_t length;
	size_t capacity;
} String;

void json_strbuf_init(String* buf);
void json_strbuf_append(String* buf, String* src);
void json_strbuf_append_str(String* buf, const char* src);
void json_strbuf_push(String* buf, char ch);
void json_strbuf_free(String* buf);
char* json_strbuf_deref(String* buf);

#endif // ifndef JSON_STRBUF_H
