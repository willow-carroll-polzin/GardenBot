//*************************
//Libraries
#include <math.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

//*************************
//Constants:
//Mux control/signal pins
int c0 = 2;
int c1 = 3;
int c2 = 4;
int c3 = 5;

//*************************
//Arduino setup:
void setup(){
  // Setup control on pins D2, D3, D4, D5 for MUX bit control
  pinMode(c0, OUTPUT); 

  pinMode(A1, OUTPUT);
  digitalWrite(A1, LOW);

  //Start with write to low on all control pins
  digitalWrite(c0, LOW);

  Serial.begin(9600); //Allow for monitoring on PC (COM3) at buad rate of 9600
}

//*************************
//Sensing and actuation loop:
void loop() {
    float curVal = analogRead(c0)*(5.0/1023.0);
    Serial.println(curVal);
}
//*************************
