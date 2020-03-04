//*************************
//Libraries
#include <math.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//*************************
//Irrigation controls:
#define pump1 0;              //Pump control pin
#define valve1 1;             //Valve 1
#define valve2 2;             //Valve 2
#define valve3 3;             //Valve 3
#define Water_Level_Sensor A3 //Water level sensor
#define LED 8                 //Watering indicator LED

//Watering sensor variables:
double WaterAna;
double WaterNum;
double MoisAna[3];
double MoisNum[3];
static const uint8_t MoisPins[3] = {A0,A1,A2};

//Sensor flag:
int curSensor[9] = {1,2,3,4,5,6,7,8,9}; //Current sesnors being read/watered
int drySenesors[9]; //List of sensors that require water

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
  pinMode(LED,OUTPUT);

  digitalWrite(pump1, LOW);
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
    
      //Check water sensors
      Serial.println("Checking moisture sensors...");
      drySenesors = senseWater();
      
      //Check if there is water available
      if (waterLevelDetection() == 1) {
        //Cycle through all sensors that may need water
        for (int i=0; i<sizeof(drySenesors); i++) {
          //Check if the current sensor needs water
          if (drySensors[i] != 0) { 
            Serial.print("BEGIN WATERING: ");
            Serial.println(curSensor[i]);
            waterPlants(curSensor[i]); //Start watering routine, for current sensor
            drySensor[i] = 0; //Reset the status of current sensor
          }
        }
      } else if (waterLevelDetection() == 0) {
        Serial.println("ERROR: FILL RESERVOIR!");
      }
}
//*************************
//Read water level sensors
int senseWater() {
  for(int i = 0; i<9; i++) { 
    MoisAna[i] = analogRead(MoisPins[i]);
    MoisNum[i] = ((590-MoisAna[i])/300)*100;
    
    Serial.print("Line ");
    Serial.print(i+1);
    Serial.print(" - Moisture precent is:");
    Serial.print(MoisNum[i]);
    Serial.println("%");

    //TODO: SET CORRECT MOISTURE THRESHOLD
    int drySensors[9];
    if MoisNum[i] < 10 {
      drySensors[i] = 1;
    } else {
      drySensors[i] = 0;
    }
  }
  return drySensors; //Will return list of sensors that require watering
}

//Check water level available
int waterLevelDetection() {
  WaterAna=(long)analogRead(Water_Level_Sensor);
  WaterNum=(WaterAna/650)*4;
  
  Serial.print("the depth is:");
  Serial.print(WaterNum);
  Serial.println("cm");
  
  if(WaterAna<100){
    digitalWrite(LED,HIGH);
    //TODO: TURN OFF PUMP, ALERT USER
    Serial.println("ERROR: WATER LEVEL TO LOW, REFILL REQUIRED!");
  } else {
    digitalWrite(LED,LOW);
  }
  
  delay(1000);
  return (1); //Will return 1 if have enough water, else will return 0
}

//Watering routine
void waterPlants(int curSensor) {
        //Actuate motors
        //M3_L = RED , M3_R = GREEN
        //M4_L = YELLOW , M4_R = BLUE 
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
        Serial.println("Vales closed.");
}
//*************************
