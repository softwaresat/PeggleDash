#include "Level.h"
#include <cstdint>
#include <iostream>

Level_t level;

Level::Level(uint8_t num){
    level.levelNum = num;
}

void Level::setImage(uint16_t *img){
    level.image = img;
}

void Level::setSound(unsigned char sound){
    level.sound = sound;
}

Level::~Level(){
    free(level);
}

