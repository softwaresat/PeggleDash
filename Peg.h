#ifndef PEG_H
#define PEG_H

#include <cstdint>
#include "GameObject.h"

struct Peg: public GameObject {
public:
    int32_t hits;
    Peg(int32_t x, int32_t y, int32_t hits, int32_t color);              
    ~Peg();
    int16_t getX();
    int16_t getY();
    uint16_t getW();
    uint16_t getH();
    const uint16_t* getImage();
    void updatePeg();

private:
    
};

#endif 
