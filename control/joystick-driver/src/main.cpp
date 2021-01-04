#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "SparkFun_Qwiic_Joystick_Arduino_Library.h" 
#include "SCMD.h"
#include "SCMD_config.h" //Contains #defines for common SCMD register names and values
#include "Wire.h"

#define X_CENTER   524
#define Y_CENTER   513

#define NEO_PIN     11
#define NUM_PIXELS   1

#define DIRECTION_ATTENUATION   3
#define MAX_SPEED             150
#define MAX_SPEED_CHANGE        5
#define CHANGE_INTERVAL        50 // ms

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1

// I2C controlled analog Joystick with button
JOYSTICK joystick; 

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUM_PIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);

// This creates the main object of one motor driver and connected peripherals
SCMD myMotorDriver; 

int16_t oldVector = 0;
int16_t oldOffset = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("Qwiic Joystick Motor Driver");

  // Initialize joystick
  while(joystick.begin() == false) {
    Serial.println("Joystick does not appear to be connected. Please check wiring...");
  }

  // Initialize neo-pixel
  pixels.begin(); 
  pixels.setBrightness(128);

  // Initialize motor driver
  myMotorDriver.settings.commInterface = I2C_MODE;
  myMotorDriver.settings.I2CAddress = 0x5D; //config pattern is "1000" (default) on board for address 0x5D

  while (myMotorDriver.begin() != 0xA9) { //Wait until a valid ID word is returned
    Serial.println( "ID mismatch, trying again" );
    delay(500);
  }
  Serial.println( "ID matches 0xA9" );

  // Check to make sure the driver is done looking for peripherals before beginning
  Serial.print("Waiting for enumeration...");
  while ( myMotorDriver.ready() == false );
  Serial.println("Done.");
  Serial.println();

  // Set motor inversion (so for both motors 'forward' means the same thing)
  while ( myMotorDriver.busy() ); //Waits until the SCMD is available.
  myMotorDriver.inversionMode(LEFT_MOTOR, 0);
  myMotorDriver.inversionMode(RIGHT_MOTOR, 0);

  // Enables the output driver hardware
  while ( myMotorDriver.busy() );
  myMotorDriver.enable(); 
}

void loop() {
  // Get joystick state
  int x = joystick.getHorizontal();
  Serial.print("X: ");
  Serial.print(x);

  int y = joystick.getVertical();
  Serial.print(" Y: ");
  Serial.print(y);
  
  byte button = joystick.getButton();
  Serial.print(" Button: ");
  Serial.print(button);

  // Normalize joystick position, considering center position
  uint16_t forward = map(max(0, y - Y_CENTER), 0, 1023-Y_CENTER, 0, 255);
  Serial.print(" Forward: ");
  Serial.print(forward);
  uint16_t backward = map(-min(0, y - Y_CENTER), 0, Y_CENTER, 0, 255);
  Serial.print(" Backward: ");
  Serial.print(backward);

  uint16_t left = map(max(0, x - X_CENTER), 0, 1023-X_CENTER, 0, 255);
  Serial.print(" Left: ");
  Serial.print(left);
  uint16_t right = map(-min(0, x - X_CENTER), 0, X_CENTER, 0, 255);
  Serial.print(" Right: ");
  Serial.print(right);

  // Show direction in pixel color
  uint8_t red = backward;
  uint8_t green = left + right;
  uint8_t blue = forward;
  pixels.setPixelColor(0, pixels.Color(red, green, blue));
  pixels.show();

  // Limit acceleration and speed
  int16_t moveVector = ((int16_t)forward) - backward;
  int16_t directionOffset = (((int16_t)left) - right) / DIRECTION_ATTENUATION;
  if ((moveVector - oldVector) > MAX_SPEED_CHANGE) {
    moveVector = min(oldVector + MAX_SPEED_CHANGE, MAX_SPEED);
  } else if ((moveVector - oldVector) < -MAX_SPEED_CHANGE) {
    moveVector = max(oldVector - MAX_SPEED_CHANGE, -MAX_SPEED);
  }
  oldVector = moveVector;

  // Drive motor
  int16_t moveLeft = moveVector + directionOffset;
  int16_t moveRight = moveVector - directionOffset;
  Serial.print(" Move left: "); 
  Serial.print(moveLeft);
  Serial.print(" Move right: "); 
  Serial.print(moveRight);
  while ( myMotorDriver.busy() ); //Waits until the SCMD is available.
  if (moveLeft >= 0) {
    uint8_t level = min(moveLeft, 255);
    myMotorDriver.setDrive(LEFT_MOTOR, 0, level);
  } else {
    uint8_t level = min(-moveLeft, 255);
    myMotorDriver.setDrive(LEFT_MOTOR, 1, level);
  }
  if (moveRight >= 0) {
    uint8_t level = min(moveRight, 255);
    myMotorDriver.setDrive(RIGHT_MOTOR, 0, level);
  } else {
    uint8_t level = min(-moveRight, 255);
    myMotorDriver.setDrive(RIGHT_MOTOR, 1, level);
  }

  Serial.println();
  delay(CHANGE_INTERVAL);
}
