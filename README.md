# Introduction

I'm using a Pimoroni Enviro Phat (with a BMP280 on the board) and I was only able to find a [library for Python](https://github.com/pimoroni/enviro-phat).

For a personal project, I had to make a library to use the BMP280 from a Raspberry Pi in C (using the excellent [wiringPi library](http://wiringpi.com)).

The library is in the early stage of development, but it does work in _Weather monitoring (lowest power)_ mode.

# Example

``` c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "bmp280.h"
#include "pi.h"
#include "infos.h"

int main(int argc, char **argv)
{
    int sensor_temperature = 0;
    int sensor_pressure = 0;
    int debug = 0;

    // Initialize wiring Pi library
    initWiringPi();

    // Display information about your BMP280 sensor
    displayBMP280Infos(getBMP280Infos());

    // Get current temperature and pressure values (integers values * 100)
    int return_value = getBMP280TemperatureAtmosphericPressure(&sensor_temperature, &sensor_pressure, debug);
    
    printf("BMP280 library returned %d - temperature: %5.2fC, atmospheric pressure: %5.2f hPa\n", 
            return_value, 
            sensor_temperature / 100.0, 
            sensor_pressure / 100.0);
    return EXIT_SUCCESS;
}
```