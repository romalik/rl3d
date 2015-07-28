#ifndef TYPES_H_
#define TYPES_H_

struct Viewport {
	int w;
	int h;
	int * data;
	float fov;
};

struct PlayerState {                                                                                          
    float x;                                                                                                  
    float y;                                                                                                  
    float yaw;                                                                                                
	float df;
	float ds;
	float dyaw;                                                                                                              
};

struct Map {                                                                                                  
    int w;                                                                                                    
    int h;                                                                                                    
    int * data;                                                                                               
};
#endif
