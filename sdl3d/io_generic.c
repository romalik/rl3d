#include "io_generic.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

Texture * generic_loadTexture(char * path) {
	printf("Texture load not implemented!\n");
} 
void generic_load_map(struct Map * map, char * path) {
        FILE * fd = fopen(path, "r");
        int w = 0;
        int h = 0;
		char colorType[1000];
		int colorMode = 0;
        fscanf(fd, "%d %d\n", &w, &h);
		fscanf(fd, "%s\n", colorType);
        printf("Loading map %s : %dx%d as %s\n" , path, w, h, colorType);
        map->data = (int *)malloc(w*h*sizeof(int));
        map->w = w;
        map->h = h;                   
		if(!strcmp(colorType,"symbol"))
			colorMode = 1;
		else if(!strcmp(colorType,"color"))
			colorMode = 2;
                                                                                                         
        int i = 0;                                                                                                                             
        while(i < w*h) {                                                                                                                       
                int c = fgetc(fd);                                                                                                             
                if(c == EOF)                                                                                                                   
                        break;                                                                                                                 
                                                                                                                                               
                if(c == '\n' || c == '\r')                                                                                                     
                        continue;                                                                                                              

                if(colorMode == 1) {

					if(c == '.') c = 0;

					map->data[i] = c & 0xff;
				} else if(colorMode == 2) {
					int val = 0;
					if(c == 'k')
						val = 0xFFE0E0E0;
					else if(c == 'g')
						val = 0xFF00FF00;
					else if(c == 'r')
						val = 0xFFFF0000;
					else if(c == 'b')
						val = 0xFF0000FF;
					else if(c == '.')
						val = 0x00000000;


					map->data[i] = val;
				}					                                                                                                       
                i++;
        }
/*
        int j = 0;
        for(i = 0; i<h; i++) {
                for(j = 0; j<w; j++) {
                        printf("%c", map->data[i*w + j]);
                }
                printf("\n");
        }
*/
}

