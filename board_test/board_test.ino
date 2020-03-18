void setup() {
  // put your setup code here, to run once:
  pinMode(0, OUTPUT);
  pinMode(2,OUTPUT);

  digitalWrite(0,HIGH);
  digitalWrite(2,HIGH);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("RUN");
}
