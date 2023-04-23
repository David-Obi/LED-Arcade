#include "Snake.h"

Snake::Snake(CRGB *leds, LiquidCrystal& lcd) :
  Game(leds, &lcd, 'R', false, 0), 
  snakeColor(0, 255, 0), 
  appleColor(255, 0, 0), 
  boardColor(0, 0, 0), 
  bodyParts(1), 
  apple(MAX_ROWS / 2, MAX_COLS / 2),
  oldDirection('L')
{
}

bool Snake::loop() {
  bool resetGame = true;
  checkInput();
  EVERY_N_MILLISECONDS(400) {
    if (running) {
      showLCD();
      lightLEDs();
      move();
      oldDirection = direction; // use of this parameter prevents multiple moves in between animation
      checkApple();
      checkCollisions();
    } else {
      resetGame = endGame();
    }
  }
  delay(120); // debounce
  return resetGame;
}

void Snake::startGame() {
  setLedsToBlack();
  // Set random seed for apple generation
  randomSeed(analogRead(1));
  // set initial position of snake head
  snakePos[0].setRowCol(MAX_ROWS / 2, 0);
  apple.setRowCol(MAX_ROWS / 2, MAX_COLS / 2);
  occupiedBy[snakePos[0].get1DIndex(MAX_COLS)] = true;
  leds[apple.get1DIndex(MAX_COLS)] = appleColor;
  lightLEDs();
  running = true;
}

bool Snake::endGame() {
  lcd->clear();
  if (bodyParts == NUM_LEDS) {
    return chooseReset(true);
  } else {
    return chooseReset(false);
  }
}

bool Snake::chooseReset(bool won) {
  String restart = "";
  String choices[2] = {"Y", "N"};
  int index = 0;
  bool show = true;
  while(restart == "") {
    if (show && !won) {
      lcd->setCursor(0, 0);
      lcd->print("Game Over       ");
    }
    if (show && won) {
      lcd->setCursor(0, 0);
      lcd->print("You Win!!!      ");      
    }
    EVERY_N_MILLISECONDS(2500) {
      lcd->setCursor(0, 0);
      lcd->print("                ");
      lcd->setCursor(0, 0);
      lcd->print("Score: ");
      lcd->print(score);
      if (show) {
        show = false;
      } else {
        show = true;
      }
    }

    if (won) {
      confetti();
      FastLED.show();
      delay(50);
    } else {
      displayDeadSnake();
    }
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

void Snake::reset() {
  score = 0;
  bodyParts = 1;
  direction = 'R';
  startGame();
}

void Snake::displayDeadSnake() {
  //static int time = millis();
  int grid[MAX_ROWS][MAX_COLS] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 2, 2, 0, 0, 0},
    {0, 0, 2, 0, 0, 2, 0, 0},
    {0, 0, 2, 0, 0, 2, 0, 0},
  };
  for (int row = 0; row < MAX_ROWS; ++row) {
    for (int col = 0; col < MAX_COLS; ++col) {
      Cell c(row, col);
      if (grid[row][col] == 1) {
        leds[c.get1DIndex(MAX_COLS)] = CRGB::Red;
      } else if (grid[row][col] == 2) {
        leds[c.get1DIndex(MAX_COLS)] = CRGB::Black;
      } else {
        leds[c.get1DIndex(MAX_COLS)] = CRGB::Green;
      }      
    }
  }
  // // if (millis() - time < 250) {
  // //   // Frown animation
  // //   leds[34] = CRGB::Green;
  // //   leds[37] = CRGB::Green;
  // //   leds[42] = CRGB::Black;
  // //   leds[45] = CRGB::Black;
  // // }

  // if (millis() - time >= 250) {
  //   // Frown animation
  //   leds[34] = CRGB::Green;
  //   leds[37] = CRGB::Green;
  //   leds[42] = CRGB::Black;
  //   leds[45] = CRGB::Black;
  // }

  // if (millis() - time >= 500) {
  //   leds[50] = CRGB::Black;
  //   leds[53] = CRGB::Black;
    
  // }

  // if (millis() - time >= 1000) {
  //   time = 0;
  // }
  FastLED.show();
}

void Snake::lightLEDs() {
  for (int i = 0; i < bodyParts; ++i) {
    leds[snakePos[i].get1DIndex(MAX_COLS)] = (i != 0) ? snakeColor : CRGB(33, 255, 33);
  }
  FastLED.show();
}

void Snake::move() {
  Cell tail = snakePos[bodyParts - 1];

  for (int pos = bodyParts; pos > 0; --pos) {
    snakePos[pos] = snakePos[pos - 1];
  }

  switch(direction) {
    case 'U':
      snakePos[0].setRow(snakePos[0].getRow() - 1);
      break;
    case 'D':
      snakePos[0].setRow(snakePos[0].getRow() + 1);
      break;
    case 'L':
      snakePos[0].setCol(snakePos[0].getCol() - 1);
      break;
    case 'R':
      snakePos[0].setCol(snakePos[0].getCol() + 1);
      break;
  }
  int tailIndex = tail.get1DIndex(MAX_COLS);
  int headIndex = snakePos[0].get1DIndex(MAX_COLS);
  if (headIndex < NUM_LEDS) occupiedBy[headIndex] = true;
  if (headIndex != apple.get1DIndex(MAX_COLS)) occupiedBy[tailIndex] = false; // this space will be occupied if an apple is eaten since the snake grows backwards
  leds[tailIndex] = boardColor;
}

void Snake::newApple() {
  if (bodyParts != NUM_LEDS) {
    Cell possibleApplePos[NUM_LEDS - bodyParts];
    int index = 0;
    for (int row = 0; row < MAX_ROWS; ++row) {
      for (int col = 0; col < MAX_COLS; ++col) {
        Cell newApple(row, col);
        if (!occupiedBy[newApple.get1DIndex(MAX_COLS)]) {
          possibleApplePos[index++] = newApple;          
        }
      }
    }
    apple = possibleApplePos[random(index)];
    leds[apple.get1DIndex(MAX_COLS)] = appleColor; // light new apple
  }
}

void Snake::checkApple() {
  if (snakePos[0].getRow() == apple.getRow() && 
      snakePos[0].getCol() == apple.getCol()) {
    ++score;
    ++bodyParts;
    newApple();
  }
}

void Snake::checkCollisions() {
  for (int i = bodyParts - 1; i > 0; --i) {
    if (snakePos[0].getRow() == snakePos[i].getRow() && 
        snakePos[0].getCol() == snakePos[i].getCol()) {
      running = false;
    }
  }

  if (snakePos[0].getRow() < 0 ||
      snakePos[0].getRow() >= MAX_ROWS ||
      snakePos[0].getCol() < 0 ||
      snakePos[0].getCol() >= MAX_COLS) {
    running  = false;
  }
}

void Snake::checkInput() {
  int button = readLCDButtons();
  if (running) {
    switch(button) {
      case btnLEFT:
        if (oldDirection != 'R') direction = 'L';
        break;
      case btnRIGHT:
        if (oldDirection != 'L') direction = 'R';
        break;
      case btnUP:
        if (oldDirection != 'D') direction = 'U';
        break;
      case btnDOWN:
        if (oldDirection != 'U') direction = 'D';
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
      case btnNONE:
        // DO NOTHING
        break;
    }
  }
}

void Snake::showLCD() {
  lcd->setCursor(0, 0);
  lcd->print("Direction: ");
  lcd->setCursor(11,0);
  lcd->print(direction);
  lcd->setCursor(0, 1);
  lcd->print("Apples: ");
  lcd->setCursor(9, 1);
  lcd->print(score);
}