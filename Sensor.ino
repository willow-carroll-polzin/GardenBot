#define Water_Level_Sensor A3

#define LED 8

double WaterAna;
double WaterNum;
double MoisAna[3];
double MoisNum[3];
static const uint8_t MoisPins[3] = {A0,A1,A2};

void setup() {
  // put your setup code here, to run once:
  pinMode(LED,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  /************Soil moisture detection*************/
  for(int i = 0; i<3; i++){
 
    MoisAna[i] = analogRead(MoisPins[i]);
    MoisNum[i] = ((590-MoisAna[i])/300)*100;

    Serial.print("Line ");
    Serial.print(i+1);
    Serial.print(" - Moisture precent is:");
    Serial.print(MoisNum[i]);
    Serial.println("%");
  }
  /*
   * if (MoisNum[]<35){
   *   move watering device
   *   start pumping
   * }else{
   *   stop pumping
   * }
   */
   
  /************Water level detection*************/
  WaterAna=(long)analogRead(Water_Level_Sensor);
  WaterNum=(WaterAna/650)*4;
  
  Serial.print("the depth is:");
  Serial.print(WaterNum);
  Serial.println("cm");
  
  if(WaterAna<100){
    digitalWrite(LED,HIGH);
    //pump stop working
  }else{
    digitalWrite(LED,LOW);
  }
  delay(1000);
}
