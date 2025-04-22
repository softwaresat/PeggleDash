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

// Colors for improved UI
#define MENU_TITLE_COLOR     ST7735_YELLOW
#define MENU_SELECTED_COLOR  ST7735_CYAN
#define MENU_NORMAL_COLOR    ST7735_WHITE
#define MENU_HEADER_COLOR    ST7735_MAGENTA
#define MENU_FOOTER_COLOR    ST7735_GREEN

// Menu states
#define MENU_MAIN     0
#define MENU_INSTRUCT 1
#define GAME_RUNNING  2
#define GAME_OVER      3
#define LEVEL_TRANSITION 4
#define GAME_WON      5

// Button press detection state
#define BUTTON_RELEASED 0

// Current menu state
Level* currentLevel = nullptr; // Store level background for redrawing
bool transitionedToGameOver = false; // Flag to prevent repeated game over transitions
uint8_t menuState = MENU_MAIN;
uint8_t selectedOption = 0;
uint8_t maxOptions = 1; // Number of options in main menu (play game and instructions)
uint32_t lastButtonState = BUTTON_RELEASED; // Track last button state

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
Ball* currBall =  new Ball(192);
Hole* movingHole = new Hole();
// Always start with level 1
const int8_t levelSelect = 1;
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

// Draw the title with decorative elements
void DrawTitle() {
  // Draw decorative border at the top
  for(int i = 0; i < 128; i+=8) {
    ST7735_DrawFastHLine(i, 0, 8, ST7735_BLUE);
  }
  
  // Set title color
  ST7735_SetTextColor(MENU_TITLE_COLOR);
  
  // Draw the game title
  ST7735_SetCursor(4, 2);
  ST7735_OutString((char *)"P E G G L E");
  ST7735_SetCursor(6, 4);
  ST7735_OutString((char *)"D A S H");
  
  // Draw decorative line
  ST7735_DrawFastHLine(20, 30, 88, ST7735_CYAN);
  ST7735_DrawFastHLine(20, 31, 88, ST7735_BLUE);
  
  // Reset text color
  ST7735_SetTextColor(MENU_NORMAL_COLOR);
}

// Draw main menu options with improved visuals
void DrawMainMenu() {
  // Draw menu options background boxes
  ST7735_FillRect(15, 44, 98, 16, ST7735_BLUE);
  ST7735_FillRect(15, 67, 98, 16, ST7735_BLUE);
  
  // Add menu option box outlines using horizontal and vertical lines
  // First box outline (top, right, bottom, left)
  ST7735_DrawFastHLine(15, 44, 98, ST7735_CYAN); // top
  ST7735_DrawFastVLine(15+98, 44, 16, ST7735_CYAN); // right
  ST7735_DrawFastHLine(15, 44+16, 98, ST7735_CYAN); // bottom
  ST7735_DrawFastVLine(15, 44, 16, ST7735_CYAN); // left
  
  // Second box outline
  ST7735_DrawFastHLine(15, 67, 98, ST7735_CYAN); // top
  ST7735_DrawFastVLine(15+98, 67, 16, ST7735_CYAN); // right
  ST7735_DrawFastHLine(15, 67+16, 98, ST7735_CYAN); // bottom
  ST7735_DrawFastVLine(15, 67, 16, ST7735_CYAN); // left

  // Handle selection highlighting
  if (selectedOption == 0) {
    ST7735_FillRect(16, 45, 96, 14, ST7735_BLUE);
    ST7735_SetTextColor(MENU_SELECTED_COLOR);
  } else {
    ST7735_FillRect(16, 45, 96, 14, ST7735_BLACK);
    ST7735_SetTextColor(MENU_NORMAL_COLOR);
  }
  
  // Draw "Play Game" option - position text inside the button
  ST7735_SetCursor(6, 5);  // Changed from (6, 6)
  ST7735_OutString((char *)"PLAY GAME");
  
  // Handle selection highlighting for second option
  if (selectedOption == 1) {
    ST7735_FillRect(16, 68, 96, 14, ST7735_BLUE);
    ST7735_SetTextColor(MENU_SELECTED_COLOR);
  } else {
    ST7735_FillRect(16, 68, 96, 14, ST7735_BLACK);
    ST7735_SetTextColor(MENU_NORMAL_COLOR);
  }
  
  // Draw "Instructions" option - position text inside the button
  ST7735_SetCursor(5, 8);  // Changed from (5, 9)
  ST7735_OutString((char *)"INSTRUCTIONS");
  
  // Reset text color for other text
  ST7735_SetTextColor(MENU_FOOTER_COLOR);
  
  // Draw controls help
  ST7735_SetCursor(2, 15);
  ST7735_OutString((char *)"UP/DOWN: Select");
  ST7735_SetCursor(2, 16);
  ST7735_OutString((char *)"LEFT: Confirm");
  
  // Reset text color
  ST7735_SetTextColor(MENU_NORMAL_COLOR);
}

