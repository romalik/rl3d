#ifndef IO_SDL_H_
#define IO_SDL_H_
#include "types.h"

void sdl_refresh_viewport(struct Viewport * vp);
void sdl_io_init(struct Viewport * vp);
void sdl_io_uninit(struct Viewport * vp);
int sdl_io_getEvent();
Texture * sdl_io_loadTexture(char * path);
#endif
