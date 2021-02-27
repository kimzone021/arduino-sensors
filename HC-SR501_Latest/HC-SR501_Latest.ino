#define pirSensor 8

#include "WiFiEsp.h"
#include <ArduinoJson.h>

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"

// set up software serial to allow serial communication to our TX and RX pins
SoftwareSerial Serial1(6, 7);
#endif

char ssid[] = "Telia-73E45D";
char pass[] = "FD979C75E0";
int status = WL_IDLE_STATUS;

char server[] = "192.168.1.127";

int reading;

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);

  WiFi.init(&Serial1);

  while (status != WL_CONNECTED)
  {
    Serial.print("Conecting to wifi network: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);
  }

  // Once we are connected log the IP address of the ESP module
  Serial.print("IP Address of ESP8266 Module is: ");
  Serial.println(WiFi.localIP());
  Serial.println("You're connected to the network");

  pinMode(pirSensor, INPUT);
  pinMode(13, OUTPUT);

  //test
}

void loop()
{

  reading = digitalRead(pirSensor);


  if (reading == HIGH)
  {
    Serial.println("Motion");
    digitalWrite(13, HIGH);

    SendMessage();
    delay (6000);
  }
  else
  {
    Serial.println("0");
    digitalWrite(13, LOW);
  }
  delay(3);// Delay for main loop

}


void SendMessage()
{
  WiFiEspClient client;
  client.stop();
  if (client.connect(server, 4444)) {
    Serial.println("Connected to server");
    client.println("GET /api/CamServer HTTP/1.1");
    client.println("content-type: application/json");

    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();

    // if there are incoming bytes available
    // from the server, read them and print them
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }
  }
}
