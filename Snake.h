#ifndef SNAKE_H
#define SNAKE_H

#include "Game.h"
#include "Cell.h"

class Snake : public Game { // FIXME: Change to private?
  public:
    Snake(CRGB *leds, LiquidCrystal& lcd); // FIXME: Try const reference
    virtual ~Snake() = default;
    virtual bool loop();
    virtual void startGame();

  private:
    virtual bool endGame();
    virtual void reset();
    bool chooseReset(bool won);
    virtual void lightLEDs();
    void move();
    void newApple();
    void checkApple();
    void checkCollisions();
    virtual void checkInput();
    virtual void showLCD();
    void displayDeadSnake();
    
    CRGB snakeColor;
    CRGB appleColor;
    CRGB boardColor;
    int bodyParts;
    Cell snakePos[NUM_LEDS];
    bool occupiedBy[NUM_LEDS];
    Cell apple;
    char oldDirection;
};

#endif // SNAKEGAME_H