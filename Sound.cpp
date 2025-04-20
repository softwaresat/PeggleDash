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
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"
#include "Sounds.h"


uint32_t Index = 0;
const uint8_t* outTable = nullptr;
uint32_t len;

void SysTick_IntArm(uint32_t period, uint32_t priority){
  // write this
}
// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
void Sound_Init(void){

  uint32_t period = 7273;
    Index = 0;

  SysTick->CTRL = 0;         // disable SysTick during setup
  SysTick->LOAD = period-1;  // reload value
  SysTick->VAL = 0;          // any write to current clears it
  SCB->SHP[1] = SCB->SHP[1]&(~0xC0000000)|0x40000000; // set priority = 1
  SysTick->CTRL = 0x1F;    // enable SysTick with core clock and interrupt 
}
extern "C" void SysTick_Handler(void);
void SysTick_Handler(void){ // called at 11 kHz
    DAC5_Out(outTable[Index]);
  
    Index++;
    Index &= 0x1F;  
  // output one value to DAC if a sound is active
    // output one value to DAC if a sound is active

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

    outTable = pt;
    len = count;

    SysTick->LOAD = count-1;
    SysTick->VAL = 0;  
  
}

void Sound_Shoot(void){
// write this
  Sound_Start( shoot, 4080);
}
void Sound_Killed(void){
// write this

}
void Sound_Explosion(void){
// write this

}

void Sound_Fastinvader1(void){

}
void Sound_Fastinvader2(void){

}
void Sound_Fastinvader3(void){

}
void Sound_Fastinvader4(void){

}
void Sound_Highpitch(void){

}
