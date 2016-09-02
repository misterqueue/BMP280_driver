#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "bmp280.h"
#include "pi.h"

struct bmp280_t bmp280;

#define I2C_BUFFER_LEN 25
#define BUFFER_LENGTH 0xFF
#define BMP280_DATA_INDEX 1
#define BMP280_ADDRESS_INDEX 2

/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BMP280_delay_msek(u32 msek)
{
    usleep(msek);
}

/*	\Brief: The function is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, where data is going to be read
 *	\param reg_data : This is the data read from the sensor, which is held in an array
 *	\param cnt : The no of data to be read
 */
s8 BMP280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    int fd = getBMP280Handle();
    //printf("Reading device=0x%02x register=0x%02x count=%d\n", dev_addr, reg_addr, cnt);

    s32 iError = BMP280_INIT_VALUE;
    u8 array[I2C_BUFFER_LEN] = {BMP280_INIT_VALUE};
    u8 stringpos = BMP280_INIT_VALUE;
    array[BMP280_INIT_VALUE] = reg_addr;

    u8 offset = BMP280_INIT_VALUE;
    for (offset = 0; offset < cnt; offset++)
    {
        array[BMP280_INIT_VALUE + offset] = wiringPiI2CReadReg8(fd, reg_addr + offset);
        BMP280_delay_msek(1);
    }

    //printf("Result: ");
    for (stringpos = BMP280_INIT_VALUE; stringpos < cnt; stringpos++)
    {
        //printf("%02x ", array[stringpos]);
        *(reg_data + stringpos) = array[stringpos];
    }
    //printf("\n\n");
    iError = SUCCESS;
    return (s8)iError;
}

/*	\Brief: The function is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, where data is to be written
 *	\param reg_data : It is a value held in the array,
 *		which is written in the register
 *	\param cnt : The no of bytes of data to be written
 */
s8 BMP280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    int fd = getBMP280Handle();
    //printf("Writing device=0x%02x register=0x%02x value=0x%02x count=%d\n", dev_addr, reg_addr, *reg_data, cnt);

    s32 iError = BMP280_INIT_VALUE;
    u8 array[I2C_BUFFER_LEN];
    u8 stringpos = BMP280_INIT_VALUE;
    array[BMP280_INIT_VALUE] = reg_addr;
    for (stringpos = BMP280_INIT_VALUE; stringpos < cnt; stringpos++)
    {
        array[stringpos + BMP280_DATA_INDEX] = *(reg_data + stringpos);
    }

    wiringPiI2CWriteReg8(fd, reg_addr, array[BMP280_INIT_VALUE + BMP280_DATA_INDEX]);

    return (s8)iError;
}

/*--------------------------------------------------------------------------*
*	The following function is used to map the I2C bus read, write, delay and
*	device address with global structure bmp280_t
*-------------------------------------------------------------------------*/
s8 I2C_routine(void)
{
    bmp280.bus_write = BMP280_I2C_bus_write;
    bmp280.bus_read = BMP280_I2C_bus_read;
    bmp280.dev_addr = BMP280_I2C_ADDRESS2;
    bmp280.delay_msec = BMP280_delay_msek;

    return BMP280_INIT_VALUE;
}

void initWiringPi()
{
    static int initialized = 0;

    if (initialized == 0)
    {
        wiringPiSetup();
        piHiPri(55);
        initialized = 1;
    }
}

int getBMP280Handle()
{
    static int fd = 0;

    if (fd == 0)
    {
        if ((fd = wiringPiI2CSetup(BMP280_I2C_ADDRESS)) < 0)
        {
            fprintf(stderr, "Unable to open I2C device: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    return fd;
}

struct bmp280_infos getBMP280Infos()
{
    struct bmp280_infos infos;
    infos.i2c_address = BMP280_I2C_ADDRESS;

    int fd = getBMP280Handle();
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
    printf("I2C address: 0x%02x\n", infos.i2c_address);
    printf("Chip ID: 0x%02x\n", infos.chip_id);

    printf("Status register: 0x%02x\n", infos.status);
    printf("  Measuring: %s\n", infos.measuring ? "Yes" : "No");
    printf("  Updating Image Registers: %s (data is being copied from NVM to image registers)\n", infos.im_update ? "Yes" : "No");

    printf("Ctrl meas register: 0x%02x\n", infos.ctrl_meas);
    printf("  Power mode: 0x%02x ", infos.power_mode);
    if (infos.power_mode == 0)
    {
        printf("= Sleep mode\n");
    }
    else if (infos.power_mode == 3)
    {
        printf("= Normal mode\n");
    }
    else
    {
        printf("= Forced mode\n");
    }
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

void initBMP280()
{
    int fd = getBMP280Handle();

    int chip_id = wiringPiI2CReadReg8(fd, BMP280_CHIP_ID_REG);
    if (BMP280_CHIP_ID != chip_id)
    {
        fprintf(stderr, "Unsupported chip: expected id=%d, found id=%d\n", BMP280_CHIP_ID, chip_id);
        exit(EXIT_FAILURE);
    }

    I2C_routine();

    s32 v_actual_temp_s32 = BMP280_INIT_VALUE;
    u32 v_actual_press_u32 = BMP280_INIT_VALUE;

    s32 com_rslt = ERROR;
    com_rslt = bmp280_init(&bmp280);

    com_rslt += bmp280_set_filter(BMP280_FILTER_COEFF_OFF);
    com_rslt += bmp280_set_work_mode(BMP280_ULTRA_LOW_POWER_MODE);
    com_rslt += bmp280_set_power_mode(BMP280_FORCED_MODE);

    // In forced mode, the measure can take a maximum of 6.4ms
    usleep(10);

    com_rslt += bmp280_read_pressure_temperature(&v_actual_press_u32,
                                                 &v_actual_temp_s32);

    com_rslt += bmp280_set_power_mode(BMP280_SLEEP_MODE);

    printf("Temperature=%d Pressure=%d\n", v_actual_temp_s32, v_actual_press_u32);
}
