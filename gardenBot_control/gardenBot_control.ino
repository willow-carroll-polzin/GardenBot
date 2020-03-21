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
uint8_t curSensor[3] = {1,2,3}; //Current sesnors being read/watered
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

//*************************
//Sensing and actuation loop:
void loop() {
      char buffer[100];
      //Check if there is water available
      //int waterLevel = waterLevelDetection();
      int waterLevel = 1; //TEST

      //Check which sensors need water
      drySensors=senseWater();
      waterPlants(1);
      //drySensors[0]=1;
      //Serial.println(drySensors[0]);
      if (waterLevel == 1) {
        //Cycle through all sensors that may need water
        for (int i=0; i<sizeof(drySensors); i++) {
          //Check if the current sensor needs water
          if (drySensors[i] != 0) { 
            //Serial.println("BEGIN WATERING: ");
            //Serial.println(curSensor[i]);
            waterPlants(curSensor[i]); //Start watering routine, for current sensor
            sens[i]=drySensors[i];
            drySensors[i] = 0; //Reset the status of current sensor
          }
        }
      } else if (waterLevel == 0) {
        //Serial.println("ERROR: FILL RESERVOIR!");
      }

      //Output data
      char dataSet = sprintf(buffer, "DATASTREAM: %d\t%d\t%d\t%d",waterLevel ,sens[0], sens[1], sens[2]);
      Serial.println(buffer);
}
//*************************
//Read moisture sensors
int * senseWater() {
  char buffer[100];
  int *sensors = new int[3];
  
  for(int i = 0; i<3; i++) { 
    MoisAna[i] = analogRead(MoisPins[i]);
    MoisNum[i] = ((590-MoisAna[i])/300)*100;
    
    //Serial.print("Line ");
    //Serial.print(i+1);
    //Serial.print(" - Moisture precent is:");
    //Serial.print(MoisNum[i]);
    //Serial.println("%");

    //TODO: SET CORRECT MOISTURE THRESHOLD
    if (MoisNum[i] < 3) {
      sensors[i] = 1;
    } else {
      sensors[i] = 0;
    }
  }
  return sensors; //Will return list of sensors that require watering
}

//Check water level available
int waterLevelDetection() {
  WaterAna=(long)analogRead(Water_Level_Sensor);
  WaterNum=(WaterAna/650)*4;
  
  //Serial.print("the depth is:");
  //Serial.print(WaterNum);
  //Serial.println("cm");
  
  if(WaterAna<100){
    digitalWrite(LED,HIGH);
    //TODO: TURN OFF PUMP, ALERT USER
    Serial.println("ERROR: WATER LEVEL TO LOW, REFILL REQUIRED!");
  } else {
    digitalWrite(LED,LOW);
  }
  return (1); //Will return 1 if have enough water, else will return 0
}

//Watering routine
void waterPlants(int curSensor) {
        //Actuate motors
        //M3_L = RED , M3_R = GREEN
        //M4_L = YELLOW , M4_R = BLUE 
        //Serial.println("Moving to destination...");
        //TODO: GET CORRECT TUNING
        if (curSensor == 1 || curSensor == 2 || curSensor == 3) {
          position_delta = 200;
          myMotor->step(position_delta,FORWARD, SINGLE); //Move to correct position
        }
        else if (curSensor == 4 || curSensor == 5 || curSensor == 6) {
          position_delta = 400;
          myMotor->step(position_delta,FORWARD, SINGLE); //Move to correct position
        }
        else if (curSensor == 7 || curSensor == 8 || curSensor == 9) {
          position_delta = 600;
          myMotor->step(position_delta,FORWARD, SINGLE); //Move to correct position
        }
        //delay(100); //Wait for motor to reach destination //TODO: FIX
        //Serial.println("Destination reached.");
    
        //Open valves
        //Serial.println("Opening valves.");
        if (curSensor == 1 || curSensor == 4 || curSensor == 7) {
          digitalWrite(valves, HIGH);
        }
        else if (curSensor == 2 || curSensor == 5 || curSensor == 8) {
          digitalWrite(valves, HIGH);
        }
        else if (curSensor == 3 || curSensor == 6 || curSensor == 9) {
          digitalWrite(valves, HIGH);
        }
        //delay between moving gantry/opening valves, and turning the pump on
        //delay(5000); //wait 5 seconds for everything to be done
        
        //Pump water
        //Serial.println("Watering plants....");
        digitalWrite(pump, HIGH);
        //delay(5000); //Water plants for 5 seconds
        digitalWrite(pump, LOW);
        //Serial.println("Completed watering plants.");

        //Move motors back to original position
        myMotor->step(position_delta, BACKWARD, SINGLE);
        myMotor->release();
        
        //Close valves
        digitalWrite(valves, LOW);
        
        //Serial.println("Vales closed.");
        //delay(5000); //make sure system is back to base config before return control to other parts of the program
}
//*************************
