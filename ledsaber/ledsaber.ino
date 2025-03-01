/**
 * Based on tutorial found here
 * https://www.instructables.com/USB-NeoPixel-Deco-Lights-via-Digispark-ATtiny85/
*/
#include <Adafruit_NeoPixel.h>

// Serial based on this sketch https://wokwi.com/projects/379145004201553921
// #include <SoftwareSerial.h>
// #include <Arduino.h>
// #include <U8x8lib.h>
// U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock */ 4, /* data */ 3, /* reset */ U8X8_PIN_NONE);
// SoftwareSerial Serial(PB0, PB1);

#define LEDPIN 1 // LED data pin
#define NUMPIXELS 9
#define RNDPIN 2 // set to any pin with analog input (it is used only once to initialize random number generator)
#define BRIGHTNESS 64 // 0 - 255
#define FOCUS 65 // shape of color spots (increase to get narrow spots, decrease to get wider spots) (No idea what this actually means)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRBW + NEO_KHZ800);

/* ################ CLICK HANDLER ################ */
#define BTN_PIN 0
#define SHORT_PRESS_BREAKPOINT 250 // 250 ms (too long/short??)
#define BTN_PRESS_RESET_TIMEOUT 400 // 400 ms
int sequentialPresses = 0;
int lastPressTime = 0;
enum LightMode { OFF, DOUBLE, TRIPLE, LOOONG }; // TODO - Update with actual modes when know what they are
enum LightMode currentLightMode = OFF;
void handleBtnPress();
void resetClickStates();

/* ################ STING ANIMATION ################ */ // Could move this to separate file?
int STING_LIGHT = strip.Color(183, 251, 253);
int STING_DARK = strip.Color(102, 158, 173);
// int STING_DARK = strip.Color(255, 0, 0);
void glimmer();
void pulse();
void lightsaber();

void setup() {
  randomSeed(analogRead((RNDPIN)));

  /*
  // Initialize Serial
  Serial.begin(9600); // Must be 9600
  Serial.println("Hello Arduino\n");
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  */

  // Initialize LED strip
  strip.begin();
  // STING_LIGHT = strip.Color(183, 251, 253);
  // STING_DARK = strip.Color(102, 158, 173);
  STING_LIGHT = strip.Color(109, 228, 247);
  STING_DARK = strip.Color(39, 216, 245);
  /*strip.setPixelColor(0, 0, 0, 255);
  strip.setPixelColor(1, 0, 255, 0);
  strip.setPixelColor(2, 255, 0, 0);*/
  strip.show();

  pinMode(BTN_PIN, INPUT_PULLUP);
}

const int speed = 10;
int current = 255;
int direction = 1; // 1 || -1
int lastLEDUpdate = 0;

void loop() {

  handleBtnPress();

  #if 1
    current += (speed*direction);
    if (current <= 0) {
      current = 0;
      direction = 1;
    } else if (current >= 255) {
      current = 255;
      direction = -1;
    }
  #endif

  #if 0
    strip.setPixelColor(0, 0, 0, current);
    strip.setPixelColor(1, 0, current, 0);
    strip.setPixelColor(1, current, 0, 0);
    strip.show();
    delay(100);
  #else

    switch (currentLightMode) {
      case DOUBLE:
        // u8x8.drawString(0, 1, "DOUBLE");
        // strip.setPixelColor(0, 0, 0, current);
        // glimmer();
        pulse();
        break;
      case TRIPLE:
        // u8x8.drawString(0, 1, "TRIPLE");
        // strip.fill(strip.Color(0, current, 0));
        // strip.setPixelColor(1, 0, 0, current);
        lightsaber();
        break;
      case LOOONG:
        // digitalWrite(BTN_STATE_LED_PIN, !digitalRead(BTN_STATE_LED_PIN));
        // u8x8.drawString(0, 1, "LOOONG");
        // strip.setPixelColor(2, 0, 0, current);
        // pulse();
        glimmer();
        break;
      case OFF:
      default:
        // u8x8.drawString(0, 1, "OFF");
        //strip.setPixelColor(0, 0, 0, 0);
        //strip.setPixelColor(1, 0, 0, 0);
        //strip.setPixelColor(2, 0, 0, 0);
        strip.clear();
        break;
    }

    /*if ((millis() - lastLEDUpdate) >= 100) { // UPDATE EVERY 100ms
      lastLEDUpdate = millis();
      strip.show();
    }*/
    strip.show();
    delay(50);
  #endif

}

/* ################ CLICK HANDLER ################ */
bool lastBtnPressed = false;

