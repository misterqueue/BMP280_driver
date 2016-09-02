#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "bmp280.h"
#include "pi.h"
#include "infos.h"

struct bmp280_infos getBMP280Infos()
{
    struct bmp280_infos infos;
    infos.i2c_address = BMP280_I2C_ADDRESS;

    int fd = getWiringPiI2CHandle();
    infos.chip_id = wiringPiI2CReadReg8(fd, BMP280_CHIP_ID_REG);

    infos.status = wiringPiI2CReadReg8(fd, BMP280_STAT_REG);
    infos.measuring = BMP280_GET_BITSLICE(infos.status, BMP280_STATUS_REG_MEASURING);
    infos.im_update = BMP280_GET_BITSLICE(infos.status, BMP280_STATUS_REG_IM_UPDATE);

    infos.ctrl_meas = wiringPiI2CReadReg8(fd, BMP280_CTRL_MEAS_REG);
    infos.power_mode = BMP280_GET_BITSLICE(infos.ctrl_meas, BMP280_CTRL_MEAS_REG_POWER_MODE);
    infos.osrs_t = BMP280_GET_BITSLICE(infos.ctrl_meas, BMP280_CTRL_MEAS_REG_OVERSAMP_TEMPERATURE);
    infos.osrs_p = BMP280_GET_BITSLICE(infos.ctrl_meas, BMP280_CTRL_MEAS_REG_OVERSAMP_PRESSURE);

    infos.config = wiringPiI2CReadReg8(fd, BMP280_CONFIG_REG);
    infos.t_sb = BMP280_GET_BITSLICE(infos.config, BMP280_CONFIG_REG_STANDBY_DURN);
    infos.filter = BMP280_GET_BITSLICE(infos.config, BMP280_CONFIG_REG_FILTER);

    return infos;
}

void displayPowerMode(u8 value)
{
    if (value == 0)
    {
        printf("= Sleep mode\n");
    }
    else if (value == 3)
    {
        printf("= Normal mode\n");
    }
    else
    {
        printf("= Forced mode\n");
    }
}

void displayOversampling(u8 value)
{
    if (value == BMP280_OVERSAMP_SKIPPED)
    {
        printf("= Skipped\n");
    }
    else if (value == BMP280_OVERSAMP_1X)
    {
        printf("= 1x\n");
    }
    else if (value == BMP280_OVERSAMP_2X)
    {
        printf("= 2x\n");
    }
    else if (value == BMP280_OVERSAMP_4X)
    {
        printf("= 4x\n");
    }
    else if (value == BMP280_OVERSAMP_8X)
    {
        printf("= 8x\n");
    }
    else if (value >= BMP280_OVERSAMP_16X)
    {
        printf("= 16x\n");
    }
}

void displayFilter(u8 value)
{
    if (value == BMP280_FILTER_COEFF_OFF)
    {
        printf("= Off\n");
    }
    else if (value == BMP280_FILTER_COEFF_2)
    {
        printf("= coefficient 2\n");
    }
    else if (value == BMP280_FILTER_COEFF_4)
    {
        printf("= coefficient 4\n");
    }
    else if (value == BMP280_FILTER_COEFF_8)
    {
        printf("= coefficient 8\n");
    }
    else if (value == BMP280_FILTER_COEFF_16)
    {
        printf("= coefficient 16\n");
    }
}

void displayStandByTime(u8 value)
{
    if (value == BMP280_STANDBY_TIME_1_MS)
    {
        printf("= 0.5ms\n");
    }
    else if (value == BMP280_STANDBY_TIME_63_MS)
    {
        printf("= 62.5ms\n");
    }
    else if (value == BMP280_STANDBY_TIME_125_MS)
    {
        printf("= 125ms\n");
    }
    else if (value == BMP280_STANDBY_TIME_250_MS)
    {
        printf("= 250ms\n");
    }
    else if (value == BMP280_STANDBY_TIME_500_MS)
    {
        printf("= 500ms\n");
    }
    else if (value == BMP280_STANDBY_TIME_1000_MS)
    {
        printf("= 1s\n");
    }
    else if (value == BMP280_STANDBY_TIME_2000_MS)
    {
        printf("= 2s\n");
    }
    else if (value == BMP280_STANDBY_TIME_4000_MS)
    {
        printf("= 4s\n");
    }
}

void displayBMP280Infos(struct bmp280_infos infos)
{
    printf("Information about your BMP280 sensor:\n");
    printf("I2C address: 0x%02x\n", infos.i2c_address);
    printf("Chip ID: 0x%02x\n", infos.chip_id);

    printf("Status register: 0x%02x\n", infos.status);
    printf("  Measuring: %s\n", infos.measuring ? "Yes" : "No");
    printf("  Updating Image Registers: %s (data is being copied from NVM to image registers)\n", infos.im_update ? "Yes" : "No");

    printf("Ctrl meas register: 0x%02x\n", infos.ctrl_meas);
    printf("  Power mode: 0x%02x ", infos.power_mode);
    displayPowerMode(infos.power_mode);

    printf("  Temperature oversampling: 0x%02x ", infos.osrs_t);
    displayOversampling(infos.osrs_t);
    printf("  Pressure oversampling: 0x%02x ", infos.osrs_p);
    displayOversampling(infos.osrs_p);

    printf("Config register: 0x%02x\n", infos.config);
    printf("  Inactive duration (in standby mode): 0x%02x ", infos.t_sb);
    displayStandByTime(infos.t_sb);
    printf("  Filter: 0x%02x ", infos.filter);
    displayFilter(infos.filter);
}
