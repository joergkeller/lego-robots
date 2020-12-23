#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "SparkFun_Qwiic_Joystick_Arduino_Library.h" 

#define X_CENTER   525
#define Y_CENTER   513
#define NEO_PIN     11
#define NUM_PIXELS   1


// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUM_PIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);

JOYSTICK joystick; 

void setup() {
  Serial.begin(115200);
  Serial.println("Qwiic Joystick Example");

  while(joystick.begin() == false)
  {
    Serial.println("Joystick does not appear to be connected. Please check wiring...");
  }
  pixels.begin(); 
  pixels.setBrightness(128);
}

uint32_t wheel(uint16_t wheelPos);

void loop() {
  int x = joystick.getHorizontal();
  Serial.print("X: ");
  Serial.print(x);

  int y = joystick.getVertical();
  Serial.print(" Y: ");
  Serial.print(y);
  
  byte button = joystick.getButton();
  Serial.print(" Button: ");
  Serial.print(button);

  uint16_t forward = max(0, y - Y_CENTER);
  Serial.print(" Forward: ");
  Serial.print(forward);
  uint16_t backward = - min(0, y - Y_CENTER);
  Serial.print(" Backward: ");
  Serial.print(backward);
  uint16_t left = max(0, x - X_CENTER);
  Serial.print(" Left: ");
  Serial.print(left);
  uint16_t right = - min(0, x - X_CENTER);
  Serial.print(" Right: ");
  Serial.print(right);

  uint8_t red = map(backward, 0, Y_CENTER, 0, 255);;
  uint8_t green = map(left, 0, 1023-X_CENTER, 0, 255) + map(right, 0, X_CENTER, 0, 255);
  uint8_t blue = map(forward, 0, 1023-Y_CENTER, 0, 255);
  pixels.setPixelColor(0, pixels.Color(red, green, blue));
  pixels.show();

  Serial.println();
  delay(200);
}
