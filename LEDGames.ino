#include "Snake.h"
#include "Tetris.h"

#define LED_PIN 3
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define FRAMES_PER_SECOND  120

CRGB leds[NUM_LEDS];

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
uint8_t brightness = 130;

// use polymorphism to choose game at runtime
Game* game = nullptr;

void setup() {
  // Delay for Startup
  delay(1000);

  // Setup the LCD display
  lcd.begin(16, 2);
  
  // set up leds for snake
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightness);
  FastLED.show(); // Run this to clear any old leds from the board

  // Choose game
  chooseAndStartGame();

  // Set serial output for debugging
  Serial.begin(9600);
}

void loop() {
  bool playSameGame = game->loop();

  if (!playSameGame) {
    for (int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = CRGB::Black;
      FastLED.show();
      FastLED.delay(25);
    }
    delete game; // deallocate memory
    game = nullptr;
    chooseAndStartGame();
  }
}

void chooseAndStartGame() {
  chooseGame();
  game->startGame();
  lcdStartGame();
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void chooseGame() {
  String chosenGame = "";
  int numGames = 3;
  String games[numGames] = {"Tetris    ", "Snake     ", "Settings  "};
  bool onSettings = false;
  int index = 0;
  while(chosenGame == "") {
    //---------------------Code to run FASTLED DemoReel100------------------------------------------
    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();

    // send the 'leds' array out to the actual LED strip
    FastLED.show();  
    // insert a delay to keep the framerate modest
    FastLED.delay(1000/FRAMES_PER_SECOND); 

    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
    //----------------------------------------------------------------------------------------------
    if (!onSettings) {
      lcd.setCursor(0, 0);
      lcd.print("Choose Game (3):");
      lcd.setCursor(0, 1);
      lcd.print("> ");
      lcd.setCursor(2, 1);
      lcd.print(games[((index % numGames) + numGames) % numGames]);
      lcd.setCursor(13, 1);
      lcd.print("-->");
    } else {
      lcd.setCursor(0, 0);
      lcd.print("Brightness:     ");
      lcd.setCursor(0, 1);
      lcd.print("> ");
      if (((index % 2) + 2) % 2 == 0) {
        lcd.print(brightness);
        lcd.print("      ");
      } else {
        lcd.print("Exit     ");
      }
      lcd.setCursor(13, 1);
      lcd.print("-->");        
    }
    EVERY_N_MILLISECONDS( 200 ) {
      int button = readLCDButtons();
      switch(button) {
        case btnLEFT:
          --index;
          break;
        case btnRIGHT:
          ++index;
          break;
        case btnUP:
          if (onSettings && ((index % 2) + 2) % 2 == 0) {
            (brightness != 255) ? brightness += 5 : brightness = 0;
            FastLED.setBrightness(brightness);
          }
          break;
        case btnDOWN:
          if (onSettings && ((index % 2) + 2) % 2 == 0) {
            (brightness != 0) ? brightness -= 5 : brightness = 255;
            FastLED.setBrightness(brightness);
          }
          break;          
        case btnSELECT:
          if (!onSettings) {
            chosenGame = games[((index % numGames) + numGames) % numGames];
            if (chosenGame == "Settings  ") { // Switch to settings menu
              onSettings = true;
              chosenGame = "";              
            }
          } else {
            if (((index % 2) + 2) % 2 == 1) { // Exit Settings back to game menu
              onSettings = false;
              index = 0;
            }
          }
          break;
      }
    }
  }

  if (chosenGame == games[0]) { // Tetris
    game = new Tetris(leds, lcd);
  } else if (chosenGame == games[1]) { // Snake
    game = new Snake(leds, lcd);
    // add code to chooseSpeed
  }
  // add code to chooseTheme
}

void lcdStartGame() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Starting Game: ");
  lcd.setCursor(0, 1);
  lcd.print("3...............");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print(".......2........");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("...............1");
  delay(1000);
  lcd.clear();
}

int readLCDButtons() {
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

// ------------------------------------------------------------------------------------------------------
// FastLED DemoReel100

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
