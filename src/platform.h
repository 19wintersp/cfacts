#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __linux__
#define PLATFORM_LINUX
#define PLATFORM_POSIX
#define PLATFORM_NAME "Linux"
#endif // ifdef __unix__

#if defined(__DragonFly__) || defined(__FreeBSD__) || \
	defined(__NetBSD__) || defined(__OpenBSD__)
#define PLATFORM_BSD
#define PLATFORM_POSIX
#define PLATFORM_NAME "BSD"
#endif // ifdef __DragonFly__ || __FreeBSD__ || __NetBSD__ || __OpenBSD__

#ifdef __APPLE__
#define PLATFORM_APPLE
#define PLATFORM_POSIX
#define PLATFORM_NAME "Apple"
#endif // ifdef __APPLE__

#ifdef _WIN32
#define PLATFORM_WINDOWS
#define PLATFORM_NAME "Windows"
#endif // ifdef _WIN32

#ifndef PLATFORM_NAME
#error unsupported platform!
#endif // ifndef PLATFORM_NAME

#endif // ifndef PLATFORM_H
