#include <WiFi.h>
#include "heltec.h"
#include "text_scrolls.h"
#include <WiFiClient.h>
#include <WiFiAP.h>

const char* ssid = "mustang2";      // Change this to your WiFi SSID
const char* password = "password";  // Change this to your WiFi password

// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
int ADC_Max = 4096;  // This is the default ADC max value on the ESP32 (12 bit ADC width);
                     // this width can be set (in low-level oode) from 9-12 bits, for a
                     // a range of max values of 512-4096
//refrence nums: int flex_Min = 2048; int flex_Max = 2600;

int flex_Min = 1000;
int flex_Max = 2300;

int sensors[5] = { 5, 4, 3, 2, 1 };
int vals[5] = { 0, 0, 0, 0, 0 };
int rawVals[5] = { 0, 0, 0, 0, 0 };
int prevVals[5] = { 0, 0, 0, 0, 0 };
int minRaw[5] = { 1102, 1416, 1186, 1191, 1317 };  //1215, 1227, 1259, 1481, 2227
int maxRaw[5] = { 2643, 3001, 2239, 2745, 2613 };  //2650, 2437, 2515, 2698, 4095

void setup() {
  Serial.begin(115200);
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  for (int i = 0; i < 5; i++) {
    vals[i] = rawVals[i] = 0;
    prevVals[i] = -11;
  }

  print("Done with set up");
}

void debugPrint(int sensorNum, int reading) {
  char buf[256];
  sprintf(buf, "Snr %d reading = [%d]", sensorNum, reading);
  print(buf);
}

void rawDebugPrint(int sensorNum, int rawReading) {
  char buf[256];
  sprintf(buf, "Snr %d RawRead = [%d]", sensorNum, rawReading);
  print(buf);
}

void minMaxDebugPrint(int sensorNum, int min, int max) {
  char buf[256];
  sprintf(buf, "Snr%d Min[%d] Max[%d]", sensorNum, min, max);
  print(buf);
}

long ourMap(long x, long in_min, long in_max, long out_min, long out_max) {
  const long run = in_max - in_min;
  if (run == 0) {
    log_e("ourMap(): Invalid input range, min == max");
    return -1;  // AVR returns -1, SAM returns 0
  }
  const long rise = out_max - out_min;
  const long delta = x - in_min;
  long result = (delta * rise) / run + out_min;
  result = min(out_max, result);
  result = max(out_min, result);
  return result;
}

int minRawSeen[5] = { 4096, 4096, 4096, 4096, 4096 };  //1215, 1227, 1259, 1481, 2227
int maxRawSeen[5] = { 0, 0, 0, 0, 0 };                 //2650, 2437, 2515, 2698, 4095

void minMaxDebugPrint() {
  char buf[256];
  Heltec.display->clear();
  for (int i = 0; i < 5; ++i) {
    sprintf(buf, "%d: %d,%d [%d]", i, minRawSeen[i], maxRawSeen[i], vals[i]);
    Heltec.display->drawString(0, i * 10, buf);
  }
  Heltec.display->display();
}

void loop() {
  for (int i = 0; i < 5; i++) {
    vals[i] = rawVals[i] = analogRead(sensors[i]);
    vals[i] = ourMap(vals[i], minRaw[i], maxRaw[i], 0, 180);  // scale it to use it with the servo (value between 0 and 180)

    minRawSeen[i] = (rawVals[i] < minRawSeen[i]) ? rawVals[i] : minRawSeen[i];
    maxRawSeen[i] = (rawVals[i] > maxRawSeen[i]) ? rawVals[i] : maxRawSeen[i];
  }

  for (int i = 0; i < 5; i++) {
    if (i == 0 || i == 1) {  // i is reffring to the servo number
      vals[i] = 180 - vals[i];
    }
    if (vals[i] >= (prevVals[i] + 10) || vals[i] <= (prevVals[i] - 10)) {
      prevVals[i] = vals[i];
      // debugPrint(i, vals[i]);
    }
  }

  delay(100);
  minMaxDebugPrint();
}
