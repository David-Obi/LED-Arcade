#ifndef TETRIS_H
#define TETRIS_H

#define LINES_PER_LEVEL 4

#include "Game.h"
#include "Piece.h"

class Tetris : public Game { // FIXME: Change to private?
  public:
    Tetris(CRGB *leds, LiquidCrystal& lcd); // FIXME: Try const reference
    virtual ~Tetris() = default;
    virtual bool loop();
    virtual void startGame();
    
  private:
    // Functions
    void movePiece();
    bool drop();
    Piece move();
    virtual bool endGame();
    virtual void reset();
    bool chooseReset();
    virtual void lightLEDs();
    virtual void checkInput();
    virtual void showLCD();
    void clearLines();
    void animation(bool lines[MAX_ROWS]);
    bool valid(Piece &p);
    bool insideGrid(int row, int col);
    bool notOccupied(int row, int col);
    void rotate(Piece& p);
    void freeze();
    void getNewPiece();
    void emptyGrid();
    void copyGrid(const int newGrid[][MAX_COLS]);
    void drawGrid();
    void printGrid();
    void printPiece(const Piece& p);
    int getLinesClearedPoints(int lines);

    // Variables
    Piece piece;
    int grid[MAX_ROWS][MAX_COLS];
    int totalLines;
    int level;
    int speed;
    static constexpr int points[5] = {
      100, // 1 line
      300, // 2 lines
      500, // 3 lines
      800, // 4 lines
      2    // hard dropped
    };

    static constexpr int levelSpeed[10] = {
      1500,
      1400,
      1300,
      1200,
      1100,
      1000,
      900,
      800,
      700,
      650
    };
};

#endif // TETRIS_H