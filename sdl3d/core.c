#include "core.h"
#include "io_generic.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

static struct PlayerState playerState;
static struct Map map;
static struct Viewport viewport;

static float * zBuffer;

int maxIntersections = 100;

struct Intersection {
	int type;
	float distance;
	float offset;
};

struct Intersection * intersectionBuffer;


int core_init(int argc, char ** argv) {
	load_map(&map, argv[1]);

	playerState.x = 1;
	playerState.y = -2;
	playerState.yaw = 0;
	playerState.df = 0;
	playerState.ds = 0;
	playerState.dyaw = 0;

	viewport.w = 80;
	viewport.h = 40;
	viewport.data = (int*)malloc(viewport.w * viewport.h * sizeof(int));
	viewport.fov = M_PI/3.0f;

	intersectionBuffer = (struct Intersection *)malloc(maxIntersections * viewport.w * sizeof(struct Intersection));

	zBuffer = (float *)malloc(viewport.h * viewport.w * sizeof(float));

	return 0;
}



void getAllIntersections(float x, float y, float yaw) {
	int i;
    for(i = 0; i<viewport.w; i++) {
        float cBeamAngle = yaw + ((float)viewport.w/2.0f - (float)i) * (viewport.fov / (float)viewport.w);

		int cIntersection = 0;

        while(cBeamAngle < -M_PI) cBeamAngle += 2.0f*M_PI;
        while(cBeamAngle > M_PI) cBeamAngle -= 2.0f*M_PI;

        //get intersections with vertical lines
		int n;
        for(n = 0; n < map.w; n++) {
            float dist = ((float)n - x)/cos(cBeamAngle);
            if(dist < 0)
                continue;

            float xi = n;
            float yi = y + ((float)n-x)*sin(cBeamAngle)/cos(cBeamAngle);
            int xm = (int)xi;

            if(cos(cBeamAngle) < 0)
                xm--;

            int ym = - (int)yi;

            if(xm < 0 || xm >= map.w || ym < 0 || ym >= map.h)
                continue;

            
            if(map.data[ym * map.w + xm] != '.') {
                
                intersectionBuffer[i*maxIntersections + cIntersection].type = map.data[ym * map.w + xm];
                intersectionBuffer[i*maxIntersections + cIntersection].distance = dist;
                if(cos(cBeamAngle) > 0) {
                    intersectionBuffer[i*maxIntersections + cIntersection].offset = 1 - ((int)yi - yi);
                } else {
                    intersectionBuffer[i*maxIntersections + cIntersection].offset = ((int)yi - yi);
                }
                cIntersection++;
            }
        }
		int m;
        for(m = 0; m > -map.h; m--) {
            float dist = ((float)m - y)/sin(cBeamAngle);
            if(dist < 0)
                continue;

            float yi = m;
            float xi = x + ((float)m-y)*cos(cBeamAngle)/sin(cBeamAngle);
            int xm = (int)xi;
            int ym = - (int)yi;

            if(sin(cBeamAngle) > 0)
                ym--;


            if(xm < 0 || xm >= map.w || ym < 0 || ym >= map.h)
                continue;

            

            if(map.data[ym * map.w + xm] != '.') {                                                      
                                                                                                              
                intersectionBuffer[i*maxIntersections + cIntersection].type = map.data[ym * map.w + xm];                
                intersectionBuffer[i*maxIntersections + cIntersection].distance = dist;                                        

                if(sin(cBeamAngle) > 0) {
                    intersectionBuffer[i*maxIntersections + cIntersection].offset = 1 - (xi - (int)xi);
                } else {
                    intersectionBuffer[i*maxIntersections + cIntersection].offset = (xi - (int)xi);
                }
                cIntersection++;
            }
        }


    }

}

void drawBg() {
	int i;
	for(i = 0; i<viewport.h * viewport.w; i++) {
		zBuffer[i] = 1000.0f;
		viewport.data[i] = ' ';
	}
}


