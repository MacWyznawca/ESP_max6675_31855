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

#include "max6675_31855.h"

#include "gpio.h"
#include "ets_sys.h"
#include "osapi.h"
#include "math.h"

LOCAL uint16_t _ics=5;   
LOCAL uint16_t _clk=4; 
LOCAL uint16_t _so=14;

LOCAL bool maxTherm_isConfigured = false;

LOCAL uint8_t maxModel = 0; // 0 - MAX6675, 1 - MAX31855

LOCAL void maxTherm_delay_between_clock(void);
LOCAL uint8_t maxTherm_readByte(void);

// some arduino lookalike methods :)
#define LOW false
#define HIGH true

#define NUM_VALID_INPUT_PINS 11
static const uint32_t pin_mux[NUM_VALID_INPUT_PINS] = {PERIPHS_IO_MUX_GPIO0_U,PERIPHS_IO_MUX_U0TXD_U,PERIPHS_IO_MUX_GPIO2_U,PERIPHS_IO_MUX_U0RXD_U,PERIPHS_IO_MUX_GPIO4_U,PERIPHS_IO_MUX_GPIO5_U,PERIPHS_IO_MUX_MTDI_U,PERIPHS_IO_MUX_MTCK_U,PERIPHS_IO_MUX_MTMS_U,PERIPHS_IO_MUX_MTDO_U,0};
static const uint8_t pin_num[NUM_VALID_INPUT_PINS] = {0,1,2,3,4,5,12,13,14,15,16};
static const uint8_t pin_func[NUM_VALID_INPUT_PINS] = {FUNC_GPIO0,FUNC_GPIO1,FUNC_GPIO2,FUNC_GPIO3,FUNC_GPIO4,FUNC_GPIO5,FUNC_GPIO12,FUNC_GPIO13,FUNC_GPIO14,FUNC_GPIO15,0};

int8_t ICACHE_FLASH_ATTR get_pin_index(uint8_t pin){
	int i = 0;
	for(i = 0; i < NUM_VALID_INPUT_PINS; i++){
		if(pin==pin_num[i]){
			return i;
		}
	}
	return -1;
}

uint32_t ICACHE_FLASH_ATTR get_pin_mux(uint8_t pin){
  uint8_t id = get_pin_index(pin);
  return id == -1 ? -1 : pin_mux[get_pin_index(pin)];
}

uint8_t ICACHE_FLASH_ATTR get_pin_func(uint8_t pin){
  uint8_t id = get_pin_index(pin);
  return id == -1 ? -1 : pin_func[get_pin_index(pin)];
}

void ICACHE_FLASH_ATTR maxTherm_delay_between_clock(void) {
  os_delay_us(100);
}

uint8_t ICACHE_FLASH_ATTR maxTherm_readByte(void) {
  int i;
  uint8_t d = 0;
  for (i = 7; i >= 0; i--)
  {
    GPIO_OUTPUT_SET(_clk, LOW);
    maxTherm_delay_between_clock();
    if (GPIO_INPUT_GET(_so) != 0) {
      d |= (1 << i);
    }
    GPIO_OUTPUT_SET(_clk, HIGH);
    maxTherm_delay_between_clock();
  }
  return d;
}


max_error_t ICACHE_FLASH_ATTR maxTherm_readTempInt(int16_t *tempCouple, int16_t *tempInternal){
  if (!maxTherm_isConfigured) {
    INFO("MAX module is not configured.\n");
    return false;
  }

  int32_t maxDataRaw = 0;

  GPIO_OUTPUT_SET(_ics, LOW);
  maxTherm_delay_between_clock();
  maxDataRaw = maxTherm_readByte();
  maxDataRaw <<= 8;
  maxDataRaw |= maxTherm_readByte();
  if(maxModel == MAX31855){
    maxDataRaw <<= 8;
    maxDataRaw |= maxTherm_readByte();
    maxDataRaw <<= 8;
    maxDataRaw |= maxTherm_readByte();
  }
  GPIO_OUTPUT_SET(_clk, LOW);
  maxTherm_delay_between_clock();
  GPIO_OUTPUT_SET(_ics, HIGH);
  if(maxModel == MAX6675){
    if (maxDataRaw & 0x4) {
      INFO("MAX6675: no thermocouple.\n", maxDataRaw);
      return MAX_NO_THERMOCOUPLE;
    }
    maxDataRaw >>= 3;
    *tempCouple = (maxDataRaw * 25) / 10;
  }
  else {
    if (maxDataRaw & 0x1) {
      INFO("MAX31855: no thermocouple.\n", maxDataRaw);
      return MAX_NO_THERMOCOUPLE;
    }
    else if (maxDataRaw & 0x2) {
      INFO("MAX31855: thermocouple is short to GND.\n", maxDataRaw);
      return MAX_THERMOCOUPLE_SHORT_GND;
    }
    else if (maxDataRaw & 0x4) {
      INFO("MAX31855: thermocouple is short to VCC.\n", maxDataRaw);
      return MAX_THERMOCOUPLE_SHORT_VCC;
    }
    maxDataRaw >>= 4;

    *tempInternal = maxDataRaw & 0xfff;
    if (maxDataRaw & 0x800){ // Negative sign  check
      *tempInternal = -*tempInternal & 0xfff; 
      *tempInternal = (*tempInternal * -625) / 100;
    }
    else {
      *tempInternal = (*tempInternal * 625) / 100;
    }
   
    maxDataRaw >>= 14;
    if (maxDataRaw & 0x2000){ // Negative sign  check
      *tempCouple = maxDataRaw;
      *tempCouple = -*tempCouple & 0x3fff; 
      *tempCouple = (*tempCouple * -25) / 10;
    }
    else {
      *tempCouple = (maxDataRaw * 25) / 10;
    }
  }
  return MAX_OK;
}


void ICACHE_FLASH_ATTR maxTherm_init(uint8_t icsPin, uint8_t clockPin, uint8_t soPin, max_model_t model ) {

  _ics = icsPin;
  _clk = clockPin;
  _so = soPin;

  maxModel = model;

  PIN_FUNC_SELECT(get_pin_mux(_ics), get_pin_func(_ics));
  PIN_PULLUP_EN(get_pin_mux(_ics));

  PIN_FUNC_SELECT(get_pin_mux(_clk), get_pin_func(_clk));
  PIN_PULLUP_EN(get_pin_mux(_clk));

  PIN_FUNC_SELECT(get_pin_mux(_so), get_pin_func(_so));
  PIN_PULLUP_DIS(get_pin_mux(_so));
  GPIO_DIS_OUTPUT(GPIO_ID_PIN(_so));

  GPIO_OUTPUT_SET(_ics, true);
  GPIO_OUTPUT_SET(_clk, false);

  INFO("\nInitiated max6675\n");
  INFO(" iCS   = GPIO%d\n", _ics);
  INFO(" CLOCK = GPIO%d\n", _clk);
  INFO(" SO    = GPIO%d\n", _so);

  maxTherm_isConfigured = true;
}
