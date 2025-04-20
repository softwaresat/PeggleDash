#ifndef LEVEL_H
#define LEVEL_H

#define MAP_WIDTH 16
#define MAP_HEIGHT 20

#include <cstdint>
#include "GameObject.h"

struct Level: public GameObject {
public:
    
    Level(uint8_t num);               // Constructor
    const uint16_t* getImage();
    void setSound(unsigned char sound);
    ~Level();

private:
    uint8_t levelNum;
    unsigned char sound;
};

#endif 
