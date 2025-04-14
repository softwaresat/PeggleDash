#include "Peg.h"
#include "GameObject.h"
#include <cstdint>
#include <iostream>



// const uint16_t ballSprite[16][16] = {
    //TODO
// };


Peg::Peg(int32_t x, int32_t y, int32_t hits) {
    this.x = x;
    this.y = y;
    this.hits = hits;
}

void Peg:updatePeg(){
    hits--;
    // Update color of peg accordingly
}