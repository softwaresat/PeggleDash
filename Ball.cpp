#include "Ball.h"
#include "GameObject.h"
#include <cstdint>
#include <iostream>

const int32_t angleTable[256][2] = {
    { 1024,    0}, { 1024,   25}, { 1023,   50}, { 1021,   75}, { 1019,  100}, { 1016,  125}, { 1013,  150}, { 1009,  175},
    { 1004,  200}, {  999,  224}, {  993,  249}, {  987,  273}, {  980,  297}, {  972,  321}, {  964,  345}, {  955,  369},
    {  946,  392}, {  936,  415}, {  926,  438}, {  915,  460}, {  903,  483}, {  891,  505}, {  878,  526}, {  865,  548},
    {  851,  569}, {  837,  590}, {  822,  610}, {  807,  630}, {  792,  650}, {  775,  669}, {  759,  688}, {  742,  706},
    {  724,  724}, {  706,  742}, {  688,  759}, {  669,  775}, {  650,  792}, {  630,  807}, {  610,  822}, {  590,  837},
    {  569,  851}, {  548,  865}, {  526,  878}, {  505,  891}, {  483,  903}, {  460,  915}, {  438,  926}, {  415,  936},
    {  392,  946}, {  369,  955}, {  345,  964}, {  321,  972}, {  297,  980}, {  273,  987}, {  249,  993}, {  224,  999},
    {  200, 1004}, {  175, 1009}, {  150, 1013}, {  125, 1016}, {  100, 1019}, {   75, 1021}, {   50, 1023}, {   25, 1024},
    {   0, 1024}, {  -25, 1024}, {  -50, 1023}, {  -75, 1021}, {-100, 1019}, {-125, 1016}, {-150, 1013}, {-175, 1009},
    {-200, 1004}, {-224,  999}, {-249,  993}, {-273,  987}, {-297,  980}, {-321,  972}, {-345,  964}, {-369,  955},
    {-392,  946}, {-415,  936}, {-438,  926}, {-460,  915}, {-483,  903}, {-505,  891}, {-526,  878}, {-548,  865},
    {-569,  851}, {-590,  837}, {-610,  822}, {-630,  807}, {-650,  792}, {-669,  775}, {-688,  759}, {-706,  742},
    {-724,  724}, {-742,  706}, {-759,  688}, {-775,  669}, {-792,  650}, {-807,  630}, {-822,  610}, {-837,  590},
    {-851,  569}, {-865,  548}, {-878,  526}, {-891,  505}, {-903,  483}, {-915,  460}, {-926,  438}, {-936,  415},
    {-946,  392}, {-955,  369}, {-964,  345}, {-972,  321}, {-980,  297}, {-273,  987}, {-249,  993}, {-224,  999},
    {-200, 1004}, {-175, 1009}, {-150, 1013}, {-125, 1016}, {-100, 1019}, { -75, 1021}, { -50, 1023}, { -25, 1024},
    {-1024,    0}, {-1024,  -25}, {-1023,  -50}, {-1021,  -75}, {-1019, -100}, {-1016, -125}, {-1013, -150}, {-1009, -175},
    {-1004, -200}, {-999, -224}, {-993, -249}, {-987, -273}, {-980, -297}, {-972, -321}, {-964, -345}, {-955, -369},
    {-946, -392}, {-936, -415}, {-926, -438}, {-915, -460}, {-903, -483}, {-891, -505}, {-878, -526}, {-865, -548},
    {-851, -569}, {-837, -590}, {-822, -610}, {-807, -630}, {-792, -650}, {-775, -669}, {-759, -688}, {-742, -706},
    {-724, -724}, {-706, -742}, {-688, -759}, {-669, -775}, {-650, -792}, {-630, -807}, {-610, -822}, {-590, -837},
    {-569, -851}, {-548, -865}, {-526, -878}, {-505, -891}, {-483, -903}, {-460, -915}, {-438, -926}, {-415, -936},
    {-392, -946}, {-369, -955}, {-345, -964}, {-321, -972}, {-297, -980}, {-273, -987}, {-249, -993}, {-224, -999},
    {-200, -1004}, {-175, -1009}, {-150, -1013}, {-125, -1016}, {-100, -1019}, { -75, -1021}, { -50, -1023}, { -25, -1024},
    {   0, -1024}, {  25, -1024}, {  50, -1023}, {  75, -1021}, { 100, -1019}, { 125, -1016}, { 150, -1013}, { 175, -1009},
    { 200, -1004}, { 224, -999}, { 249, -993}, { 273, -987}, { 297, -980}, { 321, -972}, { 345, -964}, { 369, -955},
    { 392, -946}, { 415, -936}, { 438, -926}, { 460, -915}, { 483, -903}, { 505, -891}, { 526, -878}, { 548, -865},
    { 569, -851}, { 590, -837}, { 610, -822}, { 630, -807}, { 650, -792}, { 669, -775}, { 688, -759}, { 706, -742},
    { 724, -724}, { 742, -706}, { 759, -688}, { 775, -669}, { 792, -650}, { 807, -630}, { 822, -610}, { 837, -590},
    { 851, -569}, { 865, -548}, { 878, -526}, { 891, -505}, { 903, -483}, { 915, -460}, { 926, -438}, { 936, -415},
    { 946, -392}, { 955, -369}, { 964, -345}, { 972, -321}, { 980, -297}, { 987, -273}, { 993, -249}, { 999, -224},
    {1004, -200}, {1009, -175}, {1013, -150}, {1016, -125}, {1019, -100}, {1021,  -75}, {1023,  -50}, {1024,  -25}   
};

