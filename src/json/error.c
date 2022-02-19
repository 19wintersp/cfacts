#include "error.h"
#include "json.h"

static JsonError last_error = JSON_SUCCESS;

void json_set_error(JsonError err) {
	last_error = err;
}

JsonError json_last_error() {
	return last_error;
}
