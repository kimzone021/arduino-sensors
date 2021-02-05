#define pirSensor 8

int reading;

void setup() 
{

  pinMode(pirSensor, INPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);

//test
}

void loop() 
{

  reading = digitalRead(pirSensor);

  if (reading == HIGH) 
  {
    Serial.println("Motion");
    digitalWrite(13, HIGH);
    delay (6000);
  }
  else 
  {
    Serial.println("0");
    digitalWrite(13, LOW);
  }
  delay(3);// Delay for main loop

}
