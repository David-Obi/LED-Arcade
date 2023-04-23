#include "Cell.h"

Cell::Cell(int r, int c) : row(r), col(c)
{}

Cell::Cell(const Cell& src) : row(src.row), col(src.col)
{}

Cell& Cell::operator=(const Cell &rhs) {
  if (this != &rhs) {
    row = rhs.row;
    col = rhs.col;
  }
  return *this;
}

bool Cell::operator==(const Cell &rhs) {
  return row == rhs.row && col == rhs.col;
}

void Cell::setRow(const int r) {
  row = r;
}

void Cell::setCol(const int c) {
  col = c;
}

void Cell::setRowCol(const int r, const int c) {
  row = r;
  col = c;
}

int Cell::getRow() {
  return row;
}

int Cell::getCol() {
  return col;
}

int Cell::get1DIndex(const int maxCols) {
  return row * maxCols + col;
}