void drawWalls() {
	int i = 0;
	int d = 0;
	for(i = 0; i<maxIntersections*viewport.w; i++) {
		intersectionBuffer[i].type = -1;
	}
	getAllIntersections(playerState.x, playerState.y, playerState.yaw);

    for(i = 0; i<viewport.w; i++) {                                                                        
        for(d = 0; d<maxIntersections; d++) {                                                                 
            int textype = intersectionBuffer[i*maxIntersections + d].type;
			if(textype == -1) break;                                                                     
            float offset = intersectionBuffer[i*maxIntersections + d].offset;                                                                   
            float distance = intersectionBuffer[i*maxIntersections + d].distance;                                                               
                                                                                                              
            float scaleFactor = 1.0f/distance;                                                                
            int targetStart = (1.0f - scaleFactor)*viewport.h/2.0f;                                          
            int targetEnd = (1.0f + scaleFactor)*viewport.h/2.0f;                                            
			int colour = textype;
			int r;
            for(r = max(targetStart,0); r<min(targetEnd, viewport.h); r++) {                    
                /*cv::Vec3b colour = texLine.at<cv::Vec3b>(texLine.rows*(r-targetStart)/(targetEnd-targetStart),0);;*/
                if(colour == 255) {                          
                    //transparent                                                                             
                } else {                                                                                      
                    if(zBuffer[r*viewport.w + i] > distance) {                                                   
                        viewport.data[r*viewport.w + i] = colour;                                                   
                        zBuffer[r*viewport.w + i] = distance;                                                    
                    }                                                                                         
                }                                                                                             
            }

			
                                                                                                 
        }                                                                                                     
    }                                                                                                         

}



/* KBHIT */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
 
int kbhit(void)
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

void calcNewPosition(float dt) {
    float dx = playerState.df * cos(playerState.yaw) + playerState.ds * sin(playerState.yaw);                                                             
    float dy = playerState.df * sin(playerState.yaw) - playerState.ds * cos(playerState.yaw);                                                             
    if(dt == 0) {                                                                                             
        return;                                                                                               
    }                                                                                                         
                                                                                                              
    playerState.x += dx * dt;                                                                                         
    playerState.y += dy * dt;                                                                                         
    playerState.yaw += playerState.dyaw * dt;                                                                                     
                                                                                                              
    while(playerState.yaw < -M_PI) playerState.yaw += 2.0f*M_PI;                                                                  
    while(playerState.yaw > M_PI) playerState.yaw -= 2.0f*M_PI;                                                                   
                                                                                                              
                                                                                                              
                                                                                                              
} 

double getTime() {                                                                                            
    struct timeval  tv;                                                                                       
    gettimeofday(&tv, NULL);                                                                                  
    return tv.tv_sec + (double)tv.tv_usec/1000000.0;                                                          
} 

void processEvent(int k) {


        if((char)k == 'q') {                                                                                  
            //break;                                                                                            
        } else if((char)k == ',') {                                                                           
            playerState.ds = -1.0f;                                                                                        
        } else if((char)k == '.') {                                                                           
            playerState.ds = 1.0f;                                                                                       
        } else if((char)k == 'a') {                                                                           
            playerState.dyaw = +1.0f;                                                                                   
        } else if((char)k == 'd') {                                                                           
            playerState.dyaw = -1.0f;                                                                                   
        } else if((char)k == 's') {                                                                           
            playerState.df = -5.0f;                                                                                       
        } else if((char)k == 'w') {                                                                           
            playerState.df = 5.0f;                                                                                        
		} else {
			playerState.ds = playerState.df = playerState.dyaw = 0;
		}
}

int core_loop() {

	static double prevTime = 0;

	if(prevTime == 0)
		prevTime = getTime();


	double currentTime = getTime();
	double dTime = currentTime - prevTime;
	prevTime = currentTime;

	calcNewPosition(dTime);
	drawBg();
	drawWalls();
	refresh_viewport(&viewport);
	printf("X: %f Y: %f YAW: %d\n", playerState.x, playerState.y, (int)(playerState.yaw*180.0f/M_PI));
	usleep(100*1000);

	if(kbhit()) {
		int c = getchar();
		processEvent(c);
	} else {
		processEvent(0);
	}

	

	return 0;
}

int core_uninit() {

	return 0;
}