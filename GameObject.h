#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <cstdint>

struct GameObject {
    int32_t x, y;           // Position
    uint32_t w = 0, h = 0;  // Size
    uint16_t const *image;

    GameObject() = default;
};

#endif