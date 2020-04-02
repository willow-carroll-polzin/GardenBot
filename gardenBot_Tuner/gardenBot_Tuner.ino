//*************************
//Libraries
#include <math.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//*************************
//Irrigation controls:
#define pump 8                //Pump control pin
#define valves 9              //Valve control (1,2,3)
#define Water_Level_Sensor A3 //Water level sensor

//Watering sensor variables:
double WaterAna;
double WaterNum;
int MoisAna[3];
double MoisPer[3];
static const uint8_t MoisPins[3] = {A0,A1,A2};
//static const int AIR_VAL = 630;
//static const int WATER_VAL = 300;
static const int AIR_VAL = 700;
static const int WATER_VAL = 400;

//Sensor flag:
uint8_t curSensor[3] = {1,2,3}; //Current sesnors being read/watered
int * drySensors; //List of sensors that require water
uint8_t sens[3];

//Actuation control:
Adafruit_MotorShield AFMS = Adafruit_MotorShield();  //Motor shield - Adafruit V2.0
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2); //Bi-stepper motor with 1.8 degree motor/360 has 200 steps, motor is connected to M3 & M4 (port 2)
int position_delta = 0;

//*************************
//Arduino setup:
void setup() {
  //Initilize serial connection
  Serial.begin(9600); //Allow for monitoring on PC (COM3) at buad rate of 9600
  //Serial.println("BEGIN MOTION: ");
  Serial.println("Setup starting");
  
  //Irrigation setup
  pinMode(pump, OUTPUT);
  pinMode(valves, OUTPUT);

  digitalWrite(pump, LOW) ;
  digitalWrite(valves, LOW);
  
  //Setup motor paramters
  myMotor->setSpeed(10);
  Serial.println("Motor speed set");
  
  //Initilize motor sheild
  AFMS.begin(); //WE ARE GETTING HUNG UP RIGHT HERE
  Serial.println("Setup complete");
}

//Main fns:
void loop() {
  //WATER LEVEL TEST:
  //waterTest();
  
  //MOISTURE LEVEL TEST:
  //moistTest();
  
  //PUMP & VALVES TEST:
  pumpTest();
  
  //MOTOR TEST:
  //motorTest();
}

//*************************
//Test Water level sensor:
void waterTest() {
  //Serial.println("-------PERFORMING WATER LEVEL SENSOR TEST-------");
  
  WaterAna=(long)analogRead(Water_Level_Sensor);
  WaterNum=(WaterAna/650)*4;
  Serial.print("Water level: ");
  Serial.print(WaterAna);
  Serial.print(" - ");
  Serial.println(WaterNum);
  if(WaterAna<100) {
    Serial.print("WATER LOW! - ");
    Serial.println(WaterAna);
  } else {
    Serial.print("LEVEL - ");
    Serial.println(WaterAna);
  }
}

//Test Moisutre sensors:
void moistTest() {
  //Serial.println("-------PERFORMING MOISTURE SENSORS TEST-------");
  
  char buffer[100];
  int *sensors = new int[3];
  int sens[3]={0,0,0};
  
  for(int i = 0; i<3; i++) { 
    MoisAna[i] = analogRead(MoisPins[i]);
    //Serial.println(MoisAna[i]);
    MoisPer[i] = map(MoisAna[i], AIR_VAL, WATER_VAL,0,100);
    //Serial.println(MoisPer[i]);
    
    if (MoisPer[i] < 50) {
      sens[i] = 1; //Needs water
    } else {
      sens[i] = 0; //Does not need water
    }
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(" - ");
    //Serial.println(MoisAna[i]);
    Serial.println(MoisPer[i]);
    //Serial.println(sens[i]);
  }
}

//Test pump and valves:
void pumpTest() {
  //Serial.println("-------PERFORMING PUMP AND VALVE TEST-------");
  
  Serial.println("VALVES OPEN, PUMPING!");
  digitalWrite(valves, HIGH);
  //delay(1000);
  digitalWrite(pump, HIGH);
  delay(5000);
  digitalWrite(pump, LOW);
  digitalWrite(valves, LOW);
  delay(1000);
  Serial.println("VALVES CLOSED!");
}

//Test motors:
void motorTest() {
  Serial.println("-------PERFORMING MOTOR TEST-------");
  
  position_delta = 1000;
  myMotor->step(position_delta,FORWARD, SINGLE); //Move to correct position
  Serial.println("AAA");
  myMotor->step(position_delta,BACKWARD, SINGLE); //Move to previous position
  myMotor->release();
}
