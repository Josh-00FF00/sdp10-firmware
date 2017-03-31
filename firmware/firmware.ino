#include "SerialCommand.h"
#include "SDPArduino.h"
#include <Wire.h>
#include <Arduino.h>
#include <fcntl.h>

#define FRONT 5
#define RIGHT 3
#define BACK 4
#define LEFT 1

#define SPINNER 2

#define NEWKICKER 0

// pin numbers, for Direct IO
#define KICKER 9
#define sensorAddress 6

boolean grState = false;
int IRState;
const int STATE = A3;

int run = 0;

SerialCommand sCmd;

void loop(){
  sCmd.readSerial();
}

void test(){
  run = 1;
}

void dontMove(){
  motorStop(FRONT);
  motorStop(BACK);
  motorStop(LEFT);
  motorStop(RIGHT);
}

void spinmotor(){
  int motor = atoi(sCmd.next());
  int power = atoi(sCmd.next());
  motorForward(motor, power);
}

void motorControl(int motor, int power){
  if(power == 0){
    motorStop(motor);
  } else if(power > 0){
    motorBackward(motor, power);
  } else {
    motorForward(motor, -power);
  }
}

void rationalMotors(){
  int front = atoi(sCmd.next());
  int back  = atoi(sCmd.next());
  int left  = atoi(sCmd.next());
  int right = atoi(sCmd.next());
  motorControl(FRONT, front);
  motorControl(BACK, back);
  motorControl(LEFT, left);
  motorControl(RIGHT, right);
}

void pingMethod(){
  Serial.println("pang");
}

void completeHalt(){
  motorAllStop();
}

void grab(){
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

void kick(){
  digitalWrite(KICKER,HIGH);
  delay(500);
  digitalWrite(KICKER,LOW);
  Serial.println("kicked");
}

void kick2(){
   motorBackward(LEFT, 70);
   motorBackward(RIGHT, 70);
   motorBackward(NEWKICKER, 100);
   delay(1500);
   motorStop(LEFT);
   motorStop(RIGHT);
   motorStop(NEWKICKER);
   Serial.println("kicked");
}

void spin(){
  int spin = atoi(sCmd.next());
  if(spin){
    motorControl(SPINNER, 100);
  }else{
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
  pinMode(STATE, INPUT);
  
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
  helloWorld();
}
