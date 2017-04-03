#include "SDPArduino.h"
#include "SerialCommand.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_HMC.h"
#include <Arduino.h>
#include <Wire.h>

#define FRONT 5
#define RIGHT 3
#define BACK 4
#define LEFT 1

#define SPINNER 2

// pin numbers, for Direct IO
#define KICKER 9
#define sensorAddress 6
#define IR_PIN A3

boolean grState = false;

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

SerialCommand sCmd;

int xyz[] = {0, 0, 0};

void loop() { sCmd.readSerial(); }

void dontMove() {
  motorStop(FRONT);
  motorStop(BACK);
  motorStop(LEFT);
  motorStop(RIGHT);
}

void spinmotor() {
  int motor = atoi(sCmd.next());
  int power = atoi(sCmd.next());
  motorForward(motor, power);
}

void motorControl(int motor, int power) {
  if (power == 0) {
    motorStop(motor);
  } else if (power > 0) {
    motorBackward(motor, power);
  } else {
    motorForward(motor, -power);
  }
}

void rationalMotors() {
  int front = atoi(sCmd.next());
  int back = atoi(sCmd.next());
  int left = atoi(sCmd.next());
  int right = atoi(sCmd.next());
  motorControl(FRONT, front);
  motorControl(BACK, back);
  motorControl(LEFT, left);
  motorControl(RIGHT, right);
}

void pingMethod() { Serial.println("pang"); }

void completeHalt() { motorAllStop(); }

void grab() {
  int mode = atoi(sCmd.next());
  // 0 for opened and 1 for closed
  if (mode == 1) {
    motorForward(SPINNER, 70);
    delay(900);
    Serial.println("closed");
  } else if (mode == 0) {
    grState = 0;
    motorBackward(SPINNER, 70);
    delay(900);
    motorStop(SPINNER);
    Serial.println("opened");
  }
}

void kick() {
  digitalWrite(KICKER, HIGH);
  delay(300);
  digitalWrite(KICKER, LOW);
  Serial.println("kicked");
}

void spin() {
  int spin = atoi(sCmd.next());
  if (spin) {
    motorControl(SPINNER, 100);
  } else {
    motorControl(SPINNER, 0);
  }
}

void readCompass2(sensors_event_t event) {
}


void getCompass() {
  sensors_event_t event;
  // y is current robot rotation
  mag.getEvent(&event);
  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
  Serial.print("X: ");
  Serial.print(event.magnetic.x);
  Serial.print("  ");
  Serial.print("Y: ");
  Serial.print(event.magnetic.y);
  Serial.print("  ");
  Serial.print("Z: ");
  Serial.print(event.magnetic.z);
  Serial.print("  ");
  Serial.println("uT");

  Serial.print("Version ");
  Serial.println(event.version, 10);

  // Hold the module so that Z is pointing 'up' and you can measure the heading
  // with x&y
  // Calculate heading when the magnetometer is level, then correct for signs of
  // axis.
  float heading = atan2(event.magnetic.y, event.magnetic.x);

  // Once you have your heading, you must then add your 'Declination Angle',
  // which is the 'Error' of the magnetic field in your location.
  // Find yours here: http://www.magnetic-declination.com/
  // Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
  // If you cannot find your Declination, comment out these two lines, your
  // compass will be slightly off.
  float declinationAngle = 0.035;
  heading += declinationAngle;

  // Correct for when signs are reversed.
  if (heading < 0)
    heading += 2 * PI;

  // Check for wrap due to addition of declination.
  if (heading > 2 * PI)
    heading -= 2 * PI;

  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180 / M_PI;
  Serial.print("[PKT] [compass] ");
  Serial.println(headingDegrees);
}

void noCompass() { Serial.println("[pkt] NO COMPASS I TOLD YOU."); }

void getIRDistance(){
  int IRState = digitalRead(IR_PIN);
  Serial.print("[PKT] [IR] ");
  Serial.println(IRState);
  // http://www.sharp.co.jp/products/device/doc/opto/gp2y0d02yk_e.pdf
}

void setup() {

  if (!mag.begin()) {
    Serial.println("No mag present");
  } else {
    sCmd.addCommand("compass", getCompass);
  }
  Serial.println("setup");
  sCmd.addCommand("f", dontMove);
  sCmd.addCommand("h", completeHalt);
  sCmd.addCommand("motor", spinmotor);
  sCmd.addCommand("r", rationalMotors);
  sCmd.addCommand("ping", pingMethod);
  sCmd.addCommand("kick", kick);
  sCmd.addCommand("spin", spin);
  sCmd.addCommand("grab", grab);
  sCmd.addCommand("compass", noCompass);
  sCmd.addCommand("IR", getIRDistance);
  SDPsetup();

  helloWorld();
}

