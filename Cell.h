#ifndef CELL_H
#define CELL_H

class Cell {
  public:
    Cell(int r = 0, int c = 0);
    Cell(const Cell& src);
    Cell& operator=(const Cell& rhs);
    bool operator==(const Cell& rhs);
    void setRow(const int r);
    void setCol(const int c);
    void setRowCol(const int r, const int c);
    int getRow();
    int getCol();
    int get1DIndex(const int maxCols);

  private:
    int row;
    int col;
};

#endif // CELL_H