#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#include "app.h"
#include "args.h"
#include "program.h"

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

static void print_error(char* error);

char* argv0;

int main(int argc, char* argv[]) {
	argv0 = argv[0];

	char* source = NULL;

	char mode = 0;
	char* value = NULL;

	struct {
		char mode;
		char* flag;
		bool value;
	} modes[] = {
		{ 'h', "help", false },
		{ 'v', "version", false },
		{ 't', "today", false },
		{ 'a', "ago", true },
		{ 'n', "index", true },
		{ 'r', "random", false },
		{ 'A', "add", true },
		{ 'l', "list", false }
	};

	// we're not particularly bothered about memory leaking here, since this
	// effectively lives for the whole lifetime of the program, and takes up
	// barely anything anyway - if we do end up adding a free call, fix that
	// assignment to `add` and `source` directly from the arg buffer
	Arg* args = args_parse(argc, argv);

	for (Arg* arg = args; (*arg)[0] || (*arg)[1]; arg++) {
		if (!(*arg)[0]) {
			if (source == NULL) source = (*arg)[1];
			else print_error("SOURCE may not appear multiple times");

			continue;
		}

		for (size_t i = 0; i < ARRAY_LEN(modes); i++)
			if (
				(
					(*arg)[0][1] == 0 &&
					(*arg)[0][0] == modes[i].mode
				) ||
				strcmp((*arg)[0], modes[i].flag) == 0
			) {
				if (mode) print_error("conflicting arguments");

				mode = modes[i].mode;

				if (modes[i].value) {
					if ((*arg)[1]) {
						value = (*arg)[1];
					} else if (!(*(arg + 1))[0] && (*(arg + 1))[1]) {
						value = (*(arg + 1))[1];
						arg++;
					} else {
						print_error("missing value");
					}
				} else if ((*arg)[1]) {
					print_error("unexpected value");
				}

				goto next;
			}
		
		print_error("unknown argument");

next: {}
	}

	switch (mode) {
		case 'h':
			print_help(argv0);
			break;
		case 'v':
			print_version();
			break;
		case 0:
		case 't':
			print_today(source);
			break;
		case 'a': {}
			int days = atoi(value);
			print_ago(source, days);
			break;
		case 'n': {}
			int index = atoi(value) - 1;
			print_index(source, index);
			break;
		case 'r':
			print_random(source);
			break;
		case 'A':
			if (source == NULL)
				print_error("SOURCE is required for adding");
			
			add_source(source, value);
			break;
		case 'l':
			print_source(source);
			break;
		default:
			print_error("unimplemented");
	}

	return 0;
}

noreturn static void print_error(char* error) {
	fprintf(stderr, PROGRAM_NAME ": %s\n", error);
	fprintf(stderr, "Usage: %s [OPTION]... [SOURCE]\n", argv0);
	fprintf(stderr, "Try '%s --help' for more information.\n", argv0);

	exit(1);
}
