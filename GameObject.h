#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <cstdint>

struct GameObject {
    int32_t x, y;           // Position
    uint16_t w = 0, h = 0;  // Size
    const uint16_t  *image;

    GameObject() = default;
};

#endif