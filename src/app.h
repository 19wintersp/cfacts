#ifndef APP_H
#define APP_H

void print_help(char* argv0);
void print_version();

void print_today(char* source);
void print_ago(char* source, int days);
void print_index(char* source, int index);
void print_random(char* source);

void add_source(char* name, char* url);
void print_source(char* name);

#endif // ifndef APP_H
