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

extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);
#define BUTTON_DOWN  (1 << 0)  // PA15
#define BUTTON_RIGHT (1 << 1)  // PA16
#define BUTTON_LEFT  (1 << 2)  // PA27
#define BUTTON_UP    (1 << 3)  // PA28

// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t M=1;
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
Ball* currBall =  new Ball(28);
Hole* movingHole = new Hole();

// Define the level background to use (0=random pegs only, 1=level1 background, 2=level2 background)
#define LEVEL_BACKGROUND 1  // Change this value to select different backgrounds

Level currentLevel(LEVEL_BACKGROUND); // Create level with the selected background
volatile uint8_t TimerFlag = 0;  // signal for main display update

const uint16_t BlackCoverSprite[256] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

// games engine runs at 30Hz
void TIMG12_IRQHandler(void){
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)

    data = Sensor.In();
    data = Sensor.Convert(data);
    input = Switch_In();

    movingHole->moveHole(); // Assuming hole movement is independent of level pegs

    if(!currBall->getActive()){
      currBall->reset(data);
    }

    if(!currBall->getActive() && (input & BUTTON_DOWN) == BUTTON_DOWN){
      currBall->setActive();
    } else if(currBall->getActive()){
      currBall->moveBall();

      // Check for collisions with pegs in the current level
      for (int y = 0; y < MAP_HEIGHT; ++y) {
          for (int x = 0; x < MAP_WIDTH; ++x) {
              Peg* peg = currentLevel.pegs[y][x];
              if (peg != nullptr && peg->hits > 0) { // Only check active pegs
                  // Basic AABB collision detection (replace with circle collision if needed)
                  int32_t ballX = currBall->getX(); // Use fixed-point values
                  int32_t ballY = currBall->getY();
                  // Assuming Ball getW/getH return pixel size, convert to fixed-point if needed
                  int32_t ballW = currBall->getW() * 256; 
                  int32_t ballH = currBall->getH() * 256;
                  int32_t pegX = peg->x; // Assuming Peg stores fixed-point coords
                  int32_t pegY = peg->y;
                  int32_t pegW = peg->getW() * 256;
                  int32_t pegH = peg->getH() * 256;

                  // Simple AABB check (adjust logic for circle collision)
                  if (ballX < pegX + pegW &&
                      ballX + ballW > pegX &&
                      ballY < pegY + pegH &&
                      ballY + ballH > pegY) {

                      // Collision detected!
                      peg->hits--; // Decrement hit count
                      // Sound_Play(pegHitSound); // Play peg hit sound
                      peg->updatePeg(); // Update image based on hits remaining

                      // Reverse ball velocity (simple reflection, needs Ball modification)
                      // currBall->reflectVelocity(peg); // Placeholder for reflection logic
                      currBall->simpleReflect(); // Add a simple reflection for now

                      peg->needsRedraw = true; // Mark peg for redraw in main loop

                      // Optional: Break if ball can only hit one peg per frame
                      // goto collision_handled; 
                  }
              }
          }
      }
      // collision_handled:; // Label for goto if used

      // Add collision checks for hole, boundaries etc. here
      // ...
    }

    // signal main loop to update LCD (no LCD output in ISR)
    TimerFlag = 1;

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
  ST7735_InitPrintf(INITR_BLACKTAB);
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
  ST7735_InitPrintf(INITR_BLACKTAB);
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
    ST7735_InitPrintf(INITR_BLACKTAB);
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
      // Sound_Shoot(levelOne); // call one of your sounds
    }
    if((last == 0)&&(now == 2)){
      // Sound_Killed(levelTwo); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      // Sound_Explosion(levelThree); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
  }
}
//hi
// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_BLACKTAB);
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
  
  // Draw the background first
  if (LEVEL_BACKGROUND == 1 || LEVEL_BACKGROUND == 2) {
    // Get the appropriate background data array
    const unsigned short* backgroundData = nullptr;
    if (LEVEL_BACKGROUND == 1) {
      backgroundData = level1; // Use level1 background data
    } else if (LEVEL_BACKGROUND == 2) {
      backgroundData = level2; // Use level2 background data
    }
    
    // Draw the background
    if (backgroundData != nullptr) {
      // Draw the background tile by tile
      for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
          // Get the color value from the level data
          unsigned short color = backgroundData[y * MAP_WIDTH + x];
          
          // Calculate screen position for this tile
          int screenX = x * 16; // Using TILE_SIZE which is 16
          int screenY = y * 16;
          
          // Fill a 16x16 tile with the color
          ST7735_FillRect(screenX, screenY, 16, 16, color);
        }
      }
    }
  }
  
  // Now draw the pegs over the background
  // Draw Pegs from the level
  for (int y = 0; y < MAP_HEIGHT; ++y) {
      for (int x = 0; x < MAP_WIDTH; ++x) {
          Peg* peg = currentLevel.pegs[y][x];
          if (peg != nullptr) {
              // Convert fixed-point coordinates to screen coordinates
              int16_t screenX = (int16_t)(peg->x / 256);
              int16_t screenY = (int16_t)(peg->y / 256);
              ST7735_DrawBitmap(screenX, screenY, peg->getImage(), peg->getW(), peg->getH());
              peg->prevScreenX = screenX; // Store initial screen position
              peg->prevScreenY = screenY;
              peg->needsRedraw = false; // Initialize redraw flag
          }
      }
  }

  // Draw initial hole and ball
  ST7735_DrawBitmap((int16_t)(movingHole->getX()/256), (int16_t)(movingHole->getY()/256)-10, movingHole->getImage(), 16,16);
  int16_t prevHoleX = (int16_t)(movingHole->getX()/256);
  int16_t prevHoleY = (int16_t)(movingHole->getY()/256)-10;
  int16_t prevBallX = (int16_t)(currBall->getX()/256);
  int16_t prevBallY = (int16_t)(currBall->getY()/256) + (currBall->getActive()?0:10);
  ST7735_DrawBitmap(prevBallX, prevBallY, currBall->getImage(), currBall->getW(), currBall->getH());

  while(1){
    if(TimerFlag){
      TimerFlag = 0;

      // Erase previous sprites
      ST7735_DrawBitmap(prevHoleX, prevHoleY, BlackCoverSprite, 16,16);
      ST7735_DrawBitmap(prevBallX, prevBallY, BlackCoverSprite, currBall->getW(), currBall->getH()); // Use ball's actual W/H

      // Redraw pegs that have changed state
      for (int y = 0; y < MAP_HEIGHT; ++y) {
          for (int x = 0; x < MAP_WIDTH; ++x) {
              Peg* peg = currentLevel.pegs[y][x];
              // Check if peg exists and needs redraw
              if (peg != nullptr && peg->needsRedraw) {
                  // Erase previous peg image
                  ST7735_DrawBitmap(peg->prevScreenX, peg->prevScreenY, BlackCoverSprite, peg->getW(), peg->getH());

                  if (peg->hits > 0) {
                      // Draw updated peg image at current position (pegs don't move)
                      int16_t screenX = (int16_t)(peg->x / 256);
                      int16_t screenY = (int16_t)(peg->y / 256);
                      ST7735_DrawBitmap(screenX, screenY, peg->getImage(), peg->getW(), peg->getH());
                      // Update previous coords (though pegs don't move, good practice if they could)
                      peg->prevScreenX = screenX; 
                      peg->prevScreenY = screenY;
                  }
                  // Else: Peg is destroyed (hits <= 0), already erased, do nothing more.
                  
                  peg->needsRedraw = false; // Reset flag
              }
          }
      }

      // Draw new hole sprite
      int16_t holeX = (int16_t)(movingHole->getX()/256);
      int16_t holeY = (int16_t)(movingHole->getY()/256)-10;
      ST7735_DrawBitmap(holeX, holeY, movingHole->getImage(), 16,16);

      // Draw new ball sprite
      int16_t ballX = (int16_t)(currBall->getX()/256);
      // Adjust Y based on active state (launcher vs moving)
      int16_t ballY = (int16_t)(currBall->getY()/256) + (currBall->getActive()?0:10);
      ST7735_DrawBitmap(ballX, ballY, currBall->getImage(), currBall->getW(), currBall->getH());

      // Update previous coordinates
      prevHoleX = holeX; prevHoleY = holeY;
      prevBallX = ballX; prevBallY = ballY;
    }
    // Add a small delay or wait for interrupt flag if CPU usage is too high
    // Clock_Delay1ms(1); // Example small delay
  }
}

