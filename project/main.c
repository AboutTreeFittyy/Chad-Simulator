#include "main.h"

//Turns on an animation specified for the given sprite
void playAnim(SPRITE *spr, int start, int endFrame){
    //make sure animation not already in progress
    if(spr->startFrame != start ||  spr->animdir ==  0){
		spr->startFrame = start;
		spr->curframe = start;
	    spr->maxframe = endFrame;
	    //spr->framedelay = 5;//might be useful later
	    spr->animdir = 1;	
	}	
}

void rest1(void){
	resting++;
}

void updatesprite(SPRITE *spr){
    //update x position
    if (++spr->xcount > spr->xdelay){
        spr->xcount = 0;
        spr->x += spr->xspeed;
    }
    //update y position
    if (++spr->ycount > spr->ydelay){
        spr->ycount = 0;
        spr->y += spr->yspeed;
    }
    //update frame based on animdir
    if (++spr->framecount > spr->framedelay){
        spr->framecount = 0;
        if (spr->animdir == -1){
            if (--spr->curframe < spr->startFrame){
                spr->curframe = spr->maxframe;
            }
        }
        else if (spr->animdir == 1){
            if (++spr->curframe > spr->maxframe){
                spr->curframe = spr->startFrame;
            }
        }
    }
}

void warpsprite(SPRITE *spr){
    //simple screen warping behavior
    //Allegro takes care of clipping
    if (spr->x < 0 - spr->width){
        spr->x = SCREEN_W;
    }
    else if (spr->x > SCREEN_W){
        spr->x = 0 - spr->width;
    }
    if (spr->y < 0){
        spr->y = SCREEN_H - spr->height-1;
    }
    else if (spr->y > SCREEN_H - spr->height){
        spr->y = 0;
    }
}

//reuse our friendly tile grabber from chapter 9
BITMAP *grabframe(BITMAP *source, int width, int height,int startx, int starty, int columns, int frame){
	BITMAP *temp = create_bitmap(width,height);
    int x = startx + (frame % columns) * width;
    int y = starty + (frame / columns) * height;
    blit(source,temp,x,y,0,0,width,height);//Crashes unless bitdepth 24 is used DO NOT USE 32 BIT DEPTH
    return temp;
}

int inside(int x,int y,int left,int top,int right,int bottom) {
	if (x > left && x < right && y > top && y < bottom){
		return 1;
	}
	else{
		return 0;
	}
}

int collided(SPRITE *first, SPRITE *second, int border) {
	//get width/height of both sprites 
	int width1 = first->x + first->width; 
	int height1 = first->y + first->height;
	int width2 = second->x + second->width; int height2 = second->y + second->height;
	//see if corners of first are inside second boundary 
	if (inside(first->x, first->y, second->x + border, second->y + border, width2 - border, height2 - border)){
		return 1;
	}
	if (inside(first->x, height1, second->x + border, second->y + border, width2 - border, height2 - border)){
		return 1;
	}
	if (inside(width1, first->y, second->x + border, second->y + border, width2 - border, height2 - border)){ 
		return 1;
	}
	if (inside(width1, height1, second->x + border, second->y + border, width2 - border, height2 - border)){
		return 1;
	}
	//no collisions? r
	return 0;
}

//Check to see if projectile has collided with end of screen or sprite
void checkPlayerProjectile(){
	for (n=0; n<NUMENEMIES; n++) {
		//check for collision and that the enemy is still alive, don't want to shoot ghosts
		if (collided(hotstuff[0], enemies[n], 20) && enemies[n]->alive && n > 9) {
			fire=0;//prevents drawing the sprite after hit, which would get it stuck on screen
			enemies[n]->alive = 0;//make enemy dead
		}else if (collided(hotstuff[0], enemies[n], 0) && enemies[n]->alive && n <= 9) {
			fire=0;//prevents drawing the sprite after hit, which would get it stuck on screen
			enemies[n]->alive = 0;//make enemy dead
		}//previous else if statements change the hitboxes so they are more accurate
	}
}

