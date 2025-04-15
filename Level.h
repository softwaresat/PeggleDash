#ifndef LEVEL_H
#define LEVEL_H

#define MAP_WIDTH 16
#define MAP_HEIGHT 20

#include <cstdint>
#include "GameObject.h"

struct Level {
public:
    
    GameObject levelMap[MAP_HEIGHT][MAP_WIDTH];
    Level(uint8_t num);               // Constructor

    void setImage(uint16_t *img);
    void setSound(unsigned char sound);
    ~Level();

private:
    uint8_t levelNum;
    uint16_t *image;
    unsigned char sound; 
};

#endif 
