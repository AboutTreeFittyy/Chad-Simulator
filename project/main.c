#include "main.h"
/*C file "main.c"
**Project: Chad Simulator
**Author: mathew Boland
**Version: 1.0
**Last Edited: 11/07/2020
*/
//test function to see all game states
void printGameImage(){
	printf("\nGAMEOVER: %d\n", gameover);
	printf("GAMEON: %d\n", gameon);
	printf("PAUSED: %d\n", paused);
	printf("QUITGAME: %d\n", quitgame);
	printf("CONT: %d\n", cont);
	//printf(": %d\n", );
}

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

//wait for callback
void rest1(void){
	resting++;
}

//updates the properties of the specified sprite
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

//simple warping of given sprite
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

//grabs the frame of a bitmap
BITMAP *grabframe(BITMAP *source, int width, int height,int startx, int starty, int columns, int frame){
	BITMAP *temp = create_bitmap(width,height);
    int x = startx + (frame % columns) * width;
    int y = starty + (frame / columns) * height;
    blit(source,temp,x,y,0,0,width,height);//Crashes unless bitdepth 24 is used DO NOT USE 32 BIT DEPTH
    return temp;
}

//checks if coordinates are inside a given hitbox, return 1 if inside
int inside(int x,int y,int left,int top,int right,int bottom) {
	if (x > left && x < right && y > top && y < bottom){
		return 1;
	}
	else{
		return 0;
	}
}

//checks two sprites for collisions
int collided(SPRITE *first, SPRITE *second, int border) {
	//get width/height of both sprites 
	int width1 = first->x + first->width; 
	int height1 = first->y + first->height;
	int width2 = second->x + second->width; 
	int height2 = second->y + second->height;
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
	return 0;//no collision return 0
}

//Check to see if projectile has collided with end of screen or sprite
void checkPlayerProjectile(){
	for (n=0; n<NUMENEMIES; n++) {
		for(c = 0; c < 4; c++){//check for collision and that the enemy is still alive, don't want to shoot ghosts
			//printf("INDEX: %d", i);
			if (collided(hotstuff[c], enemies[n], 20) && enemies[n]->alive && n > 9) {
				enemies[n]->alive = 0;//make enemy dead
				blit(back, buffer, hotstuff[c]->x, hotstuff[c]->y, hotstuff[c]->x, hotstuff[c]->y, hotstuff[c]->width, hotstuff[c]->height);
				hotstuff[c]->x = -200;//move off screen
		    	hotstuff[c]->y = -200;
		    	hotstuff[c]->yspeed = 0;//stop it
		    	hotstuff[c]->framedelay = 0;//reset
		    	score += ENEMYPOINTS;
		    	play_sample(sounds[4], volume, pan, pitch, FALSE);//play enemy hit sound effect
			}else if (collided(hotstuff[c], enemies[n], 0) && enemies[n]->alive && n <= 9) {
				enemies[n]->alive = 0;//make enemy dead
				blit(back, buffer, hotstuff[c]->x, hotstuff[c]->y, hotstuff[c]->x, hotstuff[c]->y, hotstuff[c]->width, hotstuff[c]->height);
				hotstuff[c]->x = -200;//move off screen
		    	hotstuff[c]->y = -200;
		    	hotstuff[c]->yspeed = 0;//stop it
		    	hotstuff[c]->framedelay = 0;//reset
		    	score += ENEMYPOINTS;
		    	play_sample(sounds[4], volume, pan, pitch, FALSE);//play enemy hit sound effect
			}//previous else if statements change the hitboxes so they are more accurate
			//The following is used to detect when an enemy makes it to the bottom
			if(enemies[n]->y > 650){
				player[0]->alive = 0;//kill player
				enemies[n]->y=0;//move enemy out of lose area
				enemies[n]->xspeed=0;//stop enemy moving, this is just so it doesn't get triggered twice by accident
				gameover = 1;
				loseGame();
			}
		}
	}
}

//Checks the enemies projectiles to see if any collided with player
void checkEnemyProjectile(){
	for(i = 0; i < enemyProjectiles; i++){//check for collision with player
		if (collided(lame[i], player[0], 0)) {
			lame[i]->y=0;//move away from player so it doesnt hit this twice by accident, game already lost
			lame[i]->yspeed=0;//stop projectile moving
			player[0]->alive = 0;//make player dead	
			gameover = 1;
			loseGame();		
		}
	}
}

