#include "io_sdl.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdint.h>



static SDL_Window* window = NULL;
static SDL_Surface* screenSurface = NULL;
static SDL_Renderer* renderer = NULL;




void sdl_load_map(struct Map * map, char * path) {
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

void sdl_refresh_viewport(struct Viewport * vp) {
    SDL_LockSurface(screenSurface);
    int r = 0;
    int c = 0;
    for(r = 0; r<vp->h; r++) {
        for(c = 0; c<vp->w; c++) {
            ((uint32_t *)(screenSurface->pixels))[r*vp->w + c] = vp->data[r*vp->w + c] == ' '?0x00ffffff:0x00ff0000;
        }
    }

    SDL_UnlockSurface(screenSurface);

    SDL_UpdateWindowSurface( window );


/*
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
*/
}

/* KBHIT */                                                                                                   
                                                                                                              
#include <stdio.h>
#include <termios.h>                                                                                          
#include <unistd.h>                                                                                           
#include <fcntl.h>                                                                                            
                                                                                                              
static int kbhit(void)
{                                                                                                             
  struct termios oldt, newt;                                                                                  
  int ch;                                                                                                     
  int oldf;                                                                                                   
                                                                                                              
  tcgetattr(STDIN_FILENO, &oldt);                                                                             
  newt = oldt;                                                                                                
  newt.c_lflag &= ~(ICANON | ECHO);                                                                           
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);                                                                    
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);                                                                     
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);                                                            
                                                                                                              
  ch = getchar();                                                                                             
	while(getchar() != EOF) {}                                                                                                               
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);                                                                    
  fcntl(STDIN_FILENO, F_SETFL, oldf);                                                                         
                                                                                                              
  if(ch != EOF)                                                                                               
  {                                                                                                           
    ungetc(ch, stdin);                                                                                        
    return 1;                                                                                                 
  }                                                                                                           
                                                                                                              
  return 0;                                                                                                   
}                                                                                                             
                                                                                                              
/* END KBHIT */    

void sdl_io_init(struct Viewport * vp) {

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
        //Create window
        window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, vp->w, vp->h, SDL_WINDOW_SHOWN );
//        SDL_CreateWindowAndRenderer(vp->w, vp->h, SDL_WINDOW_SHOWN, &window, &renderer);
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get window surface
            screenSurface = SDL_GetWindowSurface( window );

            //Fill the surface white
            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );

            //Update the surface
            SDL_UpdateWindowSurface( window );
        }
    }

}
void sdl_io_uninit(struct Viewport * vp) {
    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();

}
int sdl_io_getEvent() {
	int event = 0;
	    if(kbhit()) {
			event = getchar();                                                                                    
		}
	return event;
}


