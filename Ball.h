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
    int8_t getCounter();
    void setCounter(int8_t val);
    void IncrementVY();
    const uint16_t* getImage();
    bool checkCollision(uint16_t objX, uint16_t objY, uint16_t objW, uint16_t objH);
    void bounce(uint16_t objX, uint16_t objY);
    void moveBall();
    void destroyBall();
    int8_t angleToIndex();      // Example method

private:
    int8_t index;
    int8_t counter;
    int32_t vx = 0, vy = 0; // Velocity
    int32_t angle;
    bool active;
    uint16_t tableLength;
};

#endif 
