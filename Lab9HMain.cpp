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

// Menu states
#define MENU_MAIN     0
#define MENU_INSTRUCT 1
#define GAME_RUNNING  2
#define GAME_OVER     3

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

// Draw the title 
void DrawTitle() {
  ST7735_SetCursor(5, 2);
  ST7735_OutString((char *)"PEGGLE DASH");
}

// Draw main menu options
void DrawMainMenu() {
  ST7735_SetCursor(5, 6);
  if (selectedOption == 0) {
    ST7735_OutString((char *)"> Play Game");
  } else {
    ST7735_OutString((char *)"  Play Game");
  }
  
  ST7735_SetCursor(5, 8);
  if (selectedOption == 1) {
    ST7735_OutString((char *)"> Instructions");
  } else {
    ST7735_OutString((char *)"  Instructions");
  }
  
  // Footer
  ST7735_SetCursor(1, 15);
  ST7735_OutString((char *)"Press btn to select");
}

// Draw instructions
void DrawInstructions() {
  ST7735_SetCursor(5, 1);
  ST7735_OutString((char *)"HOW TO PLAY");
  
  ST7735_SetCursor(0, 4);
  ST7735_OutString((char *)"- Use slidepot to aim");
  
  ST7735_SetCursor(0, 7);
  ST7735_OutString((char *)"- Press button to shoot");
  
  ST7735_SetCursor(0, 10);
  ST7735_OutString((char *)"- Hit pegs to score pts");
  
  ST7735_SetCursor(0, 13);
  ST7735_OutString((char *)"- Get ball into bucket");
  
  ST7735_SetCursor(0, 16);
  ST7735_OutString((char *)"Press btn to return");
}

// Helper function to restore background at a specific position
// Helper function to restore background at a specific position
void RestoreBackground(int16_t x, int16_t y, int16_t w, int16_t h) {
  if (currentLevel == nullptr) return;
  
  // Make sure we don't try to restore outside the screen boundaries
  if (x < 0 || y < 0 || x + w > 128 || y + h > 160) return;
  
  // Create a temporary buffer to hold the section of the background
  static uint16_t backgroundSection[64]; // Max 8x8 pixels for ball
  
  // Extract the specific section of the background image
  const uint16_t* levelImage = currentLevel->getImage();
  
  // Fill the buffer with the correct background pixels
  // The level image is drawn at (0, 160) which means the y-coordinate in the image
  // is offset from the screen y-coordinate
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      // Calculate position in the level image (which is drawn at y=160)
      // This is the key fix - we need to calculate y position differently
      int imgPos = (y+j)*128 + (x+i);      
      // Make sure we don't go out of bounds
      if (imgPos >= 0 && imgPos < 128 * 160) {
        // Store in our local buffer
        backgroundSection[j * w + i] = levelImage[imgPos];
      } else {
        // Use black if we're out of bounds
        backgroundSection[j * w + i] = 0x0000;
      }
    }
  }
  
  // Draw just this small section of the background
  ST7735_DrawBitmap(x, y, backgroundSection, w, h);
  
  // Check if any pegs are in this area and need to be redrawn
  for (int i = 0; i < pegCount; i++) {
    int16_t pegX = pegs[i].getX() >> FIX;
    int16_t pegY = pegs[i].getY() >> FIX;
    
    // If this peg overlaps with our restored area, redraw it
    if (pegX + 8 > x && pegX < x + w && pegY + 8 > y && pegY < y + h) {
      ST7735_DrawBitmap(pegX, pegY, pegs[i].getImage(), 8, 8);
    }
  }
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
 // Check if ball is lost (e.g., off screen)
if (currBall->isLost()) {
  if (!transitionedToGameOver) { // Only process if not already transitioning
    gameState.useBall();
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
      
      // Update previous position
      prevBallX = currentBallX;
      prevBallY = currentBallY;
      
      // Draw current game elements
      ST7735_DrawBitmap(currentBallX, currentBallY, currBall->getImage(), 8, 8);
      ST7735_DrawBitmap(movingHole->getX() >> FIX, movingHole->getY() >> FIX, movingHole->getImage(), 48, 24);
      
      // Check for game over
      if (gameState.isGameOver() && !transitionedToGameOver) {
        menuState = GAME_OVER;
        transitionedToGameOver = true;  // Set flag to prevent repeated transitions
        ST7735_FillScreen(ST7735_BLACK);
        ST7735_SetCursor(5, 5);
        ST7735_OutString((char *)"GAME OVER");
        ST7735_SetCursor(3, 7);
        ST7735_OutString((char *)"Final Score:");
        ST7735_SetCursor(6, 9);
        ST7735_OutUDec(gameState.getScore());
        
        ST7735_SetCursor(1, 15);
        ST7735_OutString((char *)"Press any button to continue");
      }
      
      // Handle level advancement
      if (gameState.getScore() >= (gameState.getCurrentLevel() * 500)) {
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
        Sound_Explosion();
      }
    }
  }
}