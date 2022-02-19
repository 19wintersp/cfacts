#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "json.h"
#include "strbuf.h"
#include "string.h"

static char __unescape_seq(char escape) {
	switch (escape) {
		case '"':
		case '\\':
		case '/':
			return escape;
		case 'b':
			return 0x08;
		case 'f':
			return 0x0c;
		case 'n':
			return 0x0a;
		case 'r':
			return 0x0d;
		case 't':
			return 0x09;
		case 'u':
			return -1;
		default:
			return -2;
	}
}

static void __cp_to_utf8(unsigned short cp, char buf[3]) {
	if (cp <= 0x7f) {
		buf[0] = cp;
	} else if (cp <= 0x7ff) {
		buf[0] = 0xc0 | (cp >> 6);
		buf[1] = 0x80 | (cp & 0x3f);
	} else {
		buf[0] = 0xc0 | (cp >> 12);
		buf[1] = 0x80 | ((cp >> 6) & 0x3f);
		buf[2] = 0x80 | (cp & 0x3f);
	}
}

char* json_string_parse(const char* string, size_t* length) {
	*length = 1;

	String out;
	json_strbuf_init(&out);

	if (string[0] != '"') {
		json_strbuf_free(&out);
		RETURN_ERR(JSON_ERROR_EXPECTED_STRING);
	}

	for (char ch;; *length += 1)
		switch (ch = string[*length]) {
			case 0:
				RETURN_ERR(JSON_ERROR_UNTERMINATED_STRING);
			case '"':
				goto end;
			case '\\': {}
				*length += 1;
				char esc = __unescape_seq(string[*length]);

				if (esc > 0) {
					json_strbuf_push(&out, esc);
					break;
				}

				if (esc == -1) {
					char hexdig[5] = {
						string[*length + 1],
						string[*length + 2],
						string[*length + 3],
						string[*length + 4],
						0
					};

					*length += 4;

					char buf[4] = { 0 };

					long cp = strtol(hexdig, NULL, 16);
					__cp_to_utf8((unsigned short) cp, buf);

					json_strbuf_append_str(&out, buf);
					break;
				}

				RETURN_ERR(JSON_ERROR_INVALID_STRING_ESCAPE);
			default:
				json_strbuf_push(&out, ch);
				break;
		}

end:
	*length += 1;

	return json_strbuf_deref(&out);
}

char* json_string_serialize(const char* string) {
	String out;
	json_strbuf_init(&out);

	json_strbuf_push(&out, '"');

	for (int i = 0; string[i]; i++)
		if (string[i] == '\\') {
			json_strbuf_append_str(&out, "\\\\");
		} else if (string[i] == '"') {
			json_strbuf_append_str(&out, "\\\"");
		} else if ((unsigned) string[i] < 0x20) {
			char escape_seq[7] = { '\\', 'u', '0', '0', 0 };
			sprintf(escape_seq + 4, "%02x", string[i]);

			json_strbuf_append_str(&out, escape_seq);
		} else {
			json_strbuf_push(&out, string[i]);
		}

	json_strbuf_push(&out, '"');

	return json_strbuf_deref(&out);
}
