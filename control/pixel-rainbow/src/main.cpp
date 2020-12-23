#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        11

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS   1

// Time (in milliseconds) to pause between pixels
#define DELAYVAL 500 

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t wheel(uint16_t wheelPos);

void setup() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(80); // 1/3 brightness
}

void loop() {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, wheel((i*1+j) & 255));
    }
    pixels.show();
    delay(10);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t wheel(uint16_t wheelPos) {
  if(wheelPos < 85) {
    return pixels.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
  } 
  else if(wheelPos < 170) {
    wheelPos -= 85;
    return pixels.Color(255 - wheelPos * 3, 0, wheelPos * 3);
  } 
  else {
    wheelPos -= 170;
    return pixels.Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
}
