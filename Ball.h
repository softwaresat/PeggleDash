#ifndef BALL_H
#define BALL_H

#include <cstdint>
#include "GameObject.h"

struct Ball: public GameObject {
public:
    int32_t vx = 0, vy = 0; // Velocity
    int32_t angle;
    Ball(int32_t angle);               // Constructor
    ~Ball();
    
    void moveBall();
    void destroyBall();      // Example method

private:
    uint16_t tableLength;
    static const int32_t angleToVel[][];
    int16_t angleToIndex(int32_t angle);
    const uint16_t ballSprite[16][16];
};

#endif 