void checkEnemyProjectile(){
	for (n=0; n<NUMPLAYERSPRITES; n++) {
		//check for collision and that the enemy is still alive, don't want to shoot ghosts
		if (collided(lame[0], player[n], 0)) {
			enemyFire=0;//prevents drawing the sprite after hit, which would get it stuck on screen
			player[n]->alive = 0;//make enemy dead
		}
	}
}

//Tries to make an enemy fire, calls itself recursively if enemy given is dead
void enemyFired(int enemyNum){
	if(enemies[enemyNum]->alive){
		lame[0]->x=enemies[enemyNum]->x;
		lame[0]->y=enemies[enemyNum]->y;
		enemyFire=1;
	}
	else{
		enemyFired(rand()%(NUMENEMIES-1));
	}
}

//Loads the sprites into their struct from their bitmap files
void loadsprites(void){   
	//load fat sprite
    temp = load_bitmap("sprites/WALK_RIGHT.bmp", NULL);
    for (n=0; n<9; n++){
		sprite_images[0][n] = grabframe(temp,36,64,0,0,9,n);
    }
    destroy_bitmap(temp);
	for(n = 0; n < NUMENEMIES; n++){		
		//initialize the fat sprites
	    enemies[n] = malloc(sizeof(SPRITE));
	    enemies[n]->x = (n * 105);
	    enemies[n]->y = 0;
	    enemies[n]->width = sprite_images[0][0]->w;
	    enemies[n]->height = sprite_images[0][0]->h;
	    enemies[n]->xdelay = 1;
	    enemies[n]->ydelay = 0;
	    enemies[n]->xcount = 0;
	    enemies[n]->ycount = 0;
	    enemies[n]->xspeed = 2;
	    enemies[n]->yspeed = 0;
	    enemies[n]->curframe = 0;
	    enemies[n]->maxframe = 8;
	    enemies[n]->framecount = 0;
	    enemies[n]->framedelay = 5;
	    enemies[n]->animdir = 1;
	    enemies[n]->startFrame = 0;
	    enemies[n]->alive = 1;
	}
    //load obese cartwheel sprite
    temp = load_bitmap("sprites/CW_RIGHT.bmp", NULL);
    for (n=0; n<20; n++)
        sprite_images[1][n] = grabframe(temp,88,85,0,0,4,n);
    destroy_bitmap(temp);
	for(n = 10; n < 20; n++){		
		//initialize the fat CW sprites
	    enemies[n] = malloc(sizeof(SPRITE));
	    enemies[n]->x = ((n-10)* 105);
	    enemies[n]->y = 100;
	    enemies[n]->width = sprite_images[1][0]->w;
	    enemies[n]->height = sprite_images[1][0]->h;
	    enemies[n]->xdelay = 1;
	    enemies[n]->ydelay = 0;
	    enemies[n]->xcount = 0;
	    enemies[n]->ycount = 0;
	    enemies[n]->xspeed = 2;
	    enemies[n]->yspeed = 0;
	    enemies[n]->curframe = 0;
	    enemies[n]->maxframe = 19;
	    enemies[n]->framecount = 0;
	    enemies[n]->framedelay = 5;
	    enemies[n]->animdir = 1;
	    enemies[n]->startFrame = 0;
	    enemies[n]->alive = 1;
	}
    //load vlad sprite
    temp = load_bitmap("sprites/vlad.bmp", NULL);
    for (n=0; n<8; n++)
        sprite_images[2][n] = grabframe(temp,64,96,0,0,4,n);
    destroy_bitmap(temp);
	for(n = 20; n < 30; n++){		
		//initialize the vlad sprites
	    enemies[n] = malloc(sizeof(SPRITE));
	    enemies[n]->x = 100 + ((n-20)* 105);
	    enemies[n]->y = 200;
	    enemies[n]->width = sprite_images[2][0]->w;
	    enemies[n]->height = sprite_images[2][0]->h;
	    enemies[n]->xdelay = 1;
	    enemies[n]->ydelay = 0;
	    enemies[n]->xcount = 0;
	    enemies[n]->ycount = 0;
	    enemies[n]->xspeed = 2;
	    enemies[n]->yspeed = 0;
	    enemies[n]->curframe = 4;
	    enemies[n]->maxframe = 7;
	    enemies[n]->framecount = 0;
	    enemies[n]->framedelay = 5;
	    enemies[n]->animdir = 1;
	    enemies[n]->startFrame = 4;
	    enemies[n]->alive = 1;
	}
	//load chad/player sprites
    temp = load_bitmap("sprites/chadsprite.bmp", NULL);
    for (n=0; n<16; n++)
        sprite_images[3][n] = grabframe(temp,64,96,0,0,4,n);
    destroy_bitmap(temp);
    //initialize the chad sprite
    player[0] = malloc(sizeof(SPRITE));
    player[0]->x = 550;
    player[0]->y = 800;
    player[0]->width = sprite_images[3][0]->w;
    player[0]->height = sprite_images[3][0]->h;
    player[0]->xdelay = 6;
    player[0]->ydelay = 0;
    player[0]->xcount = 0;
    player[0]->ycount = 0;
    player[0]->xspeed = 0;
    player[0]->yspeed = 0;
    player[0]->curframe = 0;
    player[0]->maxframe = 15;
    player[0]->framecount = 0;
    player[0]->framedelay = 5;
    player[0]->animdir = 0;
    player[0]->startFrame = 0;
    player[0]->alive = 1;    
	//load chad/player sprites
    temp = load_bitmap("sprites/hotstuff.bmp", NULL);
    sprite_images[4][0] = grabframe(temp,20,71,0,0,1,0);
    destroy_bitmap(temp);
    //initialize the chad projectile
    hotstuff[0] = malloc(sizeof(SPRITE));
    hotstuff[0]->x = player[0]->x;
    hotstuff[0]->y = player[0]->y;
    hotstuff[0]->width = sprite_images[4][0]->w;
    hotstuff[0]->height = sprite_images[4][0]->h;
    hotstuff[0]->xdelay = 6;
    hotstuff[0]->ydelay = 0;
    hotstuff[0]->xcount = 0;
    hotstuff[0]->ycount = 0;
    hotstuff[0]->xspeed = 0;
    hotstuff[0]->yspeed = -5;
    hotstuff[0]->curframe = 0;
    hotstuff[0]->maxframe = 1;
    hotstuff[0]->framecount = 0;
    hotstuff[0]->framedelay = 5;
    hotstuff[0]->animdir = 0;
    hotstuff[0]->startFrame = 0;
    hotstuff[0]->alive = 1;    
    //load chad/player sprites
    temp = load_bitmap("sprites/lame.bmp", NULL);
    sprite_images[5][0] = grabframe(temp,33,67,0,0,1,0);
    destroy_bitmap(temp);
    //initialize the lame projectile
    lame[0] = malloc(sizeof(SPRITE));
    lame[0]->x = 0;
    lame[0]->y = 0;
    lame[0]->width = sprite_images[5][0]->w;
    lame[0]->height = sprite_images[5][0]->h;
    lame[0]->xdelay = 6;
    lame[0]->ydelay = 0;
    lame[0]->xcount = 0;
    lame[0]->ycount = 0;
    lame[0]->xspeed = 0;
    lame[0]->yspeed = 5;
    lame[0]->curframe = 0;
    lame[0]->maxframe = 1;
    lame[0]->framecount = 0;
    lame[0]->framedelay = 5;
    lame[0]->animdir = 0;
    lame[0]->startFrame = 0;
    lame[0]->alive = 1;
}

