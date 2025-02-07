/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/
#define WIFI_LoRa_32
#include "heltec.h"
#include "text_scrolls.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
//#include <ESP32Servo.h>
#include <pwmWrite.h>

// Set these to your desired credentials.
const char *ssid = "mustang2";
const char *password = "password";

// for the ESP32-S3 the GPIO pins are 1-21,35-45,47-48

Pwm pwm = Pwm();

int servoPins[5] = {5,4,3,2,1};

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  print("Configuring access point...");
  delay(2000);

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password)) {
    print("Soft AP creation failed.");
    while(1);
  }
  IPAddress myIP = WiFi.softAPIP();
  print("AP IP address: ");
  print(myIP.toString());
  server.begin();
  delay(1000);
  clearBuffer();
  print("Server Started");
  delay(1000);
}

void debugPrint(String& line, int angle, int servoNum) {
  char buf[256];
  sprintf(buf, "Servo%d angle [%s] = [%d]", servoNum, line.c_str(), angle);
  print(buf);
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    print("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      while (client.available() && client.connected()) {
        char c = char(client.read());             // read a byte, then
        if (c == '\n')                    // if the byte is a newline character
          break;
        else if (c != '\r')  // if you got anything else but a carriage return character,
          currentLine += c;  // add it to the end of the currentLine
      }
      if (currentLine.length() > 0) {
        int servoNumber = (int)currentLine.charAt(0) - (int)'0';
        int angle = atoi(currentLine.substring(2).c_str());
        debugPrint(currentLine, angle, servoNumber);
        pwm.writeServo(servoPins[servoNumber], angle);
        currentLine.clear();
      }
    }
    // close the connection:
    client.stop();
    print("Client Disconnected.");
  }
}