#ifndef LEVEL_H
#define LEVEL_H

#define MAP_WIDTH 16
#define MAP_HEIGHT 20

#include <cstdint>
#include "GameObject.h"

struct Level {
public:
    uint8_t levelNum;
    uint16_t *image;
    unsigned char sound_loop; 
    GameObject levelMap[MAP_HEIGHT][MAP_WIDTH];
    Level();               // Constructor

    void setImage(uint16_t *img);
    void setSound(unsigned char sound);
    ~Level();

private:
    
};

#endif 
