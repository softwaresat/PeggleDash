// Lab9HMain.cpp
// Runs on MSPM0G3507
// Lab 9 ECE319H
// Your name
// Last Modified: 12/26/2024

#include <cstdint>
#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "GameObject.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "images/images.h"
#include "Ball.h"
#include "Sounds.h"
#include "Peg.h"
#include "Hole.h"
#include "Level.h"
#include "GameState.h"


extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);
#define BUTTON_DOWN  (1 << 0)  // PA15
#define BUTTON_RIGHT (1 << 1)  // PA16
#define BUTTON_LEFT  (1 << 2)  // PA27
#define BUTTON_UP    (1 << 3)  // PA28
#define FIX 8

// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t M=1;


void SeedRandom(uint32_t seed) {
  M = seed;
}

uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

SlidePot Sensor(1500,0); // copy calibration from Lab 7
uint32_t data;
uint32_t input;
Ball* currBall =  new Ball(192);
Hole* movingHole = new Hole();
int8_t levelSelect = 1;
Peg pegs[25];
int8_t pegCount = 0;
int8_t indexAngle;
GameState gameState; // Global game state object to track game progress

const uint16_t BlackCoverSprite[64] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
};


// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes 
    data = Sensor.In();
    input = Switch_In();
    movingHole->moveHole();
    for (int i = 0; i < pegCount; i++) {
      if (currBall->checkCollision(pegs[i].getX(), pegs[i].getY())) {
        currBall->bounce(pegs[i].getX(), pegs[i].getY());
        // Award points when hitting pegs
        gameState.addPoints(10);
        // Play sound when hitting pegs
        Sound_Fastinvader1();
        //indexAngle = currBall->angleToIndex();
        break;
      }
    }
    
    // Check if ball falls into hole
    if (currBall->checkHoleCollision(movingHole->getX(), movingHole->getY())) {
      // Award bonus points for getting ball in hole
      gameState.addPoints(50);
      // Play sound when scoring
      Sound_Killed();
    }
    
    // Check if ball is lost (e.g., off screen)
    if (currBall->isLost()) {
      gameState.useBall();
      // Only reset if we still have balls left
      if (!gameState.isGameOver()) {
        currBall->reset(192); // Reset with default angle
      }
    }
    
    //currBall->reset(indexAngle);
    currBall->moveBall();
    // 1) sample slide pot
    // 2) read input switches
    // 3) move sprites
    // 4) start sounds
    // 5) set semaphore
    // NO LCD OUTPUT IN INTERRUPT SERVICE ROUTINES
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}
uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};
// use main1 to observe special characters
int main1(void){ // main1
    char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB);
  ST7735_FillScreen(0x0000);            // set screen to black
  for(int myPhrase=0; myPhrase<= 2; myPhrase++){
    for(int myL=0; myL<= 3; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Clock_Delay1ms(3000);
  ST7735_FillScreen(0x0000);       // set screen to black
  l = 128;
  while(1){
    Clock_Delay1ms(2000);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }
}

// use main2 to observe graphics
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB);
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom
  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);

  for(uint32_t t=500;t>0;t=t-5){
    SmallFont_OutVertical(t,104,6); // top left
    Clock_Delay1ms(50);              // delay 50 msec
  }
  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString((char *)"GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString((char *)"Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString((char *)"Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  while(1){
  }
}

// use main3 to test switches and LEDs
int main3(void){ // main3
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  while(1){
    ST7735_InitPrintf(INITR_REDTAB);
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  
    input = Switch_In();
    ST7735_SetCursor(1, 1);
    ST7735_OutUDec(input);


  }
}
// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();
  while(1){
    now = Switch_In(); // one of your buttons
    if((last == 0)&&(now == 1)){
      Sound_Shoot(); // call one of your sounds
    }
    if((last == 0)&&(now == 2)){
      Sound_Killed(); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
    last = now;
    Clock_Delay(800000); // delay ~10ms at 80 MHz
  }
}
//hi
// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB);
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  Sensor.Init(); // PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
    // initialize interrupts on TimerG12 at 30 Hz
  TimerG12_Init();
  TimerG12_IntArm(2666667, 2);
  // initialize all data structures
  __enable_irq();

  
  //Prompt user for level select
  // Initialize the game state
  gameState.resetGame();
  gameState.setCurrentLevel(levelSelect);
  
  // Display initial game stats
  ST7735_SetCursor(0, 0);
  ST7735_OutString((char *)"Balls:");
  ST7735_OutUDec(gameState.getBallsRemaining());
  ST7735_SetCursor(7, 0);
  ST7735_OutString((char *)"Score:");
  ST7735_OutUDec(gameState.getScore());
  ST7735_SetCursor(15, 0);
  ST7735_OutString((char *)"Lvl:");
  ST7735_OutUDec(gameState.getCurrentLevel());
  
  //if (levelSelect == 1) {
  Level* level = new Level(1);
  //} else {
  // Level* level = new Level(2);
  //}
  ST7735_DrawBitmap(0, 160, level->getImage(), 128, 160);
  int x = 0;
  int y = 0;
  bool found;
  SeedRandom(TIMG12->COUNTERREGS.CTR);
   while (pegCount < 25) {
    found = false;
    x = Random(113) + 4;
    y = Random(113) + 24;
    for (int i = 0; i < pegCount; i++) {
      if (((x - (pegs[i].getX() >> FIX)) < 16 && (x - (pegs[i].getX() >> FIX)) > -16) && ((y - (pegs[i].getY() >> FIX)) < 16 && (y - (pegs[i].getY() >> FIX)) > -16)) {
        found = true;
        break;
      }
    }
    if (!found) {
      pegs[pegCount].init(x*256, y*256, 0, Random(3));
      pegCount++;
    }
  }
  for (int i = 0; i < pegCount; i++) {
    ST7735_DrawBitmap(pegs[i].getX() >> FIX, pegs[i].getY() >> FIX, pegs[i].getImage(), 8, 8);
  }