const unsigned short ball[] = {
 0x0020, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0020, 0x0020, 0x0000, 0x0841, 0x20E2, 0x20E3, 0x0861, 0x0000, 0x0020,
 0x0000, 0x0861, 0x4A07, 0x4A28, 0x41A6, 0x3165, 0x0861, 0x0000, 0x0000, 0x31A6, 0x6B8F, 0x634E, 0x528A, 0x41A6, 0x18C3, 0x0000,
 0x0000, 0x4229, 0xA5D9, 0x84D5, 0x634E, 0x4A48, 0x18C2, 0x0000, 0x0000, 0x10A3, 0xAE19, 0xA5FA, 0x6BAF, 0x4A48, 0x0841, 0x0000,
 0x0020, 0x0000, 0x10C3, 0x424A, 0x39C7, 0x1082, 0x0000, 0x0020, 0x0020, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0020,
};

Ball::Ball(uint8_t angle) 
{
    active = false;
    x = 64 * 256;
    y = 8 * 256;
    vx = angleTable[angle][0];
    vy = angleTable[angle][1]; 
    image = ball;
    h = 8;
    w = 8;
    x = 64 * 256;
    y = 8 * 256;
    vx = angleTable[angle][0];
    vy = angleTable[angle][1]; 
    image = ball;
    h = 8;
    w = 8;
}

void Ball::moveBall(){
    // Apply gravity with a smaller increment for smoother motion
    vy += 32;  // reduced from 48 to 32 for smoother acceleration
    
    // Apply air resistance
    vx = (vx * 252) >> 8;  // slight air resistance (multiply by ~0.984)
    
    // Terminal velocity check
    if (vy > 1280) {  // reduced from 1536 to 1280
        vy = 1280;
    }

    x += vx;
    y += vy; 
    
    // Handle wall collisions with proper energy loss
    if((x / 256) + 8 >= 128 || x <= 0){
        vx = -((vx * 230) >> 8);  // slightly more energy loss on wall hits
        if(x <= 0) x = 1;  // Prevent sticking to walls
        if((x / 256) + 8 >= 128) x = (120 << 8);
    }
    if((y / 256) + 8 >= 160 || y <= 0){
        vy = -((vy * 230) >> 8);  // slightly more energy loss on floor/ceiling
        if(y <= 0) y = 1;  // Prevent sticking to ceiling
        if((y / 256) + 8 >= 160) y = (152 << 8);
    }
}

void Ball::reset(uint8_t angle){
    active = false; // The ball should start inactive after reset
    x = 64 * 256;  // Reset x position to middle of screen
    y = 8 * 256;   // Reset y position to top of screen
    vx = angleTable[angle][0];
    vy = angleTable[angle][1];    
}

bool Ball::checkCollision(uint16_t objX, uint16_t objY) {
    int32_t ballX = x >> 8;
    int32_t ballY = y >> 8;
    int32_t pegX = objX >> 8;
    int32_t pegY = objY >> 8;

    int32_t dx = ballX - pegX;
    int32_t dy = ballY - pegY;

    // Increased collision radius for better detection
    int32_t radiusSum = 3 + 3;  // Using radius 4 for both ball and peg (8x8 sprites)

    return (dx * dx + dy * dy < radiusSum * radiusSum);
}

