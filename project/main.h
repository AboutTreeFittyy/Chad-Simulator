//Female Sprite source: Glorious DPRK Studios, https://opengameart.org/content/sweaty-obese-woman-in-dirty-burlap-underwear-spritesheets
//Background image source: Glorious DPRK Studios, //Female Sprite source: tgfcoder, https://opengameart.org/content/sweaty-obese-woman-in-dirty-burlap-underwear-spritesheets
//////////////////////////////////////////////////
// Game Programming All In One, Third Edition
// Chapter 11 - TimerTest
//////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "allegro.h"

#define MODE GFX_AUTODETECT_WINDOWED
#define WIDTH 1100
#define HEIGHT 900
#define NUMPLAYERSPRITES 1
#define NUMENEMIES 30
#define NUMTOTALSPRITES 5
#define CHADSPEED 15
#define BLACK makecol(0,0,0)
#define WHITE makecol(255,255,255)

//define the sprite structure
typedef struct SPRITE{
    int dir, alive;
    int x,y;
    int width,height;
    int xspeed,yspeed;
    int xdelay,ydelay;
    int xcount,ycount;
    int curframe,maxframe,animdir;
    int framecount,framedelay;
    int startFrame; //Used to manipulate where the animation will start (Allows for multiple animations in one sheet)
}SPRITE;

//variables
BITMAP *back;
BITMAP *temp;
BITMAP *sprite_images[10][20];//This limits max frames
SPRITE *player[4];//player sprites
SPRITE *hotstuff[4];//player projectiles
SPRITE *lame[4];//enemy projectiles
SPRITE *enemies[30];//enemies
BITMAP *buffer;
int n, i, f, gameon, fire, start, enemyFire;

//timer variables
volatile int counter;
volatile int ticks;
volatile int framerate;
volatile int resting, rested;
