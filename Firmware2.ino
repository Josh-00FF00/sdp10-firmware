#include "SerialCommand.h"
#include "SDPArduino.h"
#include <Wire.h>
#include <Arduino.h>


//Kickers in back
//define FRONT 1
//define RIGHT 7
//define BACK 5
//define LEFT 3

//Kickers in front
#define FRONT 5
#define RIGHT 3
#define BACK 1
#define LEFT 7

#define KICKERS 0
#define SPEAKER 1

#define OPADDR 0x5A
#define REGADDR 0x04


int run = 0;

SerialCommand sCmd;

void muxTest(){
  int motor = atoi(sCmd.next());
  int dir  = atoi(sCmd.next());
  int pow  = atoi(sCmd.next());
  Wire.beginTransmission(OPADDR);
  Wire.write(motor);
  Wire.write(dir);
  Serial.println(Wire.endTransmission());
  Wire.beginTransmission(OPADDR);
  Wire.write(motor+1);
  Wire.write(pow);
  Serial.println(Wire.endTransmission());
  delay(2000);
  Wire.beginTransmission(OPADDR);
  Wire.write(motor);
  Wire.write(0);
  Serial.println(Wire.endTransmission());
}


void loop(){
  sCmd.readSerial();
}


void test(){
  run = 1;
}

void dontMove(){
  motorControl(FRONT, 0);
  motorControl(BACK, 0);
  motorControl(LEFT, 0);
  motorControl(RIGHT, 0);
}

void spinmotor(){
  int motor = atoi(sCmd.next());
  int power = atoi(sCmd.next());
  motorForward(motor, power);
}

void motorControl(int motor, int power){
  if(power == 0){
      Wire.beginTransmission(OPADDR);
      Wire.write(motor);
      Wire.write(0);
      Wire.endTransmission();
  } else if(power > 0){
      Wire.beginTransmission(OPADDR);
      Wire.write(motor);
      Wire.write(1);
      Wire.endTransmission();
      Wire.beginTransmission(OPADDR);
      Wire.write(motor + 1);
      Wire.write(power);
      Wire.endTransmission();
  } else {
      Wire.beginTransmission(OPADDR);
      Wire.write(motor);
      Wire.write(2);
      Wire.endTransmission();
      Wire.beginTransmission(OPADDR);
      Wire.write(motor + 1);
      Wire.write(-power);
      Wire.endTransmission();
  }
}

void rationalMotors(){
  int front = atoi(sCmd.next());
  int back  = atoi(sCmd.next());
  int left  = atoi(sCmd.next());
  int right = atoi(sCmd.next());
  motorControl(FRONT, -front);
  motorControl(BACK, back);
  motorControl(LEFT, -left);
  motorControl(RIGHT, right);
}

void pingMethod(){
  Serial.println("pang");
}

void kicker(){
  int type = atoi(sCmd.next());
  if(type == 0){
    motorStop(KICKERS);
  } else if (type == 1){
    motorForward(KICKERS, 100);
  } else {
    motorBackward(KICKERS, 100);
  }
}

void completeHalt(){
  motorAllStop();
  motorControl(FRONT, 0);
  motorControl(BACK, 0);
  motorControl(LEFT, 0);
  motorControl(RIGHT, 0);
}

void setup(){
  Wire.begin();
  sCmd.addCommand("f", dontMove); 
  sCmd.addCommand("h", completeHalt); 
  sCmd.addCommand("motor", spinmotor); 
  sCmd.addCommand("r", rationalMotors); 
  sCmd.addCommand("ping", pingMethod); 
  sCmd.addCommand("kick", kicker); 
  sCmd.addCommand("mux", muxTest); 
  SDPsetup();
  helloWorld();
}

