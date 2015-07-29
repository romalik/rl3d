#define VIEWPORT_W 800
#define VIEWPORT_H 400

#define IO_STDIO 0
#define IO_SDL 1

#if IO_STDIO == 1
    #define CEILING_COLOR '.'
    #define FLOOR_COLOR '-'
#endif

#if IO_SDL == 1
    #define CEILING_COLOR 0x000000ff
    #define FLOOR_COLOR 0x0000ff00
#endif

#define RENDER_BORDERS 1
#define RENDER_TEXTURES 0

#define TRANSPARENT_TEXTURES 0

#define ANIMATED_TEXTURES 0

#define RENDER_SPRITES 0
#define ANIMATIED_SPRITES 0

#define RENDER_BG_SOLID 1
#define RENDER_BG_IMAGE 0

#define CHECK_COLLISIONS 0

#define INTERACTIVE_OBJECTS 0


