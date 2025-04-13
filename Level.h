#ifndef LEVEL_H
#define LEVEL_H

#include <cstdint>
class Level {
public:
    typedef struct Level {
        uint8_t levelNum;
        uint16_t *image;
        unsigned char sound_loop; 
    } Level_t;
    Level();               // Constructor

    void setImage(uint16_t *img);
    void setSound(unsigned char sound);
    ~Level();

private:
    
};

#endif 
