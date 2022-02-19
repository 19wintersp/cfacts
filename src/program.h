#ifndef PROGRAM_H
#define PROGRAM_H

#include "platform.h"

#define PROGRAM_NAME    "cfacts"
#define PACKAGE_DOT     "io.github._19wintersp.chr.cfacts"

#define PACKAGE_NAME    "CFacts"
#define PACKAGE_VERSION "0.1.0"
#define PACKAGE_STRING  PACKAGE_NAME " " PACKAGE_VERSION

#define PACKAGE_AUTHOR  "Patrick Winters"
#define PACKAGE_COPY    "Copyright (c) 2022"
#define PACKAGE_FCOPY   PACKAGE_COPY " " PACKAGE_AUTHOR

#define COMPILE_DATE "on " __DATE__ " "
#define COMPILE_FOR  "for " PLATFORM_NAME " "
#define COMPILE_INFO "Compiled " COMPILE_DATE COMPILE_FOR

#endif // ifndef PROGRAM_H
