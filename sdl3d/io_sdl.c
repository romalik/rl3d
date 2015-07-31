#include "io_sdl.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdint.h>



static SDL_Window* window = NULL;
static SDL_Surface* screenSurface = NULL;
static SDL_Renderer* renderer = NULL;


Texture * sdl_loadTexture(char * path) {
	Texture * dest;
	SDL_Surface * newBmp = SDL_LoadBMP(path);
	if(!newBmp) return 0;
	newBmp = SDL_ConvertSurface(newBmp, screenSurface->format, SDL_SWSURFACE);
	printf("BMP load ok\n");

	dest = (Texture *)malloc(sizeof (Texture));
	printf("Texture : %p \n", dest);
	dest->w = newBmp->w;
	dest->h = newBmp->h;
	printf("BMP: %d %d\n", dest->w, dest->h);
	dest->data = (uint32_t *)malloc(5*dest->w*dest->h*sizeof(uint32_t));
	memcpy(dest->data, newBmp->pixels, dest->w*dest->h*sizeof(uint32_t));

	return dest;
}

void sdl_refresh_viewport(struct Viewport * vp) {
    SDL_LockSurface(screenSurface);
/*
    int r = 0;
    int c = 0;
    for(r = 0; r<vp->h; r++) {
        for(c = 0; c<vp->w; c++) {
            ((uint32_t *)(screenSurface->pixels))[r*vp->w + c] = vp->data[r*vp->w + c];// == ' ' ? 
//				SDL_MapRGB( screenSurface->format, 0x00, 0x00, 0x00 ):
//				SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF );
        }
    }
*/

	memcpy(screenSurface->pixels, vp->data, vp->w*vp->h*sizeof(uint32_t));
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

int evMask = 0;
int sdl_io_getEvent() {
	SDL_Event event;
	while(SDL_PollEvent( &event )) {
	switch (event.type) {
		case SDL_KEYDOWN:
			switch ( event.key.keysym.sym ) {
				case SDLK_w:
					evMask |= EVENT_FORWARD;
					break;
				case SDLK_s:
					evMask |= EVENT_BACKWARD;
					break;
				case SDLK_a:
					evMask |= EVENT_TURN_LEFT;
					break;
				case SDLK_d:
					evMask |= EVENT_TURN_RIGHT;
					break;
				case SDLK_COMMA:
					evMask |= EVENT_STRAFE_LEFT;
					break;
				case SDLK_PERIOD:
					evMask |= EVENT_STRAFE_RIGHT;
					break;
				case SDLK_SPACE:
					evMask |= EVENT_ACTION;
					break;
				case SDLK_q:
					evMask |= EVENT_QUIT;
					break;
				case SDLK_LSHIFT:
					evMask |= EVENT_SPRINT;
					break;
				default:
					break;
			}
			break;
		case SDL_KEYUP:
			switch ( event.key.keysym.sym ) {
				case SDLK_w:
					evMask &= ~EVENT_FORWARD;
					break;
				case SDLK_s:
					evMask &= ~EVENT_BACKWARD;
					break;
				case SDLK_a:
					evMask &= ~EVENT_TURN_LEFT;
					break;
				case SDLK_d:
					evMask &= ~EVENT_TURN_RIGHT;
					break;
				case SDLK_COMMA:
					evMask &= ~EVENT_STRAFE_LEFT;
					break;
				case SDLK_PERIOD:
					evMask &= ~EVENT_STRAFE_RIGHT;
					break;
				case SDLK_SPACE:
					evMask &= ~EVENT_ACTION;
					break;
				case SDLK_q:
					evMask &= ~EVENT_QUIT;
					break;
				case SDLK_LSHIFT:
					evMask &= ~EVENT_SPRINT;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
	}
	return evMask;


/*
    int event = 0;
            if(kbhit()) {
                        int c = getchar();
                        if(c == 'w') {
                                event = EVENT_FORWARD;
                        } else if(c == 's') {
                                event = EVENT_BACKWARD;
                        } else if(c == 'a') {
                                event = EVENT_TURN_LEFT;
                        } else if(c == 'd') {
                                event = EVENT_TURN_RIGHT;
                        } else if(c == ',') {
                                event = EVENT_STRAFE_LEFT;
                        } else if(c == '.') {
                                event = EVENT_STRAFE_RIGHT;
                        } else if(c == ' ') {
                                event = EVENT_ACTION;
                        } else if(c == 'f') {
                                event = EVENT_FIRE;
                        } else if(c == 'q') {
                                event = EVENT_QUIT;
                        }                                                                       
                }
        return event;
*/
}



