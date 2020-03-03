//*************************
//Libraries
#include <math.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//*************************
//Irrigation control:
int pump1 = 0; //Pump control pin
int valve1 = 1; //Valve 1
int valve2 = 2; //Valve 2
int valve3 = 3; //Valve 3

//Sensor flag:
int sensorToWater[9];; //List of sensors that will be watered
//sensorToWater = new int[5];
int curSensor[9]; //Current sesnors being read/watered

//Timing:

//Actuation control:
Adafruit_MotorShield AFMS = Adafruit_MotorShield();  //Motor shield - Adafruit V2.0

Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2); //Bi-stepper motor with 1.8 degree motor/360 has 200 steps, motor is connected to M3 & M4 (port 2)

//*************************
//Arduino setup:
void setup() {
  //Irrigation setup
  pinMode(pump1, OUTPUT);
  pinMode(valve1, OUTPUT);
  pinMode(valve2, OUTPUT);
  pinMode(valve3, OUTPUT);

  digitalWrite(pump1, LOW);
  digitalWrite(valve1, LOW);
  digitalWrite(valve2, LOW);
  digitalWrite(valve3, LOW);
  
  //Setup motor paramters
  myMotor->setSpeed(10);

  //Initilize motor sheild
  AFMS.begin();

  //Initilize serial connection
  Serial.begin(9600); //Allow for monitoring on PC (COM3) at buad rate of 9600
  Serial.println("BEGIN MOTION: ");
}

//*************************
//Sensing and actuation loop:
void loop() {
  //Elapsed time
  int curTime = 2;
  //Check current time
  if (curTime > 1) {
      //Check moisture levels
      for (int i=0; i<9; i++) {
        Serial.println("Checking moisture sensors...");
        //TODO: Check moisture sensors (i.e. curSensor[i])
        //float curVal = analogRead(c0)*(5.0/1023.0);
        //Serial.println(curVal);
        
        //TODO: assign sensors that need water
        sensorToWater[i] = curSensor[i];
      }
  
      //Check current water level
      if (checkWater > 0) {
        Serial.println("BEGIN:  WATERING!");
        for (int j=0; j<9; j++) { //Water each sensor in the sesnor to water list
          waterPlants(sensorToWater[j]); //Start watering routine
        }
      //If there is not enough water, report to user and stop all operations
      } else {
        Serial.println("ERROR: FILL RESERVOIR!");
        //TODO: Send error message to GUI, stop all functions and wait
      }
  }
}
//*************************
//Read water level sensor
int checkWater() {
  //TODO: Add code to check water level
  return (1); //Will return 1 if have enough water, else will return 0
}

//Watering routine
void waterPlants(int curSensor) {
        //Actuate motors
        Serial.println("Moving to destination...");
        //TODO: ADD METHOD TO DETERMINE WHICH SENSORS NEEDS WATER

        //M3_L = RED , M3_R = GREEN
        //M4_L = YELLOW , M4_R = BLUE 
        myMotor->step(200,FORWARD, SINGLE);
        myMotor->step(200,BACKWARD, SINGLE);
        myMotor->release();
        Serial.println("Destination reached.");
    
        //Open valves
        Serial.println("Opening valves.");
        digitalWrite(valve1, HIGH);
        digitalWrite(valve2, HIGH);
        digitalWrite(valve3, HIGH);
    
        //Pump water
        Serial.println("Watering plants....");
        digitalWrite(pump1, HIGH);
        delay(100);
        digitalWrite(pump1, LOW);
        
        //Close valves
        Serial.println("Closing valves.");
        digitalWrite(valve1, LOW);
        digitalWrite(valve2, LOW);
        digitalWrite(valve3, LOW);
}
//*************************