#ifndef LEVEL_H
#define LEVEL_H

#define MAP_WIDTH 16
#define MAP_HEIGHT 20

#include <cstdint>
#include "GameObject.h"
#include "Peg.h"  // for Peg

struct Level: public GameObject {
public:
<<<<<<< HEAD
    
=======
    GameObject levelMap[MAP_HEIGHT][MAP_WIDTH];
    Peg* pegs[MAP_HEIGHT][MAP_WIDTH];  // peg pointers for this level
>>>>>>> 418a656d28b85260a48d4ee6dad2de8ddc9f9f0c
    Level(uint8_t num);               // Constructor
    const uint16_t* getImage();
    void setSound(unsigned char sound);
    ~Level();

private:
    uint8_t levelNum;
    unsigned char sound;
};

#endif
