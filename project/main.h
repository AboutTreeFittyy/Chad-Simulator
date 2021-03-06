/*C file "main.h"
**Project: Chad Simulator
**Author: mathew Boland
**Version: 1.0
**Last Edited: 11/07/2020
*/

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
#define ENEMYPOINTS 100
#define BLACK makecol(0,0,0)
#define WHITE makecol(255,255,255)
#define TEXTCOLOUR makecol(0,0,51)

//define the structure of the sprite
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

//global variables
BITMAP *back;
BITMAP *intro;
BITMAP *title;
BITMAP *victory;
BITMAP *pause;
BITMAP *tp;
BITMAP *temp;
BITMAP *sprite_images[10][20];//This limits max frames
SPRITE *player[4];//player sprites (didn't need to be an array but not a big deal)
SPRITE *hotstuff[4];//player projectiles
SPRITE *lame[4];//enemy projectiles
SPRITE *enemies[30];//enemies
BITMAP *buffer;
char * vicnum;
char * vicpath;
SAMPLE *sounds[5];
int volume = 128;
int pan = 128;
int pitch = 1000;
int c, n, i, f, j, music, init, gameover, quitgame, gameon, cont, paused, score, starttime, endtime, cooldown, start, enemyProjectiles;
//timer variables
volatile int resting, rested;

void runGame();
void loseGame();
