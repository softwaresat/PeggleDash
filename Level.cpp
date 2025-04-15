#include "Level.h"
#include "GameObject.h"
#include <cstdint>
#include <iostream>

Level::Level(uint8_t num){
    levelNum = num;
}

void Level::setImage(uint16_t *img){
    image = img;
}

void Level::setSound(unsigned char sound){
    this->sound = sound;
}