while(1){
    data = Sensor.Convert(data);
    
    // Update game stats display
    ST7735_SetCursor(0, 0);
    ST7735_OutString((char *)"Balls:");
    ST7735_OutUDec(gameState.getBallsRemaining());
    ST7735_SetCursor(7, 0);
    ST7735_OutString((char *)"Score:");
    ST7735_OutUDec(gameState.getScore());
    ST7735_SetCursor(15, 0);
    ST7735_OutString((char *)"Lvl:");
    ST7735_OutUDec(gameState.getCurrentLevel());
    
    // Draw current game elements
    ST7735_DrawBitmap(currBall->getX() >> FIX, currBall->getY() >> FIX, currBall->getImage(), 8, 8);
    ST7735_DrawBitmap(movingHole->getX() >> FIX, movingHole->getY() >> FIX, movingHole->getImage(), 48, 24);
    
    // Check for game over
    if (gameState.isGameOver()) {
      ST7735_FillScreen(ST7735_BLACK);
      ST7735_SetCursor(1, 1);
      ST7735_OutString((char *)"GAME OVER");
      ST7735_SetCursor(1, 3);
      ST7735_OutString((char *)"Final Score:");
      ST7735_SetCursor(1, 4);
      ST7735_OutUDec(gameState.getScore());
      
      // Wait for button press to restart
      while(Switch_In() == 0) {
        // Wait for button press
      }
      
      // Reset game
      gameState.resetGame();
      pegCount = 0;
      currBall->reset(192);
      ST7735_FillScreen(ST7735_BLACK);
      
      // Redraw level and pegs
      ST7735_DrawBitmap(0, 160, level->getImage(), 128, 160);
      
      // Regenerate pegs
      while (pegCount < 25) {
        found = false;
        x = Random(113) + 4;
        y = Random(113) + 24;
        for (int i = 0; i < pegCount; i++) {
          if (((x - (pegs[i].getX() >> FIX)) < 16 && (x - (pegs[i].getX() >> FIX)) > -16) && ((y - (pegs[i].getY() >> FIX)) < 16 && (y - (pegs[i].getY() >> FIX)) > -16)) {
            found = true;
            break;
          }
        }
        if (!found) {
          pegs[pegCount].init(x*256, y*256, 0, Random(3));
          pegCount++;
        }
      }
      
      // Draw pegs
      for (int i = 0; i < pegCount; i++) {
        ST7735_DrawBitmap(pegs[i].getX() >> FIX, pegs[i].getY() >> FIX, pegs[i].getImage(), 8, 8);
      }
    }
    
    // Handle level advancement (could be triggered by specific conditions)
    // For example, if all pegs are hit or a certain score is reached
    if (gameState.getScore() >= (gameState.getCurrentLevel() * 500)) {
      gameState.nextLevel();
      // Load the next level, reset ball, etc.
      ST7735_FillScreen(ST7735_BLACK);
      
      // Load new level
      delete level;
      level = new Level(gameState.getCurrentLevel());
      
      // Draw new level
      ST7735_DrawBitmap(0, 160, level->getImage(), 128, 160);
      
      // Reset pegs
      pegCount = 0;
      
      // Generate new pegs for next level
      while (pegCount < 25) {
        found = false;
        x = Random(113) + 4;
        y = Random(113) + 24;
        for (int i = 0; i < pegCount; i++) {
          if (((x - (pegs[i].getX() >> FIX)) < 16 && (x - (pegs[i].getX() >> FIX)) > -16) && 
              ((y - (pegs[i].getY() >> FIX)) < 16 && (y - (pegs[i].getY() >> FIX)) > -16)) {
            found = true;
            break;
          }
        }
        if (!found) {
          pegs[pegCount].init(x*256, y*256, 0, Random(3));
          pegCount++;
        }
      }
      
      // Draw new pegs
      for (int i = 0; i < pegCount; i++) {
        ST7735_DrawBitmap(pegs[i].getX() >> FIX, pegs[i].getY() >> FIX, pegs[i].getImage(), 8, 8);
      }
      
      // Reset ball
      currBall->reset(192);
      
      // Play level-up sound
      Sound_Fastinvader1();
    }
    
    // wait for semaphore
    // clear semaphore
    // update ST7735R
  }
}
