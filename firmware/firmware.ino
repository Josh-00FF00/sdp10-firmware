#include "SDPArduino.h"
#include "SerialCommand.h"
#include <Arduino.h>
#include <Wire.h>

#define FRONT 5
#define RIGHT 3
#define BACK 4
#define LEFT 1

#define SPINNER 2

#define NEWKICKER 0

// pin numbers, for Direct IO
#define KICKER 9
#define sensorAddress 6
#define STATE = A3;

boolean grState = false;
int IRState;

SerialCommand sCmd;

void loop() {
  sCmd.readSerial();
  readCompass(xyz);
  sendCompass(xyz);
}

sendCompass(int[] xyz) {
  Serial.print("x: ");
  Serial.print(xyz[0]);

  Serial.print(" y: ");
  Serial.print(xyz[1]);

  Serial.print(" z: ");
  Serial.println(xyz[2]);
}

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
  //0 for opened and 1 for closed
  if(mode == 1){
    grState = 1;
    motorForward(SPINNER, 70);
    delay(900);
    //motorStop(SPINNER);
    Serial.println("closed");
  }
  else if(mode == 0){
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

void getIRDistance(){
  IRState = digitalRead(STATE);
  Serial.print("[PKT] ");
  Serial.println(IRState);
  // http://www.sharp.co.jp/products/device/doc/opto/gp2y0d02yk_e.pdf

}

void setup(){
  pinMode(3, OUTPUT);

  sCmd.addCommand("f", dontMove);
  sCmd.addCommand("h", completeHalt);
  sCmd.addCommand("motor", spinmotor);
  sCmd.addCommand("r", rationalMotors);
  sCmd.addCommand("ping", pingMethod);
  sCmd.addCommand("kick", kick2);
  sCmd.addCommand("spin", spin);
  sCmd.addCommand("grab", grab);
  sCmd.addCommand("IR", getIRDistance);
  SDPsetup();

  int xyz[3];

  helloWorld();
}
