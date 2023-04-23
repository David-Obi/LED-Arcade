#include "Tetris.h"

constexpr int Tetris::points[5];
constexpr int Tetris::levelSpeed[10];

Tetris::Tetris(CRGB *leds, LiquidCrystal& lcd) :
Game(leds, &lcd, 'D', false, 0), totalLines(0), level(0), speed(1500)
{
}

bool Tetris::loop() {
  bool resetGame = true;
  showLCD();
  movePiece();
  EVERY_N_MILLISECONDS_I(thisTimer, speed) { 
    running = drop();
    thisTimer.setPeriod(speed);
  } 
  lightLEDs();
  if (!running) {
    resetGame = endGame();
  }
  delay(120);
  return resetGame;
}

void Tetris::startGame() {
  emptyGrid();
  lightLEDs();
  running = true;
}

bool Tetris::endGame() {
  lcd->clear();
  return chooseReset();
}

bool Tetris::chooseReset() {
  String restart = "";
  String choices[2] = {"Y", "N"};
  int index = 0;
  bool show = true;
  while(restart == "") {
    if (show) {
      lcd->setCursor(0, 0);
      lcd->print("Game Over       ");
    }
    EVERY_N_MILLISECONDS(2500) {
      lcd->setCursor(0, 0);
      lcd->print("Score: ");
      lcd->print(score);
      if (show) {
        show = false;
      } else {
        show = true;
      }
    }
      // display score and blink lcd and then ask about restart
    confetti();
    FastLED.show();
    delay(12);
    lcd->setCursor(0, 1);
    lcd->print("Restart:  Y  N");
    if ((((index % 2) + 2) % 2) == 0) {
      lcd->setCursor(9, 1);
      lcd->print(">");
    } else {
      lcd->setCursor(12, 1);
      lcd->print(">");
    }
    EVERY_N_MILLISECONDS(200) { 
    int button = readLCDButtons();
      switch(button) {
        case btnLEFT:
            --index;
          break;
        case btnRIGHT:
          ++index;
          break;
        case btnSELECT:
          restart = choices[((index % 2) + 2) % 2];
          break;
      }
    }
  }
  if (restart == "Y") {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("Resetting Game");
    delay(1500);
    reset();
    lcd->clear();
    return true;
  } else {
    lcd->clear();
    return false;
  }
}

void Tetris::reset() {
  emptyGrid();
  getNewPiece();
  level = 0;
  totalLines = 0;
  score = 0;
  speed = levelSpeed[0];
  running = true;
}

void Tetris::movePiece() {
  checkInput();
  if (direction != 'D') {
    Piece p = move();
    if (valid(p)) {
      piece.unDraw(leds);
      if (direction == 'H') {
        while (valid(p)) {
          score += points[4];
          piece.move(p);
          p = move();
        }
        piece.hardDropped = true;
      } else {
        piece.move(p);
      }
    }
  }  
}

bool Tetris::drop() { 
  direction = 'D';
  Piece p = move();
  if (valid(p)) {
    piece.unDraw(leds);
    piece.move(p);
  } else {
    freeze();
    clearLines();
    // if (piece.pos.getRow() == 0 && (piece.pos.getCol() >= 2 && piece.pos.getCol() <= 4)) return false;
    delay(10);
    getNewPiece();
  }
  return valid(piece);
}

void Tetris::freeze() {
  for (int row = 0; row < piece.length; ++row) {
    for (int col = 0; col < piece.length; ++col) {
      int value = piece.shape[row][col];
      if (value != 0) {
        int shapeRow = piece.pos.getRow() + row;
        int shapeCol = piece.pos.getCol() + col;
        grid[shapeRow][shapeCol] = value;
      }
    }
  }  
}

void Tetris::getNewPiece() {
  Piece newPiece;
  piece = newPiece;
}

Piece Tetris::move() {
  Piece copy = piece;
    switch(direction) {
    case 'U':
      rotate(copy);
      break;
    case 'D':
      copy.pos.setRow(copy.pos.getRow() + 1);
      break;
    case 'H':
      copy.pos.setRow(copy.pos.getRow() + 1);
      break;
    case 'L':
      copy.pos.setCol(copy.pos.getCol() - 1);
      break;
    case 'R':
      copy.pos.setCol(copy.pos.getCol() + 1);
      break;
  }
  return copy;
}

void Tetris::clearLines() {
  int newGrid[MAX_ROWS][MAX_COLS] = {0};
  bool lineIsFull[MAX_ROWS];
  int lines = 0;
  int currentRow = MAX_ROWS - 1;

  // this checks the number of rows that are full
  // and creates a new grid with rows shifted downwards for cleared rows
  for(int row = MAX_ROWS - 1; row >= 0; --row) {
    lineIsFull[row] = true;
    for (int col = 0; col < MAX_COLS; ++col) {
      if (grid[row][col] == 0) lineIsFull[row] = false;
    }
    for (int col = 0; col < MAX_COLS; ++col) {
      if (lineIsFull[row]) {
        ++lines;
        break;
      }
      newGrid[currentRow][col] = grid[row][col];
    }
    if (!lineIsFull[row]) --currentRow;
  }
  
  if (lines > 0) {
    // add points to score
    score += getLinesClearedPoints(lines);
    totalLines += lines;
    bool newLevel = false;
    if (totalLines >= LINES_PER_LEVEL) {
      newLevel = true;
      ++level;
      totalLines -= LINES_PER_LEVEL;
      // display something to lcd
      lcd->clear();
      lcd->setCursor(0, 0);
      lcd->print("You Are Now On");
      lcd->setCursor(0, 1);
      lcd->print("Level ");
      lcd->print(level);
      lcd->print("!");
      // add buzzer sound!
      speed = levelSpeed[(level < 10) ? level : 9];
    }
    // do animation
    lightLEDs();
    animation(lineIsFull);
    copyGrid(newGrid);
    if (newLevel) lcd->clear();
  }
}