void checkFire(){
	//check if an enemy should fire
	if (clock() > start + 4000){
        blit(back, buffer, lame[0]->x, lame[0]->y, lame[0]->x, lame[0]->y, lame[0]->width, lame[0]->height);
		updatesprite(lame[0]);
        warpsprite(lame[0]);
		start = clock();
        enemyFired(rand()%(NUMENEMIES-1));
	}//Check if player firing
	if(fire){
		blit(back, buffer, hotstuff[0]->x, hotstuff[0]->y, hotstuff[0]->x, hotstuff[0]->y, hotstuff[0]->width, hotstuff[0]->height);
		updatesprite(hotstuff[0]);
        warpsprite(hotstuff[0]);
        checkPlayerProjectile();
        if(hotstuff[0]->y < 5 || !fire){//hit top, reset
        	fire=0;
		}else{//dont draw unless still moving
			draw_sprite(buffer, sprite_images[4][hotstuff[0]->curframe], hotstuff[0]->x, hotstuff[0]->y);
		}            
	}
	//Check if enemy firing
	if(enemyFire){
		blit(back, buffer, lame[0]->x, lame[0]->y, lame[0]->x, lame[0]->y, lame[0]->width, lame[0]->height);
		updatesprite(lame[0]);
        warpsprite(lame[0]);
        checkEnemyProjectile();
        if(lame[0]->y > 900 || !enemyFire){//hit bottom, reset
        	enemyFire=0;
		}else{//dont draw unless still moving
			draw_sprite(buffer, sprite_images[5][lame[0]->curframe], lame[0]->x, lame[0]->y);
		}            
	}
}

void updateGame(){
	//restore the background from regular sprites
    for (n=0; n<NUMPLAYERSPRITES; n++){
    	blit(back, buffer, player[n]->x, player[n]->y, player[n]->x, player[n]->y, player[n]->width, player[n]->height);
    }
    //restore the background from enemy sprites
    for (n=0; n<NUMENEMIES; n++){
    	blit(back, buffer, enemies[n]->x, enemies[n]->y, enemies[n]->x, enemies[n]->y, enemies[n]->width, enemies[n]->height);
    }
    //update the sprites
    for (n=0; n<NUMPLAYERSPRITES; n++){
        updatesprite(player[n]);
        warpsprite(player[n]);
        draw_sprite(buffer, sprite_images[3][player[n]->curframe], player[n]->x, player[n]->y);
    }
	//update the enemy sprites
    for(i = 0; i < 3; i++){
    	for (n=0; n<10; n++){
			f=(i*10)+n;
			if(enemies[f]->alive){
				updatesprite(enemies[f]);
				if(enemies[f]->x>1090){
					enemies[f]->x=0;
					enemies[f]->y=enemies[f]->y+100;
				}
            	warpsprite(enemies[f]);
            	draw_sprite(buffer, sprite_images[i][enemies[f]->curframe], enemies[f]->x, enemies[f]->y);
			}				
        }
	}
}

//Deallocates memory used in game
int destroy(){
	//remove objects from memory
    destroy_bitmap(back);
    destroy_bitmap(buffer);
	for(i=0;i<3;i++){
        for (f=0; f<enemies[i*10]->maxframe+1; f++){
			destroy_bitmap(sprite_images[i][f]);
		}
	}
	for(i=0;i<NUMENEMIES;i++){
		free(enemies[n]);
	}
    for (n=0; n<1; n++){
        for (f=0; f<player[n]->maxframe+1; f++){
			destroy_bitmap(sprite_images[3][f]);
		}
        free(player[n]);
    }    
    return 0;
}

//Exits game when ESC key pressed
void getinput(){
    //quit game
    if (key[KEY_ESC]){
        if(gameon){//on title screen so exit the program
        	quitgame = 1;
        	printf(".GAME-QUIT");
		}else{
			gameon = 1;//in a game so return to title screen by breaking out of game loop
			cont = 1;
		}		
    }//only need to use functions below when in game, this seperates mainscreen from game screen input
    if(!gameon){
	    if (key[KEY_A]){//move left
	        player[0]->xspeed = -CHADSPEED;
	        playAnim(player[0], 0, 3);
	    }//Move right
	    else if (key[KEY_D]){
	        player[0]->xspeed = CHADSPEED;
	        playAnim(player[0], 4, 7);
	    }//Flex/Fire
	    else if (key[KEY_W] && !fire){
	        player[0]->xspeed = 0;
	        playAnim(player[0], 8, 15);
	        fire=1;
	        hotstuff[0]->x = player[0]->x;
	    	hotstuff[0]->y = player[0]->y;
	    }//Reserved in case more needed
	    else if (key[KEY_S]){
	        
	    }else if (key[KEY_P] || key[KEY_H]){//checks for P press
	        if(key_shifts & KB_CTRL_FLAG){//checks if ctrl is held too
				if(paused){
		        	paused = 0; //UnPause
				}else{
					paused = 1; //Pause
				}
			}
	    }else{
	    	player[0]->animdir = 0; //stop playing animation
			player[0]->xspeed = 0;//stop moving, nothing pressed
		}
	}
	else{//Title screen so check for SPACE bar to begin
		if (key[KEY_SPACE]){//Start a new game
	        if(!cont){
				gameon = 0, paused = 0, fire = 0, enemyFire = 0;//Initialize the games global variables for a new session	        
			    back = load_bitmap("bgbeach.bmp", NULL);//load and draw the blocks
			    blit(back,buffer,0,0,0,0,back->w,back->h);		    
			    loadsprites();//load and set up sprites
			    printf(".STARTING-GAME");
		        runGame(); //Run the game,
		        printf(".GAME-INSTANCE-OVER");
		    }else{//advance to next screen
		    	cont = 0;
		    	acquire_screen();
				blit(title,buffer,0,0,0,0,title->w,title->h);
				blit(buffer,screen,0,0,0,0,SCREEN_W-1,SCREEN_H-1);
				release_screen();
			}
	    }
	}
}

void runGame(){
	//game loop
    while (!gameon){
        //check for escape to quit & input to move Chad
        if (keypressed()){
        	getinput();
		}		
		if(!paused){		
			updateGame();			
			if(!player[0]->alive){//check if player has died
				gameon=1;
			}
	        checkFire();//Operates and maintains the firing function for player and enemy projectiles
		}
        rest_callback(15, rest1);//This controls the speed of the game (frame rate)        
        //update the screen
        acquire_screen();
        blit(buffer,screen,0,0,0,0,SCREEN_W-1,SCREEN_H-1);
        release_screen();
    }
	acquire_screen();//Now bring back the title screen
	blit(intro,buffer,0,0,0,0,title->w,title->h);
	blit(buffer,screen,0,0,0,0,SCREEN_W-1,SCREEN_H-1);
	release_screen();		
	printf(".RETURN-TITLE");
}

/*
How to implement multiple bullets:
determine firing speed wished
determine time it takes for one bullet to travel entire screen
figure out how many bullets could be fired at that speed before one hit the top
make that (possibly plus one) bullet sprites with a timer to spread them out and a counter to keep track of the next sprite to use in order
*/

//Main function, initializes program environment, runs game routine until quit (destroy function called elsewhere for most cleanup)
int main(void){
    //initialize
    allegro_init();
    set_color_depth(16);
    set_gfx_mode(MODE, WIDTH, HEIGHT, 0, 0);
    srand(time(NULL));
    install_keyboard();
    install_timer();
    int ret = set_gfx_mode(MODE, WIDTH, HEIGHT, 0, 0);
    if (ret != 0) {
        allegro_message(allegro_error);
        return;
    }
	quitgame = 0, cont = 1, gameon = 1;
    //create double buffer
    buffer = create_bitmap(SCREEN_W,SCREEN_H);
	//Display title screen
	intro = load_bitmap("sprites/intro_screen.bmp", NULL);//load first title image
	title = load_bitmap("sprites/title_screen.bmp", NULL);//load second title image
	acquire_screen();
	blit(intro,buffer,0,0,0,0,title->w,title->h);
	blit(buffer,screen,0,0,0,0,SCREEN_W-1,SCREEN_H-1);
	release_screen();
	printf(".TITLE");	  
	while(!quitgame){
		//Check for input to either quit completely or to start a game
		if (keypressed()){
        	getinput();
		}
		rest(50);
	} 
	printf(".DESTROYING");
	destroy();
	printf(".ENDED.");
    allegro_exit();
	return 0;        
}
END_OF_MAIN()

