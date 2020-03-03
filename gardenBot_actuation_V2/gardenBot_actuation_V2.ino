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
int flag = 0; //0=no watering needed, 1=watering needed
int curSensor[9] = {1,2,3,4,5,6,7,8,9}; //Current sesnors being read/watered

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
    for (int i=0; i<9; i++) {
      //TODO: Check moisture sensors (i.e. curSensor[i])
      //float curVal = analogRead(c0)*(5.0/1023.0);
      //Serial.println(curVal);
      Serial.println("Checking moisture sensors...");

      //IF CURSENSOR NEEDS WATER:
      if (checkWater > 0) {
        Serial.println("BEGIN:  WATERING!");
        waterPlants(curSensor[i]); //Start watering routine, for current sensor
      } else {
        Serial.println("ERROR: FILL RESERVOIR!");
        //TODO: Send error message to GUI, stop all functions and wait
      }
    }

    flag = 1; //TEST
    while (flag>0) {

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
        if (curSensor == 1 || curSensor == 2 || curSensor == 3) {
          myMotor->step(200,FORWARD, SINGLE); //Move to correct position
          myMotor->step(200,BACKWARD, SINGLE); //Return to previous position
          myMotor->release();
        }
        else if (curSensor == 4 || curSensor == 5 || curSensor == 6) {
          myMotor->step(200,FORWARD, SINGLE); //Move to correct position
          myMotor->step(200,BACKWARD, SINGLE); //Return to previous position
          myMotor->release();
        }
        else if (curSensor == 7 || curSensor == 8 || curSensor == 9) {
          myMotor->step(200,FORWARD, SINGLE); //Move to correct position
          myMotor->step(200,BACKWARD, SINGLE); //Return to previous position
          myMotor->release();
        }
        delay(100); //Wait for motor to reach destination
        Serial.println("Destination reached.");
    
        //Open valves
        Serial.println("Opening valves.");
        if (curSensor == 1 || curSensor == 2 || curSensor == 3) {
          digitalWrite(valve1, HIGH);
        }
        else if (curSensor == 4 || curSensor == 5 || curSensor == 6) {
          digitalWrite(valve2, HIGH);
        }
        else if (curSensor == 7 || curSensor == 8 || curSensor == 9) {
          digitalWrite(valve3, HIGH);
        }
        
        //Pump water
        Serial.println("Watering plants....");
        digitalWrite(pump1, HIGH);
        delay(5000); //Water plants for 5 seconds
        digitalWrite(pump1, LOW);
        Serial.println("Completed watering plants.");
        
        //Close valves
        Serial.println("Closing valves.");
        digitalWrite(valve1, LOW);
        digitalWrite(valve2, LOW);
        digitalWrite(valve3, LOW);
        Serial.println("Vales closed.")
}
//*************************
