#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

// --- CONFIGURATION ---
#define PIN_ONBOARD    PIN_NEOPIXEL 

// GROUP 1: The Chain (Ring + First Strip section)
// Wired: Feather Pin 7 -> Ring IN -> Ring OUT -> Strip IN
#define PIN_CHAIN      7            
#define NUM_CHAIN      26           // 16 (Ring) + 10 (Strip 1) = 26 LEDs
#define NUM_RING       16           // First 16 LEDs in chain are the ring
#define NUM_STRIP1     10           // Next 10 LEDs in chain are first strip

// GROUP 2: The Separate Strip
// Wired: Feather Pin 10 -> Strip IN
#define PIN_SEPARATE   10           
#define NUM_SEPARATE   10           // Remaining 10 LEDs

// --- PATTERN CONFIGURATION ---
#define NUM_PATTERNS   3            // Total number of patterns
#define EEPROM_ADDR    0            // EEPROM address for pattern storage
#define MIN_BRIGHTNESS 13           // 5% floor brightness (13/255 = 0.05)
#define MAX_BRIGHTNESS 255          // 100% brightness

// --- NEOPIXEL OBJECTS ---
Adafruit_NeoPixel onboard(1, PIN_ONBOARD, NEO_GRB + NEO_KHZ800);

// Both external items are RGBW
Adafruit_NeoPixel chain(NUM_CHAIN, PIN_CHAIN, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel separate(NUM_SEPARATE, PIN_SEPARATE, NEO_GRBW + NEO_KHZ800);

// --- PATTERN STATE VARIABLES ---
uint8_t currentPattern = 0;
unsigned long lastUpdate = 0;
float pulsePhase = 0.0;
int ringPosition = 0;
int strip1Position = 0;
int strip2Position = 0;
int strip1Direction = 1;
int strip2Direction = 1;

void setup() {
  // 1. TURN OFF ONBOARD PIXEL
  onboard.begin();
  onboard.setBrightness(0);
  onboard.show();

  // 2. INITIALIZE EEPROM EMULATION
  EEPROM.begin(256);
  
  // 3. READ CURRENT PATTERN FROM EEPROM
  currentPattern = EEPROM.read(EEPROM_ADDR);
  
  // 4. ADVANCE TO NEXT PATTERN
  currentPattern = (currentPattern + 1) % NUM_PATTERNS;
  
  // 5. SAVE NEW PATTERN TO EEPROM
  EEPROM.write(EEPROM_ADDR, currentPattern);
  EEPROM.commit();

  // 6. SETUP GROUP 1 (Chain of 26)
  chain.begin();
  chain.setBrightness(255);

  // 7. SETUP GROUP 2 (Separate 10)
  separate.begin();
  separate.setBrightness(255);
  
  // 8. INITIALIZE PATTERN
  if (currentPattern == 0) {
    // Pattern 0: Solid Blue
    chain.fill(chain.Color(0, 0, 255, 0)); 
    chain.show();
    separate.fill(separate.Color(0, 0, 255, 0)); 
    separate.show();
  }
  // Other patterns will be handled in loop()
}

void loop() {
  switch (currentPattern) {
    case 0:
      patternSolidBlue();
      break;
    case 1:
      patternPulsingBlue();
      break;
    case 2:
      patternRotatingBouncing();
      break;
  }
}

// PATTERN 0: Solid Blue (static, set in setup)
void patternSolidBlue() {
  delay(1000);
}

// PATTERN 1: Pulsing Blue (20% to 100% brightness)
void patternPulsingBlue() {
  unsigned long currentTime = millis();
  
  // Update every 20ms for smooth animation
  if (currentTime - lastUpdate >= 20) {
    lastUpdate = currentTime;
    
    // Advance pulse phase (0 to 2*PI)
    pulsePhase += 0.05;
    if (pulsePhase >= 2 * PI) {
      pulsePhase = 0;
    }
    
    // Calculate brightness using sine wave
    // sin(phase) ranges from -1 to 1, we map to MIN_BRIGHTNESS to MAX_BRIGHTNESS
    float sineValue = sin(pulsePhase);
    uint8_t brightness = MIN_BRIGHTNESS + ((sineValue + 1.0) / 2.0) * (MAX_BRIGHTNESS - MIN_BRIGHTNESS);
    
    // Apply to all LEDs
    chain.setBrightness(brightness);
    chain.fill(chain.Color(0, 0, 255, 0));
    chain.show();
    
    separate.setBrightness(brightness);
    separate.fill(separate.Color(0, 0, 255, 0));
    separate.show();
  }
}

// PATTERN 2: Rotating/Bouncing Blue (groups of 2-3 LEDs with 20% floor)
void patternRotatingBouncing() {
  unsigned long currentTime = millis();
  
  // Update every 100ms for visible movement
  if (currentTime - lastUpdate >= 100) {
    lastUpdate = currentTime;
    
    // Set all LEDs to floor brightness first
    chain.setBrightness(255);
    for (int i = 0; i < NUM_CHAIN; i++) {
      chain.setPixelColor(i, chain.Color(0, 0, MIN_BRIGHTNESS, 0));
    }
    
    separate.setBrightness(255);
    for (int i = 0; i < NUM_SEPARATE; i++) {
      separate.setPixelColor(i, separate.Color(0, 0, MIN_BRIGHTNESS, 0));
    }
    
    // RING: Rotate 3-LED group (center bright, neighbors medium)
    for (int offset = -1; offset <= 1; offset++) {
      int pos = (ringPosition + offset + NUM_RING) % NUM_RING;
      uint8_t brightness = (offset == 0) ? MAX_BRIGHTNESS : (MAX_BRIGHTNESS + MIN_BRIGHTNESS) / 2;
      chain.setPixelColor(pos, chain.Color(0, 0, brightness, 0));
    }
    
    // STRIP 1: Bounce 3-LED group
    for (int offset = -1; offset <= 1; offset++) {
      int pos = strip1Position + offset;
      if (pos >= 0 && pos < NUM_STRIP1) {
        uint8_t brightness = (offset == 0) ? MAX_BRIGHTNESS : (MAX_BRIGHTNESS + MIN_BRIGHTNESS) / 2;
        chain.setPixelColor(NUM_RING + pos, chain.Color(0, 0, brightness, 0));
      }
    }
    
    // STRIP 2: Bounce 3-LED group
    for (int offset = -1; offset <= 1; offset++) {
      int pos = strip2Position + offset;
      if (pos >= 0 && pos < NUM_SEPARATE) {
        uint8_t brightness = (offset == 0) ? MAX_BRIGHTNESS : (MAX_BRIGHTNESS + MIN_BRIGHTNESS) / 2;
        separate.setPixelColor(pos, separate.Color(0, 0, brightness, 0));
      }
    }
    
    chain.show();
    separate.show();
    
    // Advance positions
    ringPosition = (ringPosition + 1) % NUM_RING;
    
    strip1Position += strip1Direction;
    if (strip1Position >= NUM_STRIP1 - 1) {
      strip1Position = NUM_STRIP1 - 1;
      strip1Direction = -1;
    } else if (strip1Position <= 0) {
      strip1Position = 0;
      strip1Direction = 1;
    }
    
    strip2Position += strip2Direction;
    if (strip2Position >= NUM_SEPARATE - 1) {
      strip2Position = NUM_SEPARATE - 1;
      strip2Direction = -1;
    } else if (strip2Position <= 0) {
      strip2Position = 0;
      strip2Direction = 1;
    }
  }
}
