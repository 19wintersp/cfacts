#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"

Arg* args_parse(int argc, char* argv[]) {
	int length = 0;
	Arg* args = malloc(8 * sizeof(Arg));

	bool no_more_args = false;

	for (int i = 1; i < argc; i++) {
		if (length % 8 == 7)
			args = realloc(args, (length + 9) * sizeof(Arg));
		
		if (no_more_args || argv[i][0] != '-') {
			char* value = malloc(strlen(argv[i]));
			strcpy(value, argv[i]);

			args[length][0] = NULL;
			args[length][1] = value;

			length++;

			continue;
		}

		if (argv[i][1] == '-') {
			if (argv[i][2] == 0) {
				no_more_args = true;
				continue;
			}

			size_t arg_len = strcspn(argv[i] + 2, "=");

			char* arg = malloc(arg_len + 1);
			strncpy(arg, argv[i] + 2, arg_len);
			arg[arg_len] = 0;

			char* value = NULL;

			if (argv[i][2 + arg_len] == '=') {
				value = malloc(strlen(argv[i] + 3 + arg_len) + 1);
				strcpy(value, argv[i] + 3 + arg_len);
			}

			args[length][0] = arg;
			args[length][1] = value;

			length++;
		} else {
			size_t arg_len = strcspn(argv[i] + 1, "=");

			for (size_t j = 0; j < arg_len; j++) {
				char* arg = malloc(2);
				arg[0] = argv[i][j + 1];
				arg[1] = 0;

				char* value = NULL;

				if (argv[i][j + 2] == '=') {
					value = malloc(strlen(argv[i] + j + 3) + 1);
					strcpy(value, argv[i] + j + 3);
				}

				args[length][0] = arg;
				args[length][1] = value;

				length++;
			}
		}
	}

	args[length][0] = NULL;
	args[length][1] = NULL;

	return args;
}

void args_free(Arg* args) {
	for (Arg* arg = args; (*arg)[0] || (*arg)[1]; arg++) {
		if ((*arg)[0]) free((*arg)[0]);
		if ((*arg)[1]) free((*arg)[1]);
	}

	free(args);
}
