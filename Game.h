#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include <FastLED.h>
#include <LiquidCrystal.h>

#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

#define MAX_ROWS 7
#define MAX_COLS 8
#define NUM_LEDS (MAX_ROWS * MAX_COLS)

class Game {
  public:
    Game(CRGB *leds, LiquidCrystal* lcd, char d, bool r, int s) : leds(leds), lcd(lcd), direction(d), running(r), score(s) {};
    virtual ~Game() = default;
    virtual bool loop() = 0;
    virtual void startGame() = 0; // Equivalent to setup()
    virtual bool endGame() = 0;
    virtual void lightLEDs();
    virtual void showLCD() = 0;
    // virtual void reset() = 0;
    virtual int readLCDButtons() {
      int adcKeyIn = analogRead(0);
      // Approx button values are 0, 100, 256, 410, 640, 
      if (adcKeyIn > 1000) return btnNONE;
      if (adcKeyIn < 50) return btnRIGHT;
      if (adcKeyIn < 195) return btnUP;
      if (adcKeyIn < 380) return btnDOWN;
      if (adcKeyIn < 555) return btnLEFT;
      if (adcKeyIn < 790) return btnSELECT;
      return btnNONE;
    }
    virtual void checkInput() = 0;
    virtual void confetti() {
      static uint8_t gHue = 0;
      EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
      // random colored speckles that blink in and fade smoothly
      fadeToBlackBy( leds, NUM_LEDS, 10);
      int pos = random16(NUM_LEDS);
      leds[pos] += CHSV( gHue + random8(64), 200, 255);
    }

    virtual void setLedsToBlack() {
      for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = CRGB::Black;
      }
      FastLED.show();
    }

    CRGB *leds;
    LiquidCrystal *lcd;
    char direction;
    bool running;
    int score;
};

#endif // GAME_H