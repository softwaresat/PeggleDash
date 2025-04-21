#include "GameState.h"

// Constructor - Initialize with default values
GameState::GameState() {
    resetGame();
}

// Getters
uint8_t GameState::getBallsRemaining() const {
    return ballsRemaining;
}

uint32_t GameState::getScore() const {
    return score;
}

uint8_t GameState::getCurrentLevel() const {
    return currentLevel;
}

bool GameState::isGameOver() const {
    return gameOver;
}

// Setters
void GameState::setBallsRemaining(uint8_t balls) {
    ballsRemaining = balls;
    gameOver = (ballsRemaining <= 0);
}

void GameState::setScore(uint32_t newScore) {
    score = newScore;
}

void GameState::setCurrentLevel(uint8_t level) {
    currentLevel = level;
}

void GameState::setGameOver(bool over) {
    gameOver = over;
}

// Game actions
void GameState::addPoints(uint32_t points) {
    score += points;
}

void GameState::useBall() {
    if (ballsRemaining > 0) {
        ballsRemaining--;
    }
    
    if (ballsRemaining <= 0) {
        gameOver = true;
    }
}

void GameState::resetGame() {
    ballsRemaining = 10;    // Default 10 balls
    score = 0;
    currentLevel = 1;
    gameOver = false;
}

void GameState::nextLevel() {
    currentLevel++;
    // You could add an extra ball as a reward for completing a level
    ballsRemaining++;
}