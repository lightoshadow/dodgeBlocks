#include <gb/gb.h>
#include <stdbool.h>
#include "ballSprite.h"
#include "textTiles.h"

#define MAXDIST 80

uint8_t curInput=0;
	
uint8_t ballX = 80;
uint8_t ballY = 72;
	
int8_t velX = 0;
int8_t velY = 0;
	
uint8_t startWallX = 44;
uint8_t startWallY = 100;

int8_t velBlocks = -1;
uint8_t blocksY = 136;
	
uint8_t distance = 0;

bool alive = true;

uint8_t score = 0;
uint8_t scoreDigits[] = {1,1,1};

uint8_t RandomNumber(uint8_t min, uint8_t max){
    unsigned char *ptr_div_reg = 0xFF04;
    return min+(*(ptr_div_reg) % (max-min));
}

void cleanSprites(){
	OAM_item_t * itm;
    for(uint8_t i=1;i<40;i++){
        itm = &shadow_OAM[i];
        if(itm->y > 200)
			itm->y = 0;
    }
}

void removeSprites(){
	OAM_item_t * itm;
    for(uint8_t i=1;i<40;i++){
        itm = &shadow_OAM[i];
		itm->y = 0;
    }
}

uint8_t GetAvailableSprite(){
	OAM_item_t * itm;
    for(uint8_t i=1;i<40;i++){
        itm = &shadow_OAM[i];
        if(itm->y==0)
			return i;
    }
    return 0;
}

void drawBlocks(uint8_t startX, uint8_t startY){
	uint8_t spriteId;
	uint8_t openCells = RandomNumber(1,8);
	for (uint8_t i = 0; i < 10; i++){
		spriteId = GetAvailableSprite();
		if(spriteId != 0 && (i != openCells && i != openCells+1)){
			set_sprite_tile(spriteId,1);
			move_sprite(spriteId, (startX + 4)+ (i*8), startY + 12);
		}
	}
} 

void updateScore(uint8_t score){
	scoreDigits[2] = (score%10)+1;
	scoreDigits[1] = ((score/10)%10)+1;
	scoreDigits[0] = ((score/100)%10)+1;
    set_bkg_tiles(1, 2, 3, 1, scoreDigits);
}

void gameUpdate(){
	curInput = joypad();

	if (curInput & J_RIGHT){
		velX = 1;
	}else if (curInput & J_LEFT){
		velX = -1;
	}else{
		velX = 0;
	}

	if(ballX > 120){
		ballX = 44;
	}else if(ballX < 44){
		ballX = 120;
	}

	ballX += velX;
	move_sprite(0,ballX + 4,ballY + 12);
		
	if(distance == 0){
		drawBlocks(44,blocksY);
		distance = MAXDIST;
	}else{
		distance--;
	}
	
	OAM_item_t * itm;
	bool scoreUpdated = false;
	for(uint8_t i = 1; i < 40; i++){
		scroll_sprite(i,0,velBlocks);
       	itm = &shadow_OAM[i];

    	// Check if sprite is active (has a valid position)
    	if(itm->y == 0) continue;
    	
    	// Adjust for the +12 offset you use when positioning sprites
    	uint8_t spriteWorldY = itm->y - 16;
    	uint8_t spriteWorldX = itm->x - 8;
    	
    	// Check for overlap instead of exact positioning
    	if(ballY >= spriteWorldY && ballY <= spriteWorldY + 8){
       		if(ballX >= spriteWorldX && ballX <= spriteWorldX + 8){
        		alive = false;

				NR10_REG=0X69;
				NR11_REG=0X90;
				NR12_REG=0X43;
				NR13_REG=0X4A;
				NR14_REG=0X86;
        		break; // No need to check other sprites once collision is found
    		}
		}

		if(spriteWorldY == ballY - 12 && !scoreUpdated) {
			score++;
			scoreUpdated = true;

			NR21_REG=0X84;
			NR22_REG=0X84;
			NR23_REG=0X08;
			NR24_REG=0X87;
		}
	}
	updateScore(score);
	cleanSprites();
}

void main(void)
{
    DISPLAY_ON;
	SHOW_BKG;
	
	NR52_REG = 0x80;
	NR50_REG = 0x77;
	NR51_REG = 0xFF;	
	
    set_bkg_data(0, 20, textTiles);
	set_bkg_tiles(0,0,20,18,tilesetBackground);
    
	SHOW_SPRITES;
	
	set_sprite_data(0, 2, ballSprite);
    set_sprite_tile(0, 0); 
	
	// Loop forever
    while(1) {

		if(alive == true){
			gameUpdate();
		}else{
			curInput = joypad();
			if(curInput & J_START){
				removeSprites();
				alive = true;
				score = 0;
			}
		}
		// Done processing, yield CPU and wait for start of next frame
        wait_vbl_done();
    }
}
