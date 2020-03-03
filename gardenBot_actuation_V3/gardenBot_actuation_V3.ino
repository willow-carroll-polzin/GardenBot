//*************************
//Libraries
#include <math.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//*************************
//Actuation control:
Adafruit_MotorShield AFMS = Adafruit_MotorShield();  //Motor shield - Adafruit V2.0

Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2); //Bi-stepper motor with 1.8 degree motor/360 has 200 steps, motor is connected to M3 & M4 (port 2)

//*************************
void setup() {
  //Setup motor paramters
  myMotor->setSpeed(10);

  //Initilize motor sheild
  AFMS.begin();

  //Initilize serial connection
  Serial.begin(9600); //Allow for monitoring on PC (COM3) at buad rate of 9600
  Serial.println("BEGIN MOTION: ");
}


//*************************
void loop() {
  for (int i=0; i<9; i++) {
        Serial.println("BEGIN:  WATERING!");
        //waterPlants(curSensor[i]); //Start watering routine, for current sensor
  
        myMotor->step(200,FORWARD, SINGLE);
        myMotor->step(200,BACKWARD, SINGLE);
        myMotor->release();
  }
}

//*************************
//Watering routine
void waterPlants(int curSensor) {
        //Actuate motors
        //Serial.println("Moving to destination...");
        
        myMotor->step(1000,FORWARD, SINGLE);
        //myMotor->step(200,BACKWARD, SINGLE);
        myMotor->release();
        
        //Serial.println("Destination reached.");

        /*
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
        digitalWrite(valve3, LOW); */
}