// Draw instructions with improved visual design
void DrawInstructions() {
  // Draw header with decorative elements
  ST7735_DrawFastHLine(0, 10, 128, ST7735_BLUE);
  ST7735_DrawFastHLine(0, 11, 128, ST7735_CYAN);
  
  // Set header color and draw title
  ST7735_SetTextColor(MENU_HEADER_COLOR);
  ST7735_SetCursor(4, 1);
  ST7735_OutString((char *)"HOW TO PLAY");
  
  // Reset color for instructions text
  ST7735_SetTextColor(MENU_NORMAL_COLOR);
  
  // Draw instruction points with highlight boxes
  // Instruction 1
  ST7735_FillRect(2, 28, 124, 16, ST7735_BLUE);
  // Draw outline around the box (top, right, bottom, left)
  ST7735_DrawFastHLine(2, 28, 124, ST7735_CYAN); // top
  ST7735_DrawFastVLine(2+124, 28, 16, ST7735_CYAN); // right
  ST7735_DrawFastHLine(2, 28+16, 124, ST7735_CYAN); // bottom
  ST7735_DrawFastVLine(2, 28, 16, ST7735_CYAN); // left

  ST7735_SetCursor(1, 4);
  ST7735_OutString((char *)"Use slidepot to aim");
  
  // Instruction 2
  ST7735_FillRect(2, 51, 124, 16, ST7735_BLUE);
  // Draw outline around the box
  ST7735_DrawFastHLine(2, 51, 124, ST7735_CYAN); // top
  ST7735_DrawFastVLine(2+124, 51, 16, ST7735_CYAN); // right
  ST7735_DrawFastHLine(2, 51+16, 124, ST7735_CYAN); // bottom
  ST7735_DrawFastVLine(2, 51, 16, ST7735_CYAN); // left

  ST7735_SetCursor(1, 7);
  ST7735_OutString((char *)"Press button to shoot");
  
  // Instruction 3
  ST7735_FillRect(2, 74, 124, 16, ST7735_BLUE);
  // Draw outline around the box
  ST7735_DrawFastHLine(2, 74, 124, ST7735_CYAN); // top
  ST7735_DrawFastVLine(2+124, 74, 16, ST7735_CYAN); // right
  ST7735_DrawFastHLine(2, 74+16, 124, ST7735_CYAN); // bottom
  ST7735_DrawFastVLine(2, 74, 16, ST7735_CYAN); // left

  ST7735_SetCursor(1, 10);
  ST7735_OutString((char *)"Hit pegs to score pts");
  
  // Instruction 4
  ST7735_FillRect(2, 97, 124, 16, ST7735_BLUE);
  // Draw outline around the box
  ST7735_DrawFastHLine(2, 97, 124, ST7735_CYAN); // top
  ST7735_DrawFastVLine(2+124, 97, 16, ST7735_CYAN); // right
  ST7735_DrawFastHLine(2, 97+16, 124, ST7735_CYAN); // bottom
  ST7735_DrawFastVLine(2, 97, 16, ST7735_CYAN); // left

  ST7735_SetCursor(1, 13);
  ST7735_OutString((char *)"Get ball into bucket");
  
  // Footer with return instructions
  ST7735_DrawFastHLine(0, 125, 128, ST7735_CYAN);
  ST7735_DrawFastHLine(0, 126, 128, ST7735_BLUE);
  
  ST7735_SetTextColor(MENU_FOOTER_COLOR);
  ST7735_SetCursor(2, 16);
  ST7735_OutString((char *)"LEFT btn: Return to menu");
  
  // Reset text color
  ST7735_SetTextColor(MENU_NORMAL_COLOR);
}


