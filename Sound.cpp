// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// your name
// your data 
#include <cstdint>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "Sounds.h"
#include "Sounds.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"

uint32_t Index = 0;
const uint8_t* soundPtr = nullptr;
uint32_t soundLen = 0;
uint32_t soundIndex = 0;
bool isPlaying = false;

const uint8_t* soundPtr = nullptr;
uint32_t soundLen = 0;
uint32_t soundIndex = 0;
bool isPlaying = false;

// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
void Sound_Init(void){
  DAC5_Init(); // Initialize the DAC
  
  uint32_t period = 7273; // For 11kHz sampling rate
  
  DAC5_Init(); // Initialize the DAC
  
  uint32_t period = 7273; // For 11kHz sampling rate
  
  SysTick->CTRL = 0;         // disable SysTick during setup
  SysTick->LOAD = period-1;  // reload value
  SysTick->VAL = 0;          // any write to current clears it
  SCB->SHP[1] = SCB->SHP[1]&(~0xC0000000)|0x40000000; // set priority = 2
  SysTick->CTRL = 0x00;      // enable SysTick but don't enable interrupt yet
  SCB->SHP[1] = SCB->SHP[1]&(~0xC0000000)|0x40000000; // set priority = 2
  SysTick->CTRL = 0x00;      // enable SysTick but don't enable interrupt yet
}


extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ // called at 11 kHz
  if(isPlaying){
    if(soundIndex < soundLen){
      DAC5_Out(soundPtr[soundIndex]); // Output current sample
      soundIndex++;
    } else {
      // Sound playback complete
      isPlaying = false;
      SysTick->CTRL = 0x01; // Disable interrupt but keep SysTick enabled
    }
  }
  if(isPlaying){
    if(soundIndex < soundLen){
      DAC5_Out(soundPtr[soundIndex]); // Output current sample
      soundIndex++;
    } else {
      // Sound playback complete
      isPlaying = false;
      SysTick->CTRL = 0x01; // Disable interrupt but keep SysTick enabled
    }
  }
}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Start(const uint8_t *pt, uint32_t count){
  if(pt != nullptr && count > 0){
    soundPtr = pt;
    soundLen = count;
    soundIndex = 0;
    isPlaying = true;
    SysTick->CTRL = 0x07; // Enable SysTick with interrupt
  }
  if(pt != nullptr && count > 0){
    soundPtr = pt;
    soundLen = count;
    soundIndex = 0;
    isPlaying = true;
    SysTick->CTRL = 0x07; // Enable SysTick with interrupt
  }
}

void Sound_Shoot(void){
  Sound_Start(shoot, sizeof(shoot)/sizeof(shoot[0]));
  Sound_Start(shoot, sizeof(shoot)/sizeof(shoot[0]));
}


void Sound_Killed(void){
  Sound_Start(invaderkilled, sizeof(invaderkilled)/sizeof(invaderkilled[0]));
  Sound_Start(invaderkilled, sizeof(invaderkilled)/sizeof(invaderkilled[0]));
}


void Sound_Explosion(void){
  Sound_Start(explosion, sizeof(explosion)/sizeof(explosion[0]));
  Sound_Start(explosion, sizeof(explosion)/sizeof(explosion[0]));
}

void Sound_Fastinvader1(void){
  Sound_Start(fastinvader1, sizeof(fastinvader1)/sizeof(fastinvader1[0]));
  Sound_Start(fastinvader1, sizeof(fastinvader1)/sizeof(fastinvader1[0]));
}


void Sound_Fastinvader2(void){
  Sound_Start(fastinvader2, sizeof(fastinvader2)/sizeof(fastinvader2[0]));
  Sound_Start(fastinvader2, sizeof(fastinvader2)/sizeof(fastinvader2[0]));
}


void Sound_Fastinvader3(void){
  Sound_Start(fastinvader3, sizeof(fastinvader3)/sizeof(fastinvader3[0]));
  Sound_Start(fastinvader3, sizeof(fastinvader3)/sizeof(fastinvader3[0]));
}


void Sound_Fastinvader4(void){
  Sound_Start(fastinvader4, sizeof(fastinvader4)/sizeof(fastinvader4[0]));
  Sound_Start(fastinvader4, sizeof(fastinvader4)/sizeof(fastinvader4[0]));
}


void Sound_Highpitch(void){
  Sound_Start(highpitch, sizeof(highpitch)/sizeof(highpitch[0]));
}
