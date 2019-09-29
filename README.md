# ESP_max6675_31855
Library for MAX31855 and MAX6675 Cold-Junction Compensated Thermocouple-to-Digital Converter  writen for ESP8266, ESP8285 NON-OS native SDK, but should work with Arduino and FreeRTOS.

**************************************

**maxTherm_init()** initialize function, set GPIOs and model

**icsPin** - CS, SS (cable select, slave select, active low pin)

**clockPin** - SCK, CLK, SCLK (clock pin)

**soPin** - SO, DO, MISO (data in pin) 

**model** - MAX6675 (0) or MAX31855 (1)

**************************************

*Ex. use:*

**maxTherm_init( 5,  4,  14, MAX31855);**

**************************************

**maxTherm_readTempInt()** read data from MAX module

**tempCouple** - pointer to int16_t for termocouple temperature valid for MAX6675 and MAX31855

value x10 (982 = 98.2°C)

**tempInternal** - pointer to int16_t for internal reference temperature valid for MAX31855

value x100 (2337 = 23.37°C)

For MAX6675 tempInternal value don't change

Return success (MAX_OK = 0) or error (negatuve value, see max_error_t enum)

**************************************

*Ex. use:*

int16_t **thermocouple** = 0;

int16_t **internal** = 0;

**maxTherm_readTempInt(&thermocouple, &internal);**

**os_printf("\nMAX  temp. thermocouple: %d, temp. interneal: %d\n",thermocouple,internal);**

For MAX6675 internal value don't care (0)

