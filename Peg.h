#ifndef PEG_H
#define PEG_H

#include <cstdint>
#include "GameObject.h"

struct Peg: public GameObject {
public:
    Peg();              
    ~Peg() = default;
    void init(int32_t x, int32_t y, int8_t hits, int8_t color);
    int32_t getX();
    int32_t getY();
    uint16_t getW();
    uint16_t getH();
    const uint16_t* getImage();
    void updatePeg();
    bool isDestroyed();
    
    // Field to indicate if peg needs to be erased from screen
    bool needsErase;
    // Timer to control how long a hit peg displays before disappearing
    uint8_t hitTimer;

private:
    int8_t hits;
    int8_t color; // Store the peg color for getting the right hit image
    bool isHit;   // Flag to track if the peg is in "hit" state
};

#endif
