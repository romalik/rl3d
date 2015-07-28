#include "io_stdio.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
void stdio_load_map(struct Map * map, char * path) {
	FILE * fd = fopen(path, "r");
	int w = 0;
	int h = 0;
	fscanf(fd, "%d %d\n", &w, &h);
	printf("Loading map %s : %dx%d\n" , path, w, h);
	map->data = (int *)malloc(w*h*sizeof(int));
	map->w = w;
	map->h = h;
	int i = 0;
	while(i < w*h) {
		int c = fgetc(fd);
		if(c == EOF)
			break;

		if(c == '\n' || c == '\r')
			continue;

		map->data[i] = c & 0xff;
		i++;
	}

	int j = 0;
	for(i = 0; i<h; i++) {
		for(j = 0; j<w; j++) {
			printf("%c", map->data[i*w + j]);
		}
		printf("\n");
	}
}

void stdio_refresh_viewport(struct Viewport * vp) {
	static int frame = 0;
	int c = 0;
	int r = 0;

	printf("Frame %d\n", frame);
	for(c = 0; c<vp->w+2; c++) {
		printf("=");
	}
	printf("\n");
	for(r = 0; r<vp->h; r++) {
		printf("|");
		for(c = 0; c<vp->w; c++) {
			printf("%c",vp->data[r*vp->w + c]);
		}
		printf("|\n");
	}
	for(c = 0; c<vp->w+2; c++) {
		printf("=");
	}
	printf("\n");
	frame++;

}
