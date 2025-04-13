#ifndef BALL_H
#define BALL_H

#include <cstdint>
class Ball {
public:
    typedef struct Ball {
        int32_t x, y;
        int32_t vx, vy;
        int32_t angle;
        uint16_t const *image;
        uint32_t const h, w;
    } Ball_t;
    Ball();               // Constructor
    ~Ball();
    
    void moveBall();
    void destroyBall();      // Example method

private:
    Ball_t sprite;
    uint16_t tableLength;
    static const int32_t angleToVel[][];
    int16_t angleToIndex(int32_t angle);
    const uint16_t ballSprite[16][16];
};

#endif 
