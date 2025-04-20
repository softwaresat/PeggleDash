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
    void setXPrev(int32_t xval);
    void setYPrev(int32_t yval);
    void moveHole();

private:
    int32_t vx; // Velocity
    bool active;
    int32_t xprev;
    int32_t yprev;
};

#endif 
