#ifndef PEG_H
#define PEG_H

#include <cstdint>
#include "GameObject.h"

extern const unsigned short bluepeg[];
extern const unsigned short orangepeg[];
extern const unsigned short bluepeghit[];
extern const unsigned short orangepeghit[];

struct Peg: public GameObject {
public:
    int32_t hits;
    int16_t prevScreenX; // Previous screen X coordinate for drawing
    int16_t prevScreenY; // Previous screen Y coordinate for drawing
    bool needsRedraw;    // Flag to indicate if the peg needs to be redrawn

    Peg(int32_t x, int32_t y, int32_t hits);              
    ~Peg();
    
    void updatePeg();

private:
    
};

#endif
