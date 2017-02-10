#include "SerialCommand.h"
#include "SDPArduino.h"
#include <Wire.h>
#include <Arduino.h>

#define FRONT 5
#define RIGHT 3
#define BACK 4
#define LEFT 1
#define SPINNER 2
#define KICKER 0

boolean grState = false;

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

void kicker(){
// removed
}

void completeHalt(){
  motorAllStop();
}

void grab(){
  int mode = atoi(sCmd.next());
  //0 for opened and 1 for closed
  if(mode == 1 && !grState){
    //grState = 1;
    motorBackward(SPINNER, 80);
    delay(600);
    motorStop(SPINNER);
    Serial.println("closed");
  }
  else if(mode == 0 && grState){
    grState = 0;
    motorForward(SPINNER, 80);
    delay(600);
    motorStop(SPINNER);
    Serial.println("opened");
  }
}

void kick(){
  digitalWrite(9,HIGH);
  delay(300);
  digitalWrite(9,LOW);
  Serial.println("kicked");
}

void setup(){
  sCmd.addCommand("f", dontMove); 
  sCmd.addCommand("h", completeHalt); 
  sCmd.addCommand("motor", spinmotor); 
  sCmd.addCommand("r", rationalMotors); 
  sCmd.addCommand("ping", pingMethod); 
  sCmd.addCommand("kick", kick);
  sCmd.addCommand("grab", grab); 
  SDPsetup();
  helloWorld();
}
