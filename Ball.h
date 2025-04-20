#ifndef BALL_H
#define BALL_H

#include <cstdint>
#include "GameObject.h"

struct Ball: public GameObject {
public:
   
    Ball(int32_t angle);               // Constructor
    void reset(int32_t angle);
    ~Ball();
    
    bool getActive();
    void setActive();
    int32_t getX();
    int32_t getY();
    int16_t getW();
    int16_t getH();
    const uint16_t* getImage();

    void moveBall();
    void destroyBall();      // Example method
    void simpleReflect();    // Add simple reflection method

private:
    int32_t vx = 0, vy = 0; // Velocity
    int32_t angle;
    bool active;
    uint16_t tableLength;
    int32_t angleToIndex(int32_t angle);
};

#endif
