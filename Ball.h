#ifndef BALL_H
#define BALL_H

#include <cstdint>
#include "GameObject.h"

struct Ball: public GameObject {
public:
   
    Ball(uint8_t angle);               // Constructor
    void reset(uint8_t angle);
    ~Ball();
    
    bool getActive();
    void setActive();
    void setActive(bool isActive); // Set active to specified state
    int32_t getX();
    int32_t getY();
    int16_t getW();
    int16_t getH();
    void IncrementVY();
    const uint16_t* getImage();
    bool checkCollision(uint16_t objX, uint16_t objY);
    bool checkHoleCollision(uint16_t holeX, uint16_t holeY);  // Added method to check collision with the bucket/hole
    bool isLost();  // Added method to check if ball is lost (off-screen)
    void bounce(uint16_t objX, uint16_t objY);
    void moveBall();
    int32_t isqrt(int32_t n);
    void destroyBall();
    int8_t angleToIndex();      // Example method

private:
    int8_t index;
    int32_t vx = 0, vy = 0; // Velocity
    int8_t angle;
    bool active;

};

#endif
