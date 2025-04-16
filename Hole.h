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

    void moveHole();

private:
    int32_t vx; // Velocity
    bool active;
};

#endif 