void Ball::bounce(uint16_t objX, uint16_t objY) {
    uint16_t ballX = x >> 8;
    uint16_t ballY = y >> 8;
    uint16_t pegX = objX >> 8;
    uint16_t pegY = objY >> 8;

    int16_t dx = ballX - pegX;
    int16_t dy = ballY - pegY;

    // Normalize the collision vector
    int32_t lengthSq = dx * dx + dy * dy;
    if (lengthSq == 0) return;  // avoid div/zero

    // Get unit normal (Q8.8 fixed-point)
    int32_t len = isqrt(lengthSq);     // length in pixels
    if (len == 0) len = 1;             // avoid div by zero
    int32_t nx = (dx << 8) / len;      // normalize to Q8.8
    int32_t ny = (dy << 8) / len;

    // Reflect velocity around the normal with energy loss
    int32_t dot = (vx * nx + vy * ny) >> 8;  // projection of velocity on normal
    
    // Apply reflection with energy loss
    vx = ((vx - 2 * ((dot * nx) >> 8)) * 230) >> 8;  // 230/256 = ~0.90 energy retention
    vy = ((vy - 2 * ((dot * ny) >> 8)) * 230) >> 8;

    // Clamp velocity to prevent excessive speeds
    int32_t speedSq = (vx * vx + vy * vy);
    const int32_t maxSpeed = 896;  // reduced from 1024 for better control
    if (speedSq > (maxSpeed * maxSpeed)) {
        int32_t scale = (maxSpeed << 8) / isqrt(speedSq);
        vx = (vx * scale) >> 8;
        vy = (vy * scale) >> 8;
    }

    // Ensure minimum speed after bounce to prevent getting stuck
    const int32_t minSpeed = 128;  // minimum speed after bounce
    if (speedSq < (minSpeed * minSpeed)) {
        int32_t scale = (minSpeed << 8) / isqrt(speedSq);
        vx = (vx * scale) >> 8;
        vy = (vy * scale) >> 8;
    }

    // Move ball out of collision
    x += (vx >> 5);  // Increased from >>6 to >>5 to better escape collision
    y += (vy >> 5);
}

bool Ball::checkHoleCollision(uint16_t holeX, uint16_t holeY) {
    int32_t ballX = x >> 8;
    int32_t ballY = y >> 8;
    int32_t bucketX = holeX >> 8;
    int32_t bucketY = holeY >> 8;
    
    // The bucket/hole is larger than pegs, so use a larger radius check
    // Check if ball is entering the top part of the bucket
    if (ballX >= bucketX + 12 && ballX <= bucketX + 36 && 
        ballY >= bucketY - 4 && ballY <= bucketY + 4) {
        return true;
    }
    
    return false;
}

bool Ball::isLost() {
    // Ball is lost if it goes below the bottom of the screen
    return ((y >> 8) > 160);
}

bool Ball::getActive(){
    return active;
}

void Ball::setActive(){
    active = true;
}
void Ball::setActive(bool isActive) {
    active = isActive;
}
void Ball::IncrementVY(){
    vy += 256;
}

int32_t Ball::getX(){
    return x;
}

int32_t Ball::getY(){
    return y;
}

int16_t Ball::getW(){
    return w;
}
int16_t Ball::getH(){
    return h;
}

const uint16_t* Ball::getImage(){
    return image;
}

int32_t Ball::isqrt(int32_t n) {
    if (n <= 0) return 0;

    int32_t res = 0;
    int32_t bit = 1 << 30;  // Start from the highest power of 4 <= 2^30

    while (bit > n) bit >>= 2;

    while (bit != 0) {
        if (n >= res + bit) {
            n -= res + bit;
            res = (res >> 1) + bit;
        } else {
            res >>= 1;
        }
        bit >>= 2;
    }

    return res;
}

int8_t Ball::angleToIndex() {
    int bestIndex = 0;
    int32_t bestDiff = INT32_MAX;

    for (int i = 0; i < 256; i++) {
        int32_t dx = vx - angleTable[i][0];
        int32_t dy = vy - angleTable[i][1];
        int32_t diff = dx * dx + dy * dy; // Euclidean distance squared

        if (diff < bestDiff) {
            bestDiff = diff;
            bestIndex = i;
        }
    }

    return bestIndex;
}