//Tries to make an enemy fire, calls itself recursively if enemy given is dead
void enemyFired(int enemyNum, int index){
	if(enemies[enemyNum]->alive){
		//printf("enemy: %d -- dropping: %d - X\n", enemyNum, enemies[enemyNum]->x+(enemies[enemyNum]->width/2));//test code for enemy fire, checks position and enemy found to see if randomness works
		lame[index]->x=enemies[enemyNum]->x+(enemies[enemyNum]->width/2);
		lame[index]->y=enemies[enemyNum]->y;
		lame[index]->yspeed = 5;
	}
	else{
		enemyFired(rand()%(NUMENEMIES-1), index);
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
	//load hotstuff sprites
    temp = load_bitmap("sprites/hotstuff.bmp", NULL);
    sprite_images[4][0] = grabframe(temp,20,71,0,0,1,0);
    destroy_bitmap(temp);
    for(i = 0; i < 4; i++){//initialize the chad projectile
	    hotstuff[i] = malloc(sizeof(SPRITE));
	    hotstuff[i]->x = -200;
	    hotstuff[i]->y = -200;
	    hotstuff[i]->width = sprite_images[4][0]->w;
	    hotstuff[i]->height = sprite_images[4][0]->h;
	    hotstuff[i]->xdelay = 6;
	    hotstuff[i]->ydelay = 0;
	    hotstuff[i]->xcount = 0;
	    hotstuff[i]->ycount = 0;
	    hotstuff[i]->xspeed = 0;
	    hotstuff[i]->yspeed = 0;
	    hotstuff[i]->curframe = 0;
	    hotstuff[i]->maxframe = 1;
	    hotstuff[i]->framecount = 0;
	    hotstuff[i]->framedelay = 5;
	    hotstuff[i]->animdir = 0;
	    hotstuff[i]->startFrame = 0;
	    hotstuff[i]->alive = 1;    
	}
    //load lame sprites
    temp = load_bitmap("sprites/lame.bmp", NULL);
    sprite_images[5][0] = grabframe(temp,33,67,0,0,1,0);
    destroy_bitmap(temp);
    for(i=0; i<enemyProjectiles; i++){
	    //initialize the lame projectile
	    lame[i] = malloc(sizeof(SPRITE));
	    if(lame[i] == NULL){
	    	printf("MEMORY_FAILURE");
	    	exit(1);
		}
	    lame[i]->x = -200;
	    lame[i]->y = -200;
	    lame[i]->width = sprite_images[5][0]->w;
	    lame[i]->height = sprite_images[5][0]->h;
	    lame[i]->xdelay = 6;
	    lame[i]->ydelay = 0;
	    lame[i]->xcount = 0;
	    lame[i]->ycount = 0;
	    lame[i]->xspeed = 0;
	    lame[i]->yspeed = 0;
	    
	    lame[i]->curframe = 0;
	    lame[i]->maxframe = 1;
	    lame[i]->framecount = 0;
	    lame[i]->framedelay = 5;
	    lame[i]->animdir = 0;
	    lame[i]->startFrame = 0;
	    lame[i]->alive = 1;
	}
}

//handles the projectiles of the player and enemy
void checkFire(){
	//check if an enemy should fire
	if (clock() > start + 850){//be careful with this timing (850 for 4), its synced with the number of projectiles to make the random aspect work
        for(i = 0; i < enemyProjectiles; i++){        	
        	if(lame[i]->yspeed == 0){//only fire what isnt already in use
        		blit(back, buffer, lame[i]->x, lame[i]->y, lame[i]->x, lame[i]->y, lame[i]->width, lame[i]->height);
				updatesprite(lame[i]);
		        warpsprite(lame[i]);
				srand(time(NULL));
				f = rand()%(NUMENEMIES-1);				
		        enemyFired(f, i);
        		i = enemyProjectiles;//make sure this loop dies after firing
			}	
		}	
		start = clock();	
	}
	for(j = 0; j < enemyProjectiles; j++){//Check/update projectiles currently fired
		if(lame[j]->yspeed != 0){
			blit(back, buffer, lame[j]->x, lame[j]->y, lame[j]->x, lame[j]->y, lame[j]->width, lame[j]->height);
			updatesprite(lame[j]);
	        warpsprite(lame[j]);
	        checkEnemyProjectile();
	        if(lame[j]->y >= 830){//hit bottom, reset
				blit(back, buffer, lame[j]->x, lame[j]->y, lame[j]->x, lame[j]->y, lame[j]->width, lame[j]->height);
				lame[j]->x = -200;//move off screen
		    	lame[j]->y = -200;
		    	lame[j]->yspeed = 0;//stop it
			}else{//dont draw unless still moving
				draw_sprite(buffer, sprite_images[5][lame[j]->curframe], lame[j]->x, lame[j]->y);
			}           
		}
	}
	//Check if need to delete a projectile
	for(i = 0; i < 4; i++){//make sure max number of projectiles not fired already on screen
		if(hotstuff[i]->yspeed != 0 && clock() >= hotstuff[i]->framedelay + 4000){
	        hotstuff[i]->x = -200;//move off screen
	    	hotstuff[i]->y = -200;
	    	hotstuff[i]->yspeed = 0;//stop it
	    	hotstuff[i]->framedelay = 0;//reset
		}
	}
	//update player projectiles    
    for(i = 0; i < 4; i++){
    	if(hotstuff[i]->yspeed != 0){//only need to draw ones that are currently fired
			blit(back, buffer, hotstuff[i]->x, hotstuff[i]->y, hotstuff[i]->x, hotstuff[i]->y, hotstuff[i]->width, hotstuff[i]->height);
			updatesprite(hotstuff[i]);
	        warpsprite(hotstuff[i]);
	        draw_sprite(buffer, sprite_images[4][hotstuff[i]->curframe], hotstuff[i]->x, hotstuff[i]->y);
	    }
	    if(hotstuff[i]->y < 5){//delete projectile if it hits top
	    	blit(back, buffer, hotstuff[i]->x, hotstuff[i]->y, hotstuff[i]->x, hotstuff[i]->y, hotstuff[i]->width, hotstuff[i]->height);
			hotstuff[i]->x = -200;//move off screen
	    	hotstuff[i]->y = -200;
	    	hotstuff[i]->yspeed = 0;//stop it
	    	hotstuff[i]->framedelay = 0;//reset
		}
	}
	checkPlayerProjectile();//check for collisions to handle
}

//runs the updates needed for the game
void updateGame(){
	//Update score
	blit(back, buffer, 0, 0, 0, 0, 100, 50);//clear previous time/score
	textprintf_centre_ex(buffer,font,50,20,WHITE,-1,"TIME: %d", (clock()-starttime)/1000);//print time since start in seconds
    textprintf_centre_ex(buffer,font,50,40,WHITE,-1,"SCORE: %d", score);//print current score
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
	//remove bitmap objects from memory (should never be null, so dont need to check)
    destroy_bitmap(back);
    destroy_bitmap(buffer);
    destroy_bitmap(intro);
    destroy_bitmap(title);    
    destroy_bitmap(pause);
    destroy_bitmap(tp);
    //check this for null, not always used
    if(victory){
    	destroy_bitmap(victory);
	}
    //check that these have been initialized with global variable init
    if(init){
    	//destroy all of the enemies
		for(i=0;i<3;i++){
	        for (f=0; f<enemies[i*10]->maxframe+1; f++){
				destroy_bitmap(sprite_images[i][f]);
			}
		}
		//free enemies memory
		for(i=0;i<NUMENEMIES;i++){
			free(enemies[i]);
		}
		//destroy and free player
	    for (n=0; n<1; n++){
	        for (f=0; f<player[n]->maxframe+1; f++){
				destroy_bitmap(sprite_images[3][f]);
			}
	        free(player[n]);
	    }  
	    //destroy projectile sprites
	    for(i=0;i<4;i++){
	    	free(hotstuff[i]);
		}
		for(i=0;i<enemyProjectiles;i++){
	    	free(lame[i]);
		}
	    destroy_bitmap(sprite_images[4][0]);
	    destroy_bitmap(sprite_images[5][0]);
	}      
    return 0;
}

//Exits game when ESC key pressed
void getinput(){
	//these check keys no matter what screen is displayed or part of the game is currently active
    if (key[KEY_ESC]){//quit game
		if(gameon){//on title screen so exit the program
        	quitgame = 1;
        	//printf(".GAME-QUIT");
		}else{
			gameon = 1;//in a game so return to title screen by breaking out of game loop
			cont = 1;
			//printf(".QUIT-TO-TITLE");
			rest(250);//try to prevent double escaping	
		}				
    }else if (key[KEY_M]){//checks for M press
        if(key_shifts & KB_CTRL_FLAG){//checks if ctrl is held too
			if(music){//music is on so turn off
	        	music = 0; //mute
	        	stop_sample(sounds[0]);
			}else{//music off so turn on
				music = 1; //unmute
		    	play_sample(sounds[0], volume, pan, pitch, TRUE);
			}
			rest(250);//add delay so that it doesn't turn music on and off
		}
	}
	//only need to use functions below when in game, this seperates mainscreen from game screen input
    if(!gameon){
	    if (key[KEY_A]){//move left
	        player[0]->xspeed = -CHADSPEED;
	        playAnim(player[0], 0, 3);
	    }//Move right
	    else if (key[KEY_D]){
	        player[0]->xspeed = CHADSPEED;
	        playAnim(player[0], 4, 7);
	    }//Flex/Fire
	    else if (key[KEY_W]){
	        for(i = 0; i < 4; i++){//make sure max number of projectiles not fired already on screen
				if(hotstuff[i]->yspeed == 0 && clock() > cooldown){
					cooldown = clock() + 1000;
					play_sample(sounds[2], volume, pan, pitch-400, FALSE);//play flexsound effect (sounds weird with higher pitch so its decreased)
					player[0]->xspeed = 0;
			        playAnim(player[0], 8, 15);
			        hotstuff[i]->x = player[0]->x+(player[0]->width/2);//center on player
			    	hotstuff[i]->y = player[0]->y;
			    	hotstuff[i]->yspeed = -5;//have it travel up now
			    	hotstuff[i]->framedelay = clock();//unused variable for this, use to track time it was sent
				}
			}
	    }else if (key[KEY_P] || key[KEY_H]){//checks for P/H press
	        if(key_shifts & KB_CTRL_FLAG){//checks if ctrl is held too
				if(paused){
		        	paused = 0; //UnPause
		        	blit(back,buffer,0,0,0,0,back->w,back->h);	//draw the game background	 
				}else{
					paused = 1; //Pause
			    	acquire_screen();
					blit(pause,buffer,-50,-50,0,0,title->w,title->h);
					release_screen();
				}
				rest(500);//add delay so that it doesn't pause and unpause from keyboard input stream (I wish the API had a KEY_UP setting)
			}
	    }else{
	    	player[0]->animdir = 0; //stop playing animation
			player[0]->xspeed = 0;//stop moving, nothing pressed
		}
	}else if(gameover){
		gameover=0;
	}else{//Title screen so check for SPACE bar to begin
		if (key[KEY_SPACE]){//Start a new game
	        if(!cont){
				init = 1, score = 0, starttime = clock(), gameon = 0, gameover = 0, paused = 0;//Initialize the games global variables for a new session	 			    
			    blit(back,buffer,0,0,0,0,back->w,back->h);	//draw the game background	    
			    loadsprites();//load and set up sprites
			    //printf(".STARTING-GAME");
		        runGame(); //Run the game,
		        //printf(".GAME-INSTANCE-OVER");
		    }else{//advance to next screen
		    	cont = 0;
		    	acquire_screen();
				blit(title,buffer,0,0,0,0,title->w,title->h);
				blit(buffer,screen,0,0,0,0,SCREEN_W-1,SCREEN_H-1);
				release_screen();
				rest(250);//add delay so that it doesn't count 2 space presses and skip this screen
			}
	    }
	}
}

//Creates the winning screeen based on the player score/time
void winGame(){
	//printf(".GAMEWON");
	//create winning screen
	endtime = (clock()-starttime)/1000;//get time elapsed	
	//calculate score (maximum score of 3000)
	for(i = 45; i < endtime; i++){//subtract 100 points for every second greater than 45
		score -= 100;
	}
	//set up char array to store reward number
	vicnum = malloc(3);
    vicnum[0] = '\0';
	//check which winning screen to display based on the score
	if(score >= 3000){//max score, display highest reward screen
		strcat(vicnum, "11");
	}else if(score > 2500){
		strcat(vicnum, "10");
	}else if(score > 2250){
		strcat(vicnum, "9");
	}else if(score > 2000){
		strcat(vicnum, "8");
	}else if(score > 1750){
		strcat(vicnum, "7");
	}else if(score > 1500){
		strcat(vicnum, "6");
	}else if(score > 1250){
		strcat(vicnum, "5");
	}else if(score > 1000){
		strcat(vicnum, "4");
	}else if(score > 750){
		strcat(vicnum, "3");
	}else if(score > 500){
		strcat(vicnum, "2");
	}else if(score > 250){
		strcat(vicnum, "1");
	}else{//worst winning 'reward'
		strcat(vicnum, "0");
	}
	vicpath = malloc(25);//alocate memory to the char array/string so that the path for the victory screen can be built
    vicpath[0] = '\0';
    strcat(vicpath, "victory_screens/");    
    strcat(vicpath, vicnum);
    strcat(vicpath, ".bmp");    
	victory = load_bitmap(vicpath, NULL);//load victory image
	blit(victory,screen,0,0,0,0,title->w,title->h);//add image to screen
	blit(tp,buffer,0,0,0,0,title->w,title->h);//add transparent background to buffer, clearing everything
    //add text to the clear buffer so it can be stretched after, depending on score
    switch(atoi(vicnum)){//selection based on score to give a somewhat dynamic message depending on how well player did 
    	case 0:
			textprintf_centre_ex(buffer,font,550,430,TEXTCOLOUR,-1,"You took too long!");
			textprintf_centre_ex(buffer,font,550,440,TEXTCOLOUR,-1,"All of the babes left!");
			textprintf_centre_ex(buffer,font,550,450,TEXTCOLOUR,-1,"TIME PENALTIES: %d", 3000-score);
		    textprintf_centre_ex(buffer,font,550,460,TEXTCOLOUR,-1,"FINAL SCORE: %d", score);
    		break;
    	case 1:
    		textprintf_centre_ex(buffer,font,550,430,TEXTCOLOUR,-1,"It could be worse!");
			textprintf_centre_ex(buffer,font,550,440,TEXTCOLOUR,-1,"There's still 1 babe on the beach!");
			textprintf_centre_ex(buffer,font,550,450,TEXTCOLOUR,-1,"TIME PENALTIES: %d", 3000-score);
		    textprintf_centre_ex(buffer,font,550,460,TEXTCOLOUR,-1,"FINAL SCORE: %d", score);
    		break;
    	case 11:
			textprintf_centre_ex(buffer,font,550,430,TEXTCOLOUR,-1,"WOW! You really are Chad!");
			textprintf_centre_ex(buffer,font,550,440,TEXTCOLOUR,-1,"All of the babes are still here!");
			textprintf_centre_ex(buffer,font,550,450,TEXTCOLOUR,-1,"TIME PENALTIES: %d", 3000-score);
		    textprintf_centre_ex(buffer,font,550,460,TEXTCOLOUR,-1,"FINAL SCORE: %d", score);
    		break;
    	default:
		    textprintf_centre_ex(buffer,font,550,430,TEXTCOLOUR,-1,"Congratulations!");
			textprintf_centre_ex(buffer,font,550,440,TEXTCOLOUR,-1,"There are still %s babes on the beach!", vicnum);
			textprintf_centre_ex(buffer,font,550,450,TEXTCOLOUR,-1,"TIME PENALTIES: %d", 3000-score);
		    textprintf_centre_ex(buffer,font,550,460,TEXTCOLOUR,-1,"FINAL SCORE: %d", score);
	}
    textprintf_centre_ex(screen,font,550,840,TEXTCOLOUR,-1,"PRESS ESC TO RETURN TO TITLE SCREEN");
	stretch_blit(buffer, screen, 380, 425, 330, 50, 50, 400, 1000, 100);//stretch the buffers text to desired size on screen
	release_screen();
	rest(500);//A sound effect for getting final enemy is playing, so delay playing winning one until it ends
	play_sample(sounds[3], volume, pan, pitch, FALSE);//play game win sound effect
	while(gameover){//create victory screeen and end game
		if (keypressed()){
	    	getinput();//wait for spacebar to continue
		}		        	
	}
	free(vicnum);
	free(vicpath);				
}

//creates the losing screen
void loseGame(){
	//printf(".GAMELOST");
	play_sample(sounds[1], volume, pan, pitch, FALSE);//play game lost sound effect	 
	victory = load_bitmap("victory_screens/0.0.bmp", NULL);//load victory image
	blit(victory,screen,0,0,0,0,title->w,title->h);//add image to screen
	blit(tp,buffer,0,0,0,0,title->w,title->h);//add transparent background to buffer, clearing everything
    //add text to the clear buffer so it can be stretched after, depending on score    
    textprintf_centre_ex(buffer,font,550,430,TEXTCOLOUR,-1,"What a loser! You aren't Chad at all!'");
	textprintf_centre_ex(buffer,font,550,440,TEXTCOLOUR,-1,"None of the babes want to be around you!", vicnum);
	textprintf_centre_ex(buffer,font,550,450,TEXTCOLOUR,-1,"TIME PENALTIES: YOU'RE A WASTE OF TIME!'", 3000-score);
    textprintf_centre_ex(buffer,font,550,460,TEXTCOLOUR,-1,"FINAL SCORE: YOU STINK LOSER!");
    textprintf_centre_ex(screen,font,550,840,TEXTCOLOUR,-1,"PRESS ESC TO RETURN TO TITLE SCREEN");
	stretch_blit(buffer, screen, 380, 425, 330, 50, 50, 400, 1000, 100);//stretch the buffers text to desired size on screen
	release_screen();
	while(gameover){//create victory screeen and end game
		if (keypressed()){
	    	getinput();//wait for spacebar to continue
		}		        	
	}
}

//runs the game loop
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
				gameon = 1;				
			}
	        checkFire();//Operates and maintains the firing function for player and enemy projectiles	
			for(i = 0; i < NUMENEMIES; i++){//Check if game won
				if(enemies[i]->alive == 1){
					i = NUMENEMIES; //Found alive enemy so dont waster resources checking the rest
				}else if(i == NUMENEMIES - 1){
					gameover = 1; //i can only equal NUMENEMIES - 1 if all enemies have been scanned and dead
					winGame();
				}
			}				    
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
	//printf(".RETURN-TITLE");
}

