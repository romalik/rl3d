#!/bin/sh
gcc  -o main ./main.c ./io_stdio.c ./core.c ./io_sdl.c ./io_generic.c -lm -lSDL2