void handleBtnPress() {
  bool btnPressed = digitalRead(BTN_PIN) == LOW;
  int curTime = millis();
  int elapsedTime = curTime - lastPressTime; // Expensive to calc this each run?

  if (currentLightMode != OFF && !btnPressed) { // CLICKING WHEN IN LIGHT MODE -> DISABLE SELECTED LIGHT MODE
    if (lastBtnPressed && !btnPressed) {
      currentLightMode = OFF;
      resetClickStates();
    }

    lastBtnPressed = btnPressed; // Early exit -> ensure that lastBtnPressed is still updated
    return;
  }

  if (!lastBtnPressed && btnPressed) { // PRESS
    lastPressTime = curTime;
    sequentialPresses += 1;
    elapsedTime = curTime - lastPressTime; // Expensive to calc this each run? // FIXME - Remove if no need to recalculate here
  }
  else if (
    lastBtnPressed &&
    btnPressed &&
    elapsedTime >= SHORT_PRESS_BREAKPOINT) { // HOLD
    
    currentLightMode = LOOONG;
    resetClickStates();
  }


  if (elapsedTime > BTN_PRESS_RESET_TIMEOUT && sequentialPresses > 0) {
    if (sequentialPresses >= 3) {
      currentLightMode = TRIPLE;
    } else if (sequentialPresses >= 2) {
      currentLightMode = DOUBLE;
    }
    resetClickStates();
  }
  lastBtnPressed = btnPressed;
}

void resetClickStates() {
  sequentialPresses = 0;
}


/* ################ STING ANIMATION ################ */ // Could move this to separate file?

int lastGlimmerUpdate = 0;
int glimmerUpdateTime = 150; // ms
void glimmer() {
  // strip.fill(STING_DARK);
  // strip.fill(strip.Color(102, 158, 173));
  strip.fill(strip.Color(39, 216, 245));
  strip.setBrightness(255); // Ensure brightness at max
  // strip.setBrightness(BRIGHTNESS);

  int curTime = millis();
  if (curTime > (lastGlimmerUpdate + glimmerUpdateTime)) {
    lastGlimmerUpdate = curTime;

    //1. Randomize nr of glimmer pixels
    //2. Randomize which pixels should glimmer
    int glimmerPixels = random(0, NUMPIXELS/2);
    for (int i = 0; i < glimmerPixels; i++) {
      // NOTE - It is possible that the same pixel is selected multiple times.
      //        Keeping it for now for irregularity.
      int glimmerPixel = random(0, NUMPIXELS);
      // strip.setPixelColor(glimmerPixel, STING_LIGHT);
      // strip.setPixelColor(glimmerPixel, strip.Color(183, 251, 253));
      strip.setPixelColor(glimmerPixel, strip.Color(109, 228, 247));
    }
  }
}

const int pulseSpeed = 5;
const int pulseMin = 20;
const int pulseMax = 220;
int pulseCurrent = 0;
int pulseDirection = 1; // 1 || -1
void pulse() {
  pulseCurrent += (pulseSpeed*pulseDirection);
  if (pulseCurrent <= pulseMin) {
    pulseCurrent = pulseMin;
    pulseDirection = 1;
  } else if (pulseCurrent >= pulseMax) {
    pulseCurrent = pulseMax;
    pulseDirection = -1;
  }

  // strip.fill(STING_DARK);
  // strip.fill(strip.Color(102, 158, 173));
  // strip.fill(strip.Color(37, 205, 232));
  strip.fill(strip.Color(39, 216, 245));
  strip.setBrightness(pulseCurrent);
}

int lastLSUpdate = 0;
int lsUpdateTime = 100; // ms
int lsExtendedLEDs = 0;
void lightsaber() {
  strip.setBrightness(255);
  // int lsColor = strip.Color(175, 115, 218);
  // int lsExtendedColor = strip.Color(0, 255, 0);
  // bool extended = strip.getPixelColor(NUMPIXELS-1) == lsColor;
  bool extended = strip.getPixelColor(NUMPIXELS-1) > 0; // TODO - Remove
  if (extended) {
    lsExtendedLEDs = 0;
    // strip.fill(lsExtendedColor); // FIXME - For debugging -> remove when sure working
  } else {
    int curTime = millis();
    if (curTime >= (lastLSUpdate + lsUpdateTime)) {
      lastLSUpdate = curTime;
      lsExtendedLEDs += 1;
      strip.clear();
      // strip.fill(lsColor, 0, lsExtendedLEDs);
      strip.fill(strip.Color(160, 32, 240), 0, lsExtendedLEDs);
      /*if (lsExtendedLEDs == NUMPIXELS) { // Reset for next run
        lsExtendedLEDs = 0;
      }*/
    }
  }
}
