//*************************
//Libraries
#include <math.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//*************************
//Actuation controls:
Adafruit_MotorShield AFMS = Adafruit_MotorShield();       //Motor shield - Adafruit V2.0
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2); //Bi-stepper motor with 1.8 degree motor/360 has 200 steps, motor is connected to M3 & M4 (port 2)
int position_delta = 0;                                   //Current change in steps of motors
int check = 0;                                            //Check if test is complete

//*************************
//Arduino setup:
void setup() {
  //Setup motor paramters
  myMotor->setSpeed(10);

  //Initilize motor sheild
  AFMS.begin();

  //Initilize serial connection
  Serial.begin(9600); //Allow for monitoring on PC (COM3) at buad rate of 9600
  Serial.println("-------PERFORMING MOTOR TUNING-------");
}

//*************************
//Tuning Loop:
void loop() {
  check = motorTune();

  if (check == 1) {
    Serial.println("-------COMPLETED MOTOR TUNING-------");
    while (1) {
      Serial.println(".");
    }
  }
}

//*************************
//Tuning Function:
int motorTune() {
  //Get positions:
  Serial.println("TEST START");
  position_delta += 1;     //Step through positions
  //position_delta = 0;    //First sensor
  //position_delta = 900;  //Second sensor
  //position_delta = 1800; //Third sensor
  
  myMotor->step(position_delta,FORWARD, DOUBLE); //Move to next position
  
  Serial.print("STEP: ");
  Serial.println(position_delta);
  
  myMotor->step(position_delta,BACKWARD, DOUBLE);
  myMotor->release();

  if (position_delta >= 1800) {
    return 1;
  } else {
   return 0;
  }
}
//*************************
