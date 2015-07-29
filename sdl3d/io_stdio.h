#ifndef IO_STDIO_H_
#define IO_STDIO_H_
#include "types.h"

void stdio_load_map(struct Map * map, char * path);
void stdio_refresh_viewport(struct Viewport * vp);
void stdio_io_init(struct Viewport * vp);
void stdio_io_uninit(struct Viewport * vp);
int stdio_io_getEvent();
#endif
