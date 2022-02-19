#ifndef CONFIG_H
#define CONFIG_H

#include "platform.h"
#include "program.h"

#ifdef PLATFORM_LINUX
#define CONFIG_DIR ".config/" PROGRAM_NAME
#define BASE_DIR   "HOME"
#endif // ifdef PLATFORM_LINUX

#ifdef PLATFORM_APPLE
#define CONFIG_DIR "Library/Preferences/" PACKAGE_DOT
#define BASE_DIR   "HOME"
#endif // ifdef PLATFORM_APPLE

#ifdef PLATFORM_WINDOWS
#define CONFIG_DIR PROGRAM_NAME
#define BASE_DIR   "APPDATA"
#endif // ifdef PLATFORM_WINDOWS

#define CONFIG_FILE CONFIG_DIR "/config.json"

typedef struct {
	char* name;
	char* url;
} FactSource;

FactSource** get_fact_sources();
int update_fact_sources(FactSource** sources);
void free_fact_sources(FactSource** sources);

#endif // ifndef CONFIG_H
