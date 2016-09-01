#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "bmp280.h"
#include "pi.h"

/*
 ***********************************************************************
 * The main program
 ***********************************************************************
 */

int main(int argc, char **argv)
{
    initWiringPi();
    initBMP280();
    return EXIT_SUCCESS;
}