/*
Function for MAX31855 and MAX6675 Cold-Junction Compensated Thermocouple-to-Digital Converter

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

#ifndef MAX_THERMO_H_
#define MAX_THERMO_H_

typedef enum {
    MAX_OK = 0,
    MAX_NO_THERMOCOUPLE = -1, 
    MAX_THERMOCOUPLE_SHORT_GND = -2, 
    MAX_THERMOCOUPLE_SHORT_VCC = -3, 
} max_error_t;

typedef enum {
    MAX6675 = 0,
    MAX31855, 
} max_model_t;

#ifdef __cplusplus
extern "C"
{
#endif

// DEBUG OPTIONS
//#define DEBUG_ON

#if defined(DEBUG_ON)
#define INFO( format, ... ) os_printf( format, ## __VA_ARGS__ )
#else
#define INFO( format, ... )
#endif

#include "c_types.h"

/*
 * maxTherm_init() initialize function, set GPIOs and model
 * icsPin - CS, SS (cable select, slave select, active low pin)
 * clockPin - SCK, CLK, SCLK (clock pin)
 * soPin - SO, DO, MISO (data in pin) 
 * model - MAX6675 (0) or MAX31855 (1)
 * 
 * Ex. use:
    maxTherm_init( 5,  4,  14, MAX31855);
 *
 */

void maxTherm_init(uint8_t icsPin, uint8_t clockPin, uint8_t soPin, max_model_t model) ;

/* 
 * maxTherm_readTempInt() read data from MAX module
 * tempCouple - pointer to int16_t for termocouple temperature valid for MAX6675 and MAX31855
 * value x10 (982 = 98.2°C)
 * 
 * tempInternal - pointer to int16_t for internal reference temperature valid for MAX31855
 * value x100 (2337 = 23.37°C)
 * 
 * For MAX6675 tempInternal value don't change
 * 
 * Return success (MAX_OK = 0) or error (negatuve value, see max_error_t enum)
 * 
 * Ex. use:
    int16_t thermocouple = 0;
    int16_t internal = 0;
    maxTherm_readTempInt(&thermocouple, &internal);

    os_printf("\nMAX  temp. thermocouple: %d, temp. interneal: %d\n",thermocouple,internal);
 * 
 * For MAX6675 internal value don't care (0)
 */
max_error_t maxTherm_readTempInt(int16_t *tempCouple, int16_t *tempInternal);

#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif // MAX_THERMO_H_
