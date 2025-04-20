#ifndef HOLE_H
#define HOLE_H

#include <cstdint>
#include "GameObject.h"

struct Hole: public GameObject {
public:
   
    Hole();               // Constructor
    void reset();
    ~Hole();
    
    int32_t getX();
    int32_t getY();

    bool getActive();
    void setActive();
    const uint16_t* getImage();
    int32_t getXPrev();
    int32_t getYPrev();
    int32_t setXPrev(int32_t xval);
    int32_t setYPrev(int32_t yval);
    void moveHole();

private:
    int32_t vx; // Velocity
    bool active;
    int32_t xprev;
    int32_t yprev;
};

#endif 
