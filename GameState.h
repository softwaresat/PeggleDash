#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <cstdint>

class GameState {
private:
    uint8_t ballsRemaining;   // Number of balls the player has left
    uint32_t score;           // Player's current score
    uint8_t currentLevel;     // Current level (1, 2, etc.)
    bool gameOver;            // Flag to indicate if the game is over
    
public:
    GameState();              // Constructor
    
    // Getters
    uint8_t getBallsRemaining() const;
    uint32_t getScore() const;
    uint8_t getCurrentLevel() const;
    bool isGameOver() const;
    
    // Setters and state modifiers
    void setBallsRemaining(uint8_t balls);
    void setScore(uint32_t newScore);
    void setCurrentLevel(uint8_t level);
    void setGameOver(bool over);
    
    // Game actions
    void addPoints(uint32_t points);
    void useBall();           // Decrements ball count
    void addBall();           // Adds a ball back to the player
    void resetGame();         // Reset to initial state
    void nextLevel();         // Advance to next level
};

#endif // GAMESTATE_H