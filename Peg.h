#ifndef PEG_H
#define PEG_H

#include <cstdint>
#include "GameObject.h"

struct Peg: public GameObject {
public:
    int32_t hits;

    Peg(int32_t x, int32_t y, int32_t hits);              
    ~Peg();
    
    void updatePeg();

private:
    
};

#endif 