// Draw level transition screen
void DrawLevelTransition() {
  ST7735_FillScreen(ST7735_BLACK);
  
  // Draw decorative top border with animation
  for(int i = 0; i < 128; i+=8) {
    ST7735_DrawFastHLine(i, 0, 8, ST7735_BLUE);
    Clock_Delay1ms(10);  // Small delay for animation effect
  }
  
  // Draw decorative box
  ST7735_FillRect(14, 40, 100, 60, ST7735_BLUE);
  ST7735_DrawFastHLine(14, 40, 100, ST7735_CYAN);    // top
  ST7735_DrawFastVLine(114, 40, 60, ST7735_CYAN);    // right
  ST7735_DrawFastHLine(14, 100, 100, ST7735_CYAN);   // bottom
  ST7735_DrawFastVLine(14, 40, 60, ST7735_CYAN);     // left
  
  ST7735_FillRect(15, 41, 98, 58, ST7735_BLACK);     // Inner black fill
  
  // Display level text
  ST7735_SetTextColor(MENU_TITLE_COLOR);
  ST7735_SetCursor(5, 5);
  ST7735_OutString((char *)"LEVEL 2");
  
  // Draw fancy separator
  for(int i = 0; i < 80; i+=4) {
    ST7735_DrawFastHLine(24+i, 60, 4, ST7735_CYAN);
    Clock_Delay1ms(20);  // Animation delay
  }
  
  // Display encouraging message
  ST7735_SetTextColor(MENU_NORMAL_COLOR);
  ST7735_SetCursor(3, 8);
  ST7735_OutString((char *)"GREAT JOB!");
  
  ST7735_SetCursor(2, 10);
  ST7735_OutString((char *)"GET READY FOR");
  ST7735_SetCursor(3, 11);
  ST7735_OutString((char *)"MORE ACTION!");
  
  // Footer instruction
  ST7735_SetTextColor(MENU_FOOTER_COLOR);
  ST7735_SetCursor(1, 15);
  ST7735_OutString((char *)"Press LEFT to continue");
  
  // Sound effect
  Sound_Killed();
}

// Draw the "You Won" screen at the end of level 2
void DrawYouWonScreen() {
  ST7735_FillScreen(ST7735_BLACK);
  
  // Animated top border
  for(int i = 0; i < 128; i+=4) {
    ST7735_DrawFastHLine(i, 0, 4, ST7735_GREEN);
    Clock_Delay1ms(5);  // Small delay for animation effect
  }
  
  // Draw decorative elements
  for(int i = 0; i < 3; i++) {
    ST7735_DrawFastHLine(20, 20+i, 88, ST7735_YELLOW);
    ST7735_DrawFastHLine(20, 120+i, 88, ST7735_YELLOW);
  }
  
  // Display congratulatory title with flashing effect
  for(int i = 0; i < 3; i++) {
    ST7735_SetTextColor(ST7735_GREEN);
    ST7735_SetCursor(5, 3);
    ST7735_OutString((char *)"YOU WON!");
    Clock_Delay1ms(200);
    ST7735_SetTextColor(ST7735_YELLOW);
    ST7735_SetCursor(5, 3);
    ST7735_OutString((char *)"YOU WON!");
    Clock_Delay1ms(200);
  }
  
  // Draw stars around the screen (decorative elements)
  ST7735_SetTextColor(ST7735_YELLOW);
  ST7735_SetCursor(1, 2);
  ST7735_OutChar('*');
  ST7735_SetCursor(15, 2);
  ST7735_OutChar('*');
  ST7735_SetCursor(3, 13);
  ST7735_OutChar('*');
  ST7735_SetCursor(13, 13);
  ST7735_OutChar('*');
  
  // Draw congratulatory message
  ST7735_SetTextColor(ST7735_WHITE);
  ST7735_SetCursor(3, 6);
  ST7735_OutString((char *)"CONGRATULATIONS");
  
  // Draw score in box
  ST7735_FillRect(34, 70, 60, 30, ST7735_BLUE);
  ST7735_DrawRect(34, 70, 60, 30, ST7735_CYAN);
  
  ST7735_SetTextColor(ST7735_WHITE);
  ST7735_SetCursor(5, 9);
  ST7735_OutString((char *)"SCORE:");
  ST7735_SetCursor(6, 10);
  ST7735_OutUDec(gameState.getScore());
  
  // Footer instruction
  ST7735_SetTextColor(MENU_FOOTER_COLOR);
  ST7735_SetCursor(1, 15);
  ST7735_OutString((char *)"Press LEFT for menu");
  
  // Play victory sounds
  Sound_Killed();
  Clock_Delay1ms(300);
  Sound_Killed();
  Clock_Delay1ms(300);
  Sound_Explosion();
}