void Tetris::animation(bool lines[MAX_ROWS])
{
  for (int col = 0; col < MAX_COLS; ++col) {
    for (int row = 0; row < MAX_ROWS; ++row) {
      if (lines[row]) leds[row * MAX_COLS + col] = CRGB::White;
    }
    FastLED.show();
    FastLED.delay(50);
  }

  for (int col = 0; col < MAX_COLS; ++col) {
    for (int row = 0; row < MAX_ROWS; ++row) {
      if (lines[row]) leds[row * MAX_COLS + col] = CRGB::Black;
    }
    FastLED.show();
    FastLED.delay(50);
  }
}

int Tetris::getLinesClearedPoints(int lines) { // only called if lines > 0
  int lineClearPoints = 0;
  if (lines <= 4) {
    lineClearPoints = points[lines - 1];
  } else {
    lineClearPoints = points[3];
  }
  return (level + 1) * lineClearPoints;
}

void Tetris::drawGrid() {
  for (int row = 0; row < MAX_ROWS; ++row) {
    for (int col = 0; col < MAX_COLS; ++col) {
      Cell c(row, col);
      leds[c.get1DIndex(MAX_COLS)] = Piece::getColor(grid[row][col]);
    }
   }
}

void Tetris::checkInput() {
  int button = readLCDButtons();
  if (running) {
    switch(button) {
      case btnLEFT:
        direction = 'L';
        break;
      case btnRIGHT:
        direction = 'R';
        break;
      case btnUP:
        direction = 'U';
        break;
      case btnDOWN:
        direction = 'H'; // H for Hard Drop
        break;
      case btnSELECT:
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("Paused: Press");
        lcd->setCursor(0, 1);
        lcd->print("Select to Play");
        delay(200);
        while (true) {
          if (readLCDButtons() == btnSELECT) {
            delay(200);
            lcd->clear();
            break;
          }
        }
        break;
      default:
        direction = 'D';
        break;
    }
  }
}

void Tetris::showLCD() {
  lcd->setCursor(0, 0);
  lcd->print("Level: ");
  lcd->setCursor(7,0);
  lcd->print(level);
  lcd->setCursor(0, 1);
  lcd->print("Score: ");
  lcd->setCursor(7, 1);
  lcd->print(score);
}

void Tetris::emptyGrid() {
  for (int row = 0; row < MAX_ROWS; ++row) {
    for (int col = 0; col < MAX_COLS; ++col) {
      grid[row][col] = 0;
    }
  }
}

void Tetris::copyGrid(const int newGrid[][MAX_COLS]) {
  for (int row = 0; row < MAX_ROWS; ++row) {
    for (int col = 0; col < MAX_COLS; ++col) {
      grid[row][col] = newGrid[row][col];
    }
  }
}

bool Tetris::valid(Piece &p) {
  for (int row = 0; row < p.length; ++row) {
    for (int col = 0; col < p.length; ++col) {
      if (p.shape[row][col] != 0) {
        int shapeRow = p.pos.getRow() + row;
        int shapeCol = p.pos.getCol() + col;
        if (!insideGrid(shapeRow, shapeCol)) return false;
        if (!notOccupied(shapeRow, shapeCol)) return false;
      }
    }
  }
  return true;
}

bool Tetris::insideGrid(int row, int col) {
  return (col >= 0 && col < MAX_COLS) && (row >= 0 && row < MAX_ROWS);
}

bool Tetris::notOccupied(int row, int col) {
  return grid[row][col] == 0;
}

void Tetris::rotate(Piece &p) {
  // Transpose the matrix
  if (!piece.hardDropped) { // stops unnecessary work
    for (int row = 0; row < p.length; ++row) {
      for (int col = 0; col < row; ++col) {
        int temp = p.shape[row][col];
        p.shape[row][col] = p.shape[col][row];
        p.shape[col][row] = temp;
      }
    }
    // Reverse the rows
    for (int row = 0; row < p.length; ++row) {
      for (int col = 0; col < p.length / 2; ++col) {
        int temp = p.shape[row][col];
        p.shape[row][col] = p.shape[row][p.length - col - 1];
        p.shape[row][p.length - col - 1] = temp;
      }
    }
  }
}

void Tetris::lightLEDs() {
  drawGrid();
  piece.draw(leds);
  FastLED.show();
}

void Tetris::printGrid() {
  for (int row = 0; row < MAX_ROWS; ++row) {
    for (int col = 0; col < MAX_COLS; ++col) {
      Serial.print(grid[row][col]);      
    }
    Serial.println("");
  }
  Serial.println("");
}

void printPiece(const Piece &p) {
  for (int row = 0; row < p.length; ++row) {
    for (int col = 0; col < p.length; ++col) {
      Serial.print(p.shape[row][col]);
    }
    Serial.println("");
  }
  Serial.println("");
}