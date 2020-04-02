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
uint8_t position_delta = 0;                               //Current change in steps of motors
int flag = 0;
int check = 0;

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
  flag = 0;
  check = motorTune();
  if (position_delta == 1000) {
    flag = 1;
    check = motorTune(flag);
  }

  if (check == 1) {
    Serial.println("-------COMPLETED MOTOR TUNING-------");
  }
}

//*************************
//Tuning Function:
void motorTune(int flag) {
  position_delta += 1;
  myMotor->step(position_delta,FORWARD, SINGLE); //Move to next position
  Serial.print("STEP: ");
  Serial.println(position_delta);

  if (flag == 1) {
    myMotor->step(position_delta,BACKWARD, SINGLE); //Move to start position
    return 1;
  }
  myMotor->release();
  return 0;
}
//*************************
