#ifndef IO_GENERIC_H_
#define IO_GENERIC_H_
#include "config.h"
#include "types.h"
#if IO_STDIO == 1
	#include "io_stdio.h"
	#define load_map generic_load_map
	#define refresh_viewport stdio_refresh_viewport
	#define io_init stdio_io_init
	#define io_uninit stdio_io_uninit
	#define io_getEvent stdio_io_getEvent
	#define loadTexture generic_loadTexture
#endif
#if IO_SDL == 1
	#include "io_sdl.h"
	#define load_map generic_load_map
	#define refresh_viewport sdl_refresh_viewport
	#define io_init sdl_io_init
	#define io_uninit sdl_io_uninit
	#define io_getEvent sdl_io_getEvent
	#define loadTexture sdl_loadTexture
#endif

void generic_load_map(struct Map * map, char * path);
Texture * generic_loadTexture(char * path);
#endif
