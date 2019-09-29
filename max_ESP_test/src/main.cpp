/*

Demo of a max6675_31855 library
Example for Arduino.

Library is writen for ESP8266, ESP8285 MON-OS native SDK, but should work with Arduino and FreeRTOS.

Copyright (C) 2019 by Jaromir Kopp <macwyznawca at me dot com> under MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <Arduino.h>

#include "max6675_31855.h"

int16_t thermocouple = 0;
int16_t internal = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  maxTherm_init( 5,  4,  14, MAX6675);

  Serial.println();
  Serial.println();
  Serial.println("max6675_31855 test!");

}

void loop() {

  delay(3000);

  maxTherm_readTempInt(&thermocouple, &internal);

  Serial.print("NAX  temp. thermocouple: ");
  Serial.print(thermocouple);
  Serial.print(", temp. interneal: ");
  Serial.print(internal);
  
  Serial.println("");
  
}