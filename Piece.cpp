#include "Piece.h"

constexpr int Piece::shape1[][3];
constexpr int Piece::shape2[][3];
constexpr int Piece::shape3[][3];
constexpr int Piece::shape4[][3];
constexpr int Piece::shape5[][3];
constexpr int Piece::shape6[][3];
constexpr int Piece::shape7[][3];
constexpr int Piece::shape8[][3];

Piece::Piece() : typeId(randomizeTetrominoType()), pos(), color(getColor(typeId)), length(getLength()), hardDropped(false) {
  random16_set_seed(analogRead(1));
  setStartingPos();
  setShape();
}

void Piece::move(Piece p) {
  if (!hardDropped) {
    pos.setRow(p.pos.getRow());
    pos.setCol(p.pos.getCol()); 
    copyShape(p.shape);   
  }
}

void Piece::draw(CRGB *leds) {
  draw(leds, color);
}

void Piece::draw(CRGB *leds, CRGB color) {
  for (int row = 0; row < length; ++row) {
    for (int col = 0; col < length; ++col) {
      if (shape[row][col] > 0) {
        Cell c(pos.getRow() + row, pos.getCol() + col);
        leds[c.get1DIndex(MAX_COLS)] = color;
      }
    }
  }
}

void Piece::unDraw(CRGB *leds) {
  draw(leds, CRGB(0, 0, 0));
}

int Piece::randomizeTetrominoType() {
  random16_set_seed(random8()); // was running into issues with random number generator so I used this
  // try a large number and modulo 8 it. 
  int rand = random8(1, 9);
  // Serial.print("RAND: ");
  // Serial.println(rand);
  // Serial.println("");
  return rand;
}

void Piece::setShape(){
  if (typeId == 1) {
    copyShape(shape1);
  } else if (typeId == 2) {
    copyShape(shape2);
  } else if (typeId == 3) {
    copyShape(shape3);
  } else if (typeId == 4) {
    copyShape(shape4);
  } else if (typeId == 5) {
    copyShape(shape5);
  } else if (typeId == 6) {
    copyShape(shape6);
  } else if (typeId == 7) {
    copyShape(shape7);
  } else if (typeId == 8) {
    copyShape(shape8);
  } else {
    // some error
  }

}

void Piece::copyShape(const int newShape[][3]) {
  for (int row = 0; row < length; ++row) {
    for (int col = 0; col < length; ++col) {
      shape[row][col] = newShape[row][col];
    }
  }
}

CRGB Piece::getColor(int id) {
  if (id == 1) return CRGB(0, 255, 255);
  if (id == 2) return CRGB(0, 0, 255);
  if (id == 3) return CRGB(255, 127, 0);
  if (id == 4) return CRGB(255, 255, 0);
  if (id == 5) return CRGB(0, 255, 0);
  if (id == 6) return CRGB(128, 0, 128);
  if (id == 7) return CRGB(255, 0, 0);
  if (id == 8) return CRGB(127, 127, 127);
  return CRGB(0, 0, 0);
}

int Piece::getLength() {
  if (typeId == 8) {
    return 1;
  } else if (typeId == 4) {
    return 2;
  } else {
    return 3;
  }
}

void Piece::setStartingPos() {
  pos.setCol(typeId == 4 || typeId == 8 || typeId == 3 ? 3 : 2);
}