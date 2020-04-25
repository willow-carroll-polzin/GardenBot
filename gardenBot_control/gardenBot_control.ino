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
#define LED LED_BUILTIN       //Water level indicator
#define PLANT_THRESH 45       //Soil moisture level threshold for plant

//Watering sensor controls:/
double WaterAna;
double WaterNum;

//Moisture sensor controls:
double MoisAna[3];                              //Array of analog moisture values
double MoisPer[3];                              //Array of percent moisture values
static const uint8_t MoisPins[3] = {A0,A1,A2};  //Moisutre sensor pins
static const int AIR_VAL = 700;                 //Mositure reading of the sensor in air @ room temp
static const int WATER_VAL = 400;               //Moisture reading of the sensor submersed in tap water

//Sensor arrays:
uint8_t curSensor[3] = {1,2,3}; //Current sesnors being read/watered
int * drySensors;               //Sensors that require water
uint8_t sens[3];                //Sensor array to return to user

//Actuation controls:
Adafruit_MotorShield AFMS = Adafruit_MotorShield();       //Motor shield - Adafruit V2.0
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2); //Bi-stepper motor with 1.8 degree motor/360 has 200 steps, motor is connected to M3 & M4 (port 2)
int position_delta = 0;                               //Current change in steps of motors

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
  Serial.println("Setup complete");
}

//*************************
//Sensing and actuation loop:
void loop() {
      char buffer[100];
      //Check if there is water available:
      int waterLevel = waterLevelDetection();

      //Check which sensors need water:
      drySensors = senseWater();

      //If there is enough water available:
      if (waterLevel == 1) {
        //Cycle through all sensors that may need water
        for (int i=0; i<(sizeof(drySensors)+1); i++) {
          //Check if the current sensor needs water
          if (drySensors[i] == 1) { 
            waterPlants(curSensor[i]); //Start watering routine, for current sensor
            sens[i]=drySensors[i];     //Record sensor value to place in the buffer
            drySensors[i] = 0;         //Reset the status of current sensor
          }
        }
      }
      
      //Output data:
      char dataSet = sprintf(buffer, "DATASTREAM: %d\t%d\t%d\t%d",waterLevel ,sens[0], sens[1], sens[2]);
      Serial.println(buffer);
}

//*************************
//MOISTURE LEVEL:
int * senseWater() {
  char buffer[100];
  int *sensors = new int[3];
  
  for(int i = 0; i<3; i++) { 
    MoisAna[i] = analogRead(MoisPins[i]);
    MoisPer[i] = map(MoisAna[i], AIR_VAL, WATER_VAL,0,100); //Convert analog value to relative percent (0-100)
    
    if (MoisPer[i] < PLANT_THRESH) {
      sensors[i] = 1; //Need water
    } else {
      sensors[i] = 0; //Does not need water
    }
  }
  return sensors; //Will return list of sensors that require watering (value = 1)
}

//*************************
//WATER LEVEL:
int waterLevelDetection() {
  WaterAna=(long)analogRead(Water_Level_Sensor);
  WaterNum=(WaterAna/650)*4;
  
  if(WaterAna<100){
    digitalWrite(LED,HIGH);
    return (0);
  } else {
    
    digitalWrite(LED,LOW);
    return (1); //Will return 1 if have enough water, else will return 0
  }
}

//*************************
//WATERING SEQUENCE:
void waterPlants(int curSensor) {
        //M3_L = RED , M3_R = GREEN
        //M4_L = YELLOW , M4_R = BLUE 
        position_delta = 0;
        
        //Actuate motors:
        if (curSensor == 1) {
          position_delta = 0;
          myMotor->step(position_delta,FORWARD, DOUBLE); //Move to correct position 
          myMotor->release();        
        }
        else if (curSensor == 2) {
          position_delta = 850;
          myMotor->step(position_delta,FORWARD, DOUBLE); //Move to correct position
          myMotor->release();
        }
        else if (curSensor == 3) {
          position_delta = 1600;
          myMotor->step(position_delta,FORWARD, DOUBLE); //Move to correct position
          myMotor->release();
        }
    
        //Open valves
        if (curSensor == 1) {
          digitalWrite(valves, HIGH);
        }
        else if (curSensor == 2) {
          digitalWrite(valves, HIGH);
        }
        else if (curSensor == 3) {
          digitalWrite(valves, HIGH);
        }

        //Pump and continue to pump untill current dry sensor is not dry
        int dry = 1;
        int * sensors;    
        sensors = senseWater();  
        while (sensors[curSensor-1]==1) {
            digitalWrite(pump, HIGH);
            sensors = senseWater();
        }
        
        digitalWrite(pump, HIGH);
        delay(7000); //Ensure sensor reading is stabalized
        digitalWrite(pump, LOW);

        //Close valves
        digitalWrite(valves, LOW);
        
        //Move motors back to original position
        myMotor->step(position_delta, BACKWARD, DOUBLE);
        myMotor->release();
}
//*************************
