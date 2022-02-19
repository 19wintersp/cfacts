#ifndef ARGS_H
#define ARGS_H

typedef char* Arg[2];

Arg* args_parse(int argc, char* argv[]);
void args_free(Arg* args);

#endif // ifndef ARGS_H
