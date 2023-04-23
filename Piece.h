#ifndef PIECE_H
#define PIECE_H

#include "Cell.h"
#include "Game.h"

class Piece {
  public:
    Piece();
    Piece(const Piece&) = default;
    Piece& operator=(const Piece&) = default;
    void draw(CRGB *leds);
    void draw(CRGB *leds, CRGB col);
    void unDraw(CRGB *leds);
    void move(Piece p);
    static CRGB getColor(int id);
  
    int typeId;
    Cell pos;
    CRGB color;
    int length;
    int shape[3][3];
    bool hardDropped;

  private:
    static const int NUM_PIECES = 8;
    int randomizeTetrominoType();
    void setShape();
    void copyShape(const int newShape[][3]);
    void setStartingPos();
    int getLength();

    static constexpr int shape1[3][3] = {
      {0, 0, 0}, 
      {1, 1, 1},
      {0, 0, 0}
    };
    static constexpr int shape2[3][3] = {
      {0, 2, 0}, 
      {0, 2, 2},
      {0, 0, 0}
    };
    static constexpr int shape3[3][3] = {
      {0, 3, 0}, 
      {3, 3, 0},
      {0, 0, 0}
    };
    static constexpr int shape4[3][3] = {
      {4, 4, 0}, 
      {4, 4, 0},
      {0, 0, 0}
    };

    static constexpr int shape5[3][3] = {
      {0, 5, 5}, 
      {5, 5, 0},
      {0, 0, 0}
    };

    static constexpr int shape6[3][3] = {
      {0, 6, 0}, 
      {6, 6, 6},
      {0, 0, 0}
    };

    static constexpr int shape7[3][3] = {
      {7, 7, 0}, 
      {0, 7, 7},
      {0, 0, 0}
    };

    static constexpr int shape8[3][3] = {
      {8, 0, 0}, 
      {0, 0, 0},
      {0, 0, 0}
    };
};

#endif // PIECE_H