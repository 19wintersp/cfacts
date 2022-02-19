#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <time.h>

#include "api.h"
#include "app.h"
#include "config.h"
#include "program.h"

noreturn static void print_error(char* error) {
	fprintf(stderr, PROGRAM_NAME ": %s\n", error);

	exit(2);
}

void print_help(char* argv0) {
	printf("Usage: %s [OPTION]... [SOURCE]\n", argv0);
	puts("Fetch and display a cool fact.");

	puts("");

	puts("In normal operation, SOURCE is used as the fact source.");
	puts("If it is omitted, a random one will be chosen.");

	puts("");

	puts("-h, --help      display this help and exit");
	puts("-v, --version   display version information");

	puts("");

	puts("-t, --today     display today's fact (default)");
	puts("-a, --ago=DAYS  display the fact from DAYS days ago");
	puts("-n, --index=N   display fact number N");
	puts("-r, --random    display a random fact");

	puts("");

	puts("-A, --add=URL   add URL as a new source");
	puts("                SOURCE is required for the name");
	puts("-l, --list      display the list of sources");

	puts("");

	puts(PACKAGE_LICENSE);
}

void print_version() {
	puts(PACKAGE_STRING);
	puts(PACKAGE_FCOPY);
	puts(COMPILE_INFO);

	puts("");

	puts(PACKAGE_LICENSE);
}

static char* get_source_url(char* name) {
	FactSource** sources = get_fact_sources();

	if (sources == NULL) print_error("failed to load sources");

	int i;

	if (name == NULL) {
		FactSource** source = sources;
		for (; *source; source++);
		size_t length = source - sources;

		if (length == 0) print_error("no sources");

		srand(time(NULL));
		i = rand() % length;
	} else {
		for (FactSource** source = sources; *source; source++)
			if (strcmp((*source)->name, name) == 0) {
				i = source - sources;
				goto end;
			}
		
		print_error("cannot find named source");
	}

end: {}
	char* buf = malloc(strlen(sources[i]->url) + 1);
	strcpy(buf, sources[i]->url);

	free_fact_sources(sources);

	return buf;
}

void print_today(char* source) {
	char* base = get_source_url(source);
	char* fact = api_today(base);

	free(base);

	if (fact != NULL) {
		puts(fact);
		free(fact);
	} else {
		print_error("failed to fetch fact");
	}
}

void print_ago(char* source, int days) {
	char* base = get_source_url(source);
	char* fact = api_ago(base, days);

	free(base);

	if (fact != NULL) {
		puts(fact);
		free(fact);
	} else {
		print_error("failed to fetch fact");
	}
}

void print_index(char* source, int index) {
	char* base = get_source_url(source);
	char* fact = api_index(base, index);

	free(base);

	if (fact != NULL) {
		puts(fact);
		free(fact);
	} else {
		print_error("failed to fetch fact");
	}
}

void print_random(char* source) {
	char* base = get_source_url(source);
	char* fact = api_random(base);

	free(base);

	if (fact != NULL) {
		puts(fact);
		free(fact);
	} else {
		print_error("failed to fetch fact");
	}
}

void add_source(char* name, char* url) {
	FactSource** sources = get_fact_sources();

	if (sources == NULL) print_error("failed to load sources");

	FactSource** source = sources;
	for (; *source; source++);
	size_t length = source - sources;

	sources = realloc(sources, (length + 2) * sizeof(FactSource*));

	FactSource new = { name, url };
	sources[length] = &new;

	if (update_fact_sources(sources) != 0)
		print_error("failed to update sources");
	
	sources[length] = NULL;
	
	free_fact_sources(sources);
}

void print_source(char* name) {
	FactSource** sources = get_fact_sources();

	if (sources == NULL) print_error("failed to load sources");

	for (FactSource** source = sources; *source; source++)
		if (name == NULL || strcmp(name, (*source)->name) == 0)
			printf("%s: %s\n", (*source)->name, (*source)->url);

	free_fact_sources(sources);
}