//Main function, initializes program environment, runs game routine until quit (destroy function called elsewhere for most cleanup)
int main(void){
    //initialize
    allegro_init();
    set_color_depth(16);
    set_gfx_mode(MODE, WIDTH, HEIGHT, 0, 0);    
    install_keyboard();
    install_timer();
    int ret = set_gfx_mode(MODE, WIDTH, HEIGHT, 0, 0);
    if (ret != 0) {
        allegro_message(allegro_error);
        return;
    }    
    //install sound
    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, "") != 0){
        allegro_message("Error initializing sound system");
        return 1;
    }
    //load sounds
    sounds[0] = load_sample("sounds/house_party.wav");
    sounds[1] = load_sample("sounds/fail_trumpet.wav");
    sounds[2] = load_sample("sounds/male_grunt.wav");
    sounds[3] = load_sample("sounds/male_oh_yeah.wav");
    sounds[4] = load_sample("sounds/fat_sound.wav");
    //play background music
    play_sample(sounds[0], volume, pan, pitch, TRUE);
	music = 1, quitgame = 0, init = 0, cont = 1, gameover = 0, gameon = 1, cooldown = 0, enemyProjectiles = 4;
    //create double buffer
    buffer = create_bitmap(SCREEN_W,SCREEN_H);
	//Display title screen
	intro = load_bitmap("backgrounds/intro_screen.bmp", NULL);//load first title image
	title = load_bitmap("backgrounds/title_screen.bmp", NULL);//load second title image
	back = load_bitmap("backgrounds/bgbeach.bmp", NULL);//Load games background
	tp = load_bitmap("backgrounds/transparent_pink.bmp", NULL);//load transparent background for stretching text
	pause = load_bitmap("backgrounds/pause_screen.bmp", NULL);//Load pause screen
	acquire_screen();
	blit(intro,buffer,0,0,0,0,title->w,title->h);
	blit(buffer,screen,0,0,0,0,SCREEN_W-1,SCREEN_H-1);
	release_screen();
	//printf(".TITLE");	  
	while(!quitgame){
		//Check for input to either quit completely or to start a game
		if (keypressed()){
        	getinput();
		}
		rest(50);
	} 
	//printf(".DESTROYING");
	destroy();
	//printf(".ENDED.");
    allegro_exit();
	return 0;        
}
END_OF_MAIN()

