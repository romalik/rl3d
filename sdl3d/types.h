#ifndef TYPES_H_
#define TYPES_H_

#define EVENT_NONE  0x00
#define EVENT_FORWARD  0x01
#define EVENT_BACKWARD  0x02
#define EVENT_TURN_LEFT  0x04
#define EVENT_TURN_RIGHT  0x08
#define EVENT_STRAFE_LEFT  0x10
#define EVENT_STRAFE_RIGHT  0x20
#define EVENT_ACTION  0x40
#define EVENT_SPRINT  0x80
#define EVENT_FIRE  0x100

#define EVENT_QUIT  0x200

typedef struct Viewport {
	int w;
	int h;
	int * data;
	float fov;
} Texture;


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