// Initialize the game
void InitGame() {

  if (currentLevel != nullptr) {
    delete currentLevel;
  }
  currentLevel = new Level(levelSelect);
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
  
  Level* level = new Level(levelSelect);
  ST7735_DrawBitmap(0, 160, level->getImage(), 128, 160);
  
  // Generate pegs
  pegCount = 0;
  int x = 0;
  int y = 0;
  bool found;
  SeedRandom(TIMG12->COUNTERREGS.CTR);
  
  while (pegCount < 25) {
    found = false;
    x = Random(113) + 4;
    y = Random(113) + 24;
    if (x < 0) {
      x = -x;
    } 
    if (y < 0) {
      y = -y;
    }
    for (int i = 0; i < pegCount; i++) {
      if (((x - (pegs[i].getX() >> FIX)) < 12 && (x - (pegs[i].getX() >> FIX)) > -12) && 
          ((y - (pegs[i].getY() >> FIX)) < 12 && (y - (pegs[i].getY() >> FIX)) > -12)) {
        found = true;
        break;
      }
    }
    if (!found) {
      pegs[pegCount].init(x*256, y*256, 0, Random(2));
      pegCount++;
    }
  }
  
  // Draw all pegs
  for (int i = 0; i < pegCount; i++) {
    ST7735_DrawBitmap(pegs[i].getX() >> FIX, pegs[i].getY() >> FIX, pegs[i].getImage(), 8, 8);
  }
}

// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes 
// game engine goes 
    data = Sensor.In();
    input = Switch_In();
    movingHole->moveHole();
    for (int i = 0; i < pegCount; i++) {
      // Skip pegs that are already destroyed
      if (pegs[i].isDestroyed()) {
        continue;
      }
      
      if (currBall->checkCollision(pegs[i].getX(), pegs[i].getY())) {
        currBall->bounce(pegs[i].getX(), pegs[i].getY());
        // Award points when hitting pegs
        gameState.addPoints(10);
        // Play sound when hitting pegs
        Sound_Fastinvader1();
        // Simply update the peg (decrements hits)
        pegs[i].updatePeg();
        
        // If the peg is now destroyed, mark it to be erased in main loop
        if (pegs[i].isDestroyed()) {
          // We'll handle the actual erasing in main, since we can't do LCD operations in ISR
          pegs[i].needsErase = true;
        }
        
        break;
      }
    }
    
    // Check if ball falls into hole
    if (currBall->checkHoleCollision(movingHole->getX(), movingHole->getY())) {
      // Award bonus points for getting ball in hole (now 100 points)
      gameState.addPoints(100);
      // Award an extra ball when the player gets the ball in the bucket
      gameState.addBall();
      // Play sound when scoring
      Sound_Killed();
      // Reset the ball since it's been captured
      currBall->reset(192);
    }
    
    // Process peg hit timers
    for (int i = 0; i < pegCount; i++) {
      if (pegs[i].isHit && !pegs[i].needsErase) {
        if (pegs[i].hitTimer > 0) {
          // Decrement timer
          pegs[i].hitTimer--;
        } else {
          // Timer expired, mark for removal if peg is destroyed
          if (pegs[i].isDestroyed()) {
            pegs[i].needsErase = true;
          }
        }
      }
    }
    
    // Check if ball is lost (e.g., off screen)
    if (currBall->isLost()) {
      if (!transitionedToGameOver) { // Only process if not already transitioning
        gameState.useBall();
        // Play a sound when ball is lost
        Sound_Fastinvader2();
        // Only reset if we still have balls left
        if (!gameState.isGameOver()) {
          currBall->reset(192); // Reset with default angle
        }
      }
    }
    
    //currBall->reset(indexAngle);
    if (currBall->getActive()) {
      currBall->moveBall();
    }
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
      Sound_Shoot(); // call one of your sounds
    }
    if((last == 0)&&(now == 2)){
      Sound_Killed(); // call one of your sounds
      Sound_Killed(); // call one of your sounds
    }
    if((last == 0)&&(now == 4)){
      Sound_Explosion(); // call one of your sounds
      Sound_Explosion(); // call one of your sounds
    }
    if((last == 0)&&(now == 8)){
      Sound_Fastinvader1(); // call one of your sounds
    }
    // modify this to test all your sounds
    last = now;
    Clock_Delay(800000); // delay ~10ms at 80 MHz
    last = now;
    Clock_Delay(800000); // delay ~10ms at 80 MHz
  }
}
//hi
//hi
// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf(INITR_REDTAB);
  ST7735_InitPrintf(INITR_REDTAB);
  ST7735_FillScreen(ST7735_BLACK);
  Sensor.Init(); // PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
  // initialize interrupts on TimerG12 at 30 Hz
  // initialize interrupts on TimerG12 at 30 Hz
  TimerG12_Init();
  TimerG12_IntArm(2666667, 2);
  // initialize all data structures
  __enable_irq();
  
  // Play startup sound
  Sound_Shoot();
  
  // Draw title screen
  DrawTitle();
  DrawMainMenu();
  
  uint32_t lastInput = BUTTON_RELEASED;
  uint32_t currentInput = BUTTON_RELEASED;
  Level* level = nullptr;
  
  // Main game loop
  while(1) {
    currentInput = Switch_In();
    // Give more time for switch debouncing
    Clock_Delay(800000);  // increased from 300000 to 800000 for better debouncing
    
    // Simplified button press detection
    if (currentInput != lastInput) {  // Only process when input changes
      if (currentInput != 0) {  // Any button is pressed
        if (menuState == MENU_MAIN) {
          // Main Menu Controls
          if (currentInput & BUTTON_DOWN) {
            // Toggle between options with any up/down press
            selectedOption = (selectedOption == 0) ? 1 : 0;
            DrawMainMenu();
          } else if (currentInput & BUTTON_LEFT) {
            // Option selected
            Sound_Shoot(); // Selection sound
            
            if (selectedOption == 0) {
              // Start Game
              menuState = GAME_RUNNING;
              ST7735_FillScreen(ST7735_BLACK);
              InitGame();
              currBall->reset(192);
            } else if (selectedOption == 1) {
              // Show Instructions
              menuState = MENU_INSTRUCT;
              ST7735_FillScreen(ST7735_BLACK);
              DrawInstructions();
            }
          }
        } else if (menuState == MENU_INSTRUCT) {
          // Instructions Screen Controls
          if (currentInput & BUTTON_LEFT) {
            // Return to main menu
            menuState = MENU_MAIN;
            ST7735_FillScreen(ST7735_BLACK);
            DrawTitle();
            DrawMainMenu();
          }
        } else if (menuState == GAME_RUNNING) {
          // In-game controls
          if (currentInput & BUTTON_LEFT) { // Use specific button to return to menu
            menuState = MENU_MAIN;
            ST7735_FillScreen(ST7735_BLACK);
            DrawTitle();
            DrawMainMenu();
            if (level != nullptr) {
              delete level;
              level = nullptr;
            }
          } else if ((currentInput & BUTTON_DOWN) && !currBall->getActive()) {
            // Shoot the ball when player presses the right button and it was previously inactive
            currBall->setActive(true);
            // Play sound when shooting
            Sound_Shoot();
          }
        } else if (menuState == GAME_OVER) {
          // Game Over Screen Controls
          if (currentInput & BUTTON_LEFT) {
            // Return to main menu
            menuState = MENU_MAIN;
            transitionedToGameOver = false;  // Reset the flag
            ST7735_FillScreen(ST7735_BLACK);
            DrawTitle();
            DrawMainMenu();
          }
        } else if (menuState == LEVEL_TRANSITION) {
          // Level Transition Screen Controls
          if (currentInput & BUTTON_LEFT) {
            // Continue to level 2
            menuState = GAME_RUNNING;
            ST7735_FillScreen(ST7735_BLACK);
            
            // Load new level
            if (level != nullptr) {
              delete level;
            }
            level = new Level(gameState.getCurrentLevel());
            
            // Draw the level background
            ST7735_DrawBitmap(0, 160, level->getImage(), 128, 160);
            
            // Reset pegs and generate new ones for level 2
            pegCount = 0;
            int x = 0;
            int y = 0;
            bool found = false;
            
            while (pegCount < 25) {
              found = false;
              x = Random(113) + 4;
              y = Random(104) + 24;
              if (x < 0) {
                x = -x;
              } 
              if (y < 0) {
                y = -y;
              } 
              for (int i = 0; i < pegCount; i++) {
                if (((x - (pegs[i].getX() >> FIX)) < 12 && (x - (pegs[i].getX() >> FIX)) > -12) && 
                    ((y - (pegs[i].getY() >> FIX)) < 12 && (y - (pegs[i].getY() >> FIX)) > -12)) {
                  found = true;
                  break;
                }
              }
              if (!found) {
                // Add more variety of pegs in level 2
                pegs[pegCount].init(x*256, y*256, 0, Random(3));
                pegCount++;
              }
            }
            
            // Draw pegs for new level
            for (int i = 0; i < pegCount; i++) {
              ST7735_DrawBitmap(pegs[i].getX() >> FIX, pegs[i].getY() >> FIX, pegs[i].getImage(), 8, 8);
            }
            
            // Reset ball
            currBall->reset(192);
            
            // Play level start sound
            Sound_Shoot();
          }
        } else if (menuState == GAME_WON) {
          // Game Won Screen Controls
          if (currentInput & BUTTON_LEFT) {
            // Return to main menu
            menuState = MENU_MAIN;
            ST7735_FillScreen(ST7735_BLACK);
            DrawTitle();
            DrawMainMenu();
            
            // Clean up level if needed
            if (level != nullptr) {
              delete level;
              level = nullptr;
            }
          }
        }
      }
    }
    
    // Update last input state
    lastInput = currentInput;
    
    // Game state processing
    if (menuState == GAME_RUNNING) {
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
      
      // Store the previous position of the ball
      static int16_t prevBallX = -1;
      static int16_t prevBallY = -1;
      
      // Current ball position
      int16_t currentBallX = currBall->getX() >> FIX;
      int16_t currentBallY = currBall->getY() >> FIX;
      
      // Clear the previous position of the ball using black sprite
      if (prevBallX >= 0 && prevBallY >= 0 && 
          (prevBallX != currentBallX || prevBallY != currentBallY)) {
        ST7735_DrawBitmap(prevBallX, prevBallY, BlackCoverSprite, 8, 8);
      }
      
      // Check for pegs that need to be erased from the screen
      for (int i = 0; i < pegCount; i++) {
        if (pegs[i].needsErase) {
          // Get peg position and erase it
          int16_t pegX = pegs[i].getX() >> FIX;
          int16_t pegY = pegs[i].getY() >> FIX;
          
          // Erase the peg by drawing a black sprite over it
          ST7735_DrawBitmap(pegX, pegY, BlackCoverSprite, 8, 8);
          
          // Reset the flag so we don't erase it again
          pegs[i].needsErase = false;
        }
      }
      
      // Update previous position
      prevBallX = currentBallX;
      prevBallY = currentBallY;
      
      // Draw current game elements
      ST7735_DrawBitmap(currentBallX, currentBallY, currBall->getImage(), 8, 8);
      
      // Only draw pegs that aren't destroyed
      for (int i = 0; i < pegCount; i++) {
        if (!pegs[i].isDestroyed()) {
          ST7735_DrawBitmap(pegs[i].getX() >> FIX, pegs[i].getY() >> FIX, pegs[i].getImage(), 8, 8);
        }
      }
      
      ST7735_DrawBitmap(movingHole->getX() >> FIX, movingHole->getY() >> FIX, movingHole->getImage(), 48, 24);
      
      // Check for game over
      if (gameState.isGameOver() && !transitionedToGameOver) {
        menuState = GAME_OVER;
        transitionedToGameOver = true;  // Set flag to prevent repeated transitions
        ST7735_FillScreen(ST7735_BLACK);
        
        // Draw decorative top border
        for(int i = 0; i < 128; i+=8) {
          ST7735_DrawFastHLine(i, 0, 8, ST7735_RED);
        }
        
        // Draw game over title
        ST7735_SetTextColor(MENU_TITLE_COLOR);
        ST7735_SetCursor(5, 2);
        ST7735_OutString((char *)"GAME OVER");
        
        // Draw decorative separator
        ST7735_DrawFastHLine(20, 20, 88, ST7735_RED);
        ST7735_DrawFastHLine(20, 21, 88, ST7735_RED);
        
        // Draw score frame - filled rectangle with outline
        ST7735_FillRect(24, 50, 80, 40, ST7735_BLUE);
        
        // Draw rectangle outline using horizontal and vertical lines
        ST7735_DrawFastHLine(24, 50, 80, ST7735_CYAN); // top
        ST7735_DrawFastVLine(24+80, 50, 40, ST7735_CYAN); // right
        ST7735_DrawFastHLine(24, 50+40, 80, ST7735_CYAN); // bottom
        ST7735_DrawFastVLine(24, 50, 40, ST7735_CYAN); // left
        
        // Fill inner rectangle with different color
        ST7735_FillRect(25, 51, 78, 38, ST7735_BLACK);

        // Display final score heading
        ST7735_SetTextColor(MENU_HEADER_COLOR);
        ST7735_SetCursor(4, 5);
        ST7735_OutString((char *)"FINAL SCORE");
        
        // Display score value with highlight
        ST7735_SetTextColor(ST7735_WHITE);
        ST7735_SetCursor(6, 8);
        ST7735_OutUDec(gameState.getScore());
        
        // Draw footer with return instructions
        ST7735_DrawFastHLine(0, 130, 128, ST7735_BLUE);
        ST7735_DrawFastHLine(0, 131, 128, ST7735_CYAN);
        
        ST7735_SetTextColor(MENU_FOOTER_COLOR);
        ST7735_SetCursor(1, 15);
        ST7735_OutString((char *)"Press LEFT to continue");
        
        // Reset text color
        ST7735_SetTextColor(MENU_NORMAL_COLOR);
        
        // Play game over sound
        Sound_Explosion();
      }
      
      // Handle level advancement
      if (gameState.getScore() >= (gameState.getCurrentLevel() * 500)) {
        // Check if we're on level 1 and advancing to level 2
        if (gameState.getCurrentLevel() == 1) {
          menuState = LEVEL_TRANSITION;
          gameState.nextLevel(); // Advance to level 2
          
          // Show level transition screen
          DrawLevelTransition();
          
          // We'll wait for button press to continue (handled in input processing)
          continue; // Skip the rest of the loop iteration
        } 
        // Check if we're completing level 2
        else if (gameState.getCurrentLevel() == 2) {
          menuState = GAME_WON;
          
          // Show victory screen
          DrawYouWonScreen();
          
          // We'll wait for button press to return to menu (handled in input processing)
          continue; // Skip the rest of the loop iteration
        }
        else {
          gameState.nextLevel();
          // Load the next level, reset ball, etc.
          ST7735_FillScreen(ST7735_BLACK);
          
          // Load new level
          if (level != nullptr) {
            delete level;
          }
          level = new Level(gameState.getCurrentLevel());
          
          // Draw new level
          ST7735_DrawBitmap(0, 160, level->getImage(), 128, 160);
          
          // Reset pegs
          pegCount = 0;
          
          // Generate new pegs for next level
          int x = 0;
          int y = 0;
          bool found = false;
          
          while (pegCount < 25) {
            found = false;
            x = Random(113) + 4;
            y = Random(104) + 24;
            if (x < 0) {
              x = -x;
            } 
            if (y < 0) {
              y = -y;
            } 
            for (int i = 0; i < pegCount; i++) {
              if (((x - (pegs[i].getX() >> FIX)) < 12 && (x - (pegs[i].getX() >> FIX)) > -12) && 
                  ((y - (pegs[i].getY() >> FIX)) < 12 && (y - (pegs[i].getY() >> FIX)) > -12)) {
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
          Sound_Explosion();
        }
      }
    }
  }
}