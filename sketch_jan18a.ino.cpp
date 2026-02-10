#include <Arduino.h>
#line 1 "C:\\Users\\jhal_\\AppData\\Local\\Temp\\.arduinoIDE-unsaved2026018-23076-fz55nz.o4syr\\sketch_jan18a\\sketch_jan18a.ino"
#include <Adafruit_NeoPixel.h>

// --- CONFIGURATION ---
#define PIN_ONBOARD    PIN_NEOPIXEL 

// GROUP 1: The Chain (Ring + First Strip section)
// Wired: Feather Pin 7 -> Ring IN -> Ring OUT -> Strip IN
#define PIN_CHAIN      7            
#define NUM_CHAIN      26           // 16 (Ring) + 10 (Strip 1) = 26 LEDs

// GROUP 2: The Separate Strip
// Wired: Feather Pin 10 -> Strip IN
#define PIN_SEPARATE   10           
#define NUM_SEPARATE   10           // Remaining 10 LEDs

// --- NEOPIXEL OBJECTS ---
Adafruit_NeoPixel onboard(1, PIN_ONBOARD, NEO_GRB + NEO_KHZ800);

// Both external items are RGBW
Adafruit_NeoPixel chain(NUM_CHAIN, PIN_CHAIN, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel separate(NUM_SEPARATE, PIN_SEPARATE, NEO_GRBW + NEO_KHZ800);

#line 23 "C:\\Users\\jhal_\\AppData\\Local\\Temp\\.arduinoIDE-unsaved2026018-23076-fz55nz.o4syr\\sketch_jan18a\\sketch_jan18a.ino"
void setup();
#line 44 "C:\\Users\\jhal_\\AppData\\Local\\Temp\\.arduinoIDE-unsaved2026018-23076-fz55nz.o4syr\\sketch_jan18a\\sketch_jan18a.ino"
void loop();
#line 23 "C:\\Users\\jhal_\\AppData\\Local\\Temp\\.arduinoIDE-unsaved2026018-23076-fz55nz.o4syr\\sketch_jan18a\\sketch_jan18a.ino"
void setup() {
  // 1. TURN OFF ONBOARD PIXEL
  onboard.begin();
  onboard.setBrightness(0);
  onboard.show();

  // 2. SETUP GROUP 1 (Chain of 26)
  chain.begin();
  chain.setBrightness(255);
  // Pure Blue: (Red=0, Green=0, Blue=255, White=0)
  chain.fill(chain.Color(0, 0, 255, 0)); 
  chain.show();

  // 3. SETUP GROUP 2 (Separate 10)
  separate.begin();
  separate.setBrightness(255);
  // Pure Blue
  separate.fill(separate.Color(0, 0, 255, 0)); 
  separate.show();
}

void loop() {
  delay(1000);
}
