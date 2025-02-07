/*
 * HelTec Dev boards OLED string buffer example.
 */
#ifndef _TEXT_SCROLLS_H
#define _TEXT_SCROLLS_H

#include "Arduino.h"
#include "heltec.h"

const int SCREEN_SIZE = 6;

unsigned int numLines = 0;
String *lineBuffer[SCREEN_SIZE];

void clearBuffer() {
  for (int i=0; i<numLines; ++i)
    delete lineBuffer[i];
  numLines = 0;
}

void print(const char * line) {
  if (numLines == SCREEN_SIZE) {
    delete lineBuffer[0];
    for (int i=1; i<numLines; ++i)
      lineBuffer[i - 1] = lineBuffer[i];
    lineBuffer[SCREEN_SIZE - 1] = new String(line);
  }
  else {
    lineBuffer[numLines++] = new String(line);
  }

  Heltec.display->clear();
  for (int i=0; i<numLines; ++i)
    Heltec.display->drawString(0, i * 10, lineBuffer[i]->c_str());
  Heltec.display->display();
}

void print(const String& s) {
  print(s.c_str());
}

#endif // _TEXT_SCROLLS_H
