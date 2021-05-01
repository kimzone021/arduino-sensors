#define pirSensor 8

#include "WiFiEsp.h"
#include <ArduinoJson.h>

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"

// set up software serial to allow serial communication to our TX and RX pins
SoftwareSerial Serial1(6, 7);
#endif

char ssid[] = "FunkyNet";
char pass[] = "Smart2homeZone7";
int status = WL_IDLE_STATUS;

char serverIp[] = "192.168.1.127";

String endpoint = "GET /api/CamServer/pirstair";
String payload = "";

int reading;
bool _manualSwitch = false;

// Define an esp server that will listen on port 80
WiFiEspServer server(80);

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

  server.begin();
}

void loop()
{

  if (_manualSwitch != true)
  {
    reading = digitalRead(pirSensor);

    if (reading == HIGH)
    {
      Serial.println("Motion");
      digitalWrite(13, HIGH);

      payload = String("?stair=on HTTP/1.1");
      SendMessage(payload);
      delay (6000);
      payload = String("?stair=off HTTP/1.1");
      SendMessage(payload);
      digitalWrite(13, LOW);
    }
    else
    {
      Serial.println("0");

    }
  }
  ReceiveMessage();

  delay(3);// Delay for main loop

}


void SendMessage(String payload)
{
  WiFiEspClient client;
  client.stop();
  if (client.connect(serverIp, 4444)) {
    Serial.println("Connected to server");
    client.println(endpoint + payload);
    client.println("content-type: application/json");

    client.print("Host: ");
    client.println(serverIp);
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

void ReceiveMessage()
{
  // listen for incoming clients
  WiFiEspClient client = server.available();
  if (client) {
    Serial.println("New client");

    if (client.available())
    {
      String jsonStrWithoutBrackets = client.readStringUntil('\r');
      Serial.println(jsonStrWithoutBrackets);

      bool payloadOk = false;
      if (jsonStrWithoutBrackets.indexOf("on") >= 0)
      {
        Serial.println("on");
        payloadOk = true;
        digitalWrite(13, HIGH);
        _manualSwitch = true;
      }
      else if (jsonStrWithoutBrackets.indexOf("off") >= 0)
      {
        Serial.println("off");
        payloadOk = true;
        digitalWrite(13, LOW);
        _manualSwitch = false;
      }
      else
      {
        Serial.println("unknown command");
        payloadOk = false;
      }

      if (payloadOk)
      {
        // send response and close connection

        client.println("HTTP/1.1 200 OK");
        client.println("Access-Control-Allow-Origin: *");
        client.println("Access-Control-Allow-Methods: POST");
        client.println("Access-Control-Allow-Methods: GET");
        client.println("content-type: application/json");
        client.println("Connection: close");
        client.println();

        client.stop();
      }
      else
      {
        // we were unable to parse json, send http error status and close connection
        client.println("HTTP/1.1 500 ERROR");
        client.println("Access-Control-Allow-Origin: *");
        client.println("Access-Control-Allow-Methods: POST");
        client.println("Access-Control-Allow-Methods: GET");
        client.println("content-type: application/json");
        client.println("Connection: close");
        client.println();

        Serial.println("Error, bad or missing json");
        client.stop();
      }
    }

    delay(10);
    client.stop();
    Serial.println("Client disconnected");
  }
}
