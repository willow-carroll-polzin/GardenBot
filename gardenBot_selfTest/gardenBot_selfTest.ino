//*************************
//Libraries
#include <math.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//*************************
//Irrigation controls:
#define pump 1                //Pump control pin
#define valves 0              //Valve control (1,2,3)
#define Water_Level_Sensor A3 //Water level sensor
#define LED 8                 //Watering indicator LED

//Watering sensor variables:
double WaterAna;
double WaterNum;
double MoisAna[3];
double MoisNum[3];
static const uint8_t MoisPins[3] = {A0,A1,A2};

//Sensor flag:
uint8_t curSensor[9] = {1,2,3,4,5,6,7,8,9}; //Current sesnors being read/watered
int * drySensors; //List of sensors that require water
uint8_t sens[3];

//Actuation control:
Adafruit_MotorShield AFMS = Adafruit_MotorShield();  //Motor shield - Adafruit V2.0
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2); //Bi-stepper motor with 1.8 degree motor/360 has 200 steps, motor is connected to M3 & M4 (port 2)
uint8_t position_delta = 0;

//*************************
//Arduino setup:
void setup() {
  //Irrigation setup
  pinMode(pump, OUTPUT);
  pinMode(valves, OUTPUT);
  pinMode(LED,OUTPUT);

  digitalWrite(pump, LOW);
  digitalWrite(valves, LOW);
  
  //Setup motor paramters
  myMotor->setSpeed(10);

  //Initilize motor sheild
  AFMS.begin();

  //Initilize serial connection
  Serial.begin(9600); //Allow for monitoring on PC (COM3) at buad rate of 9600
  //Serial.println("BEGIN MOTION: ");
}

//Main fns:
void loop() {
  //WATER LEVEL TEST:
  waterTest();
  
  //MOISTURE LEVEL TEST:
  moistTest();
  
  //PUMP TEST:
  pumpTest();
  
  //MOTOR TEST:
  motorTest();
}

//*************************
//Test Water level sensor:
int waterTest() {
  WaterAna=(long)analogRead(Water_Level_Sensor);
  WaterNum=(WaterAna/650)*4;

  if(WaterAna<100) {
    Serial.print("WATER LOW - ");
    Serial.println(WaterAna);
  } else {
    Serial.println(WaterAna);
  }
}

//Test Moisutre sensors:
int moistTest() {
  char buffer[100];
  int *sensors = new int[9];
  
  for(int i = 0; i<9; i++) { 
    MoisAna[i] = analogRead(MoisPins[i]);
    MoisNum[i] = ((590-MoisAna[i])/300)*100;

    if (MoisNum[i] < 10) {
      sensors[i] = 1;
    } else {
      sensors[i] = 0;
    }

    Serial.print("MOISTURE SENSOR: ");
    Serial.println(sensors[i]);
    if (sensors[i] == 1) {
      Serial.println("^ IS TO LOW!");
    }
  }
}

//Test pump and valves:
int pumpTest() {
  digitalWrite(valves, HIGH);
  Serial.println("VALVES OPEN, PUMPING!");
  digitalWrite(pump, HIGH);
  delay(1500);
  digitalWrite(pump, LOW);
  digitalWrite(valves, LOW);
  Serial.println("VALVES CLOSED!");
}

//Test motors:
int motorTest() {
  position_delta = 500;
  myMotor->step(position_delta,FORWARD, SINGLE); //Move to correct position
  myMotor->step(position_delta,BACKWARD, SINGLE); //Move to previous position
  myMotor->release();
}
