#ifndef PEG_H
#define PEG_H

#include <cstdint>
#include "GameObject.h"

struct Peg: public GameObject {
public:
    Peg();              
    ~Peg() = default;
    void init(int16_t x, int16_t y, int8_t hits, int8_t color);
    int16_t getX();
    int16_t getY();
    uint16_t getW();
    uint16_t getH();
    const uint16_t* getImage();
    void updatePeg();
    bool isDestroyed();
    
    // Field to indicate if peg needs to be erased from screen
    bool needsErase;

private:
    int8_t hits;
};

#endif
