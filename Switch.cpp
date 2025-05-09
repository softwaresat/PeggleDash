/*
 * Switch.cpp
 *
 *  Created on: Nov 5, 2023
 *      Author:
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"
// LaunchPad.h defines all the indices into the PINCM table
void Switch_Init(void){
    // Enable pull-up resistors on switch inputs (bits 7-4 = 0010 for pull-up)
    IOMUX->SECCFG.PINCM[PA28INDEX] = 0x00050081; // input with pull-up
    IOMUX->SECCFG.PINCM[PA27INDEX] = 0x00050081; // input with pull-up
    IOMUX->SECCFG.PINCM[PA16INDEX] = 0x00050081; // input with pull-up
    IOMUX->SECCFG.PINCM[PA15INDEX] = 0x00050081; // input with pull-up
}
// return current state of switches
uint32_t Switch_In(void){
    uint32_t data = GPIOA->DIN31_0;
    data = ((data>>15)&0x03) | ((data&((1<<28)|(1<<27)))>>25);
    return data; 
}
