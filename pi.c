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

struct bmp280_t bmp280;

#define I2C_BUFFER_LEN 25


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
    int fd = getWiringPiI2CHandle();

    s32 iError = BMP280_INIT_VALUE;
    u8 array[I2C_BUFFER_LEN] = {BMP280_INIT_VALUE};
    u8 stringpos = BMP280_INIT_VALUE;
    array[BMP280_INIT_VALUE] = reg_addr;

    u8 offset = BMP280_INIT_VALUE;
    for (offset = 0; offset < cnt; offset++)
    {
        array[BMP280_INIT_VALUE + offset] = wiringPiI2CReadReg8(fd, reg_addr + offset);
    }

    for (stringpos = BMP280_INIT_VALUE; stringpos < cnt; stringpos++)
    {
        *(reg_data + stringpos) = array[stringpos];
    }
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
    int fd = getWiringPiI2CHandle();

    s32 iError = BMP280_INIT_VALUE;
    u8 array[I2C_BUFFER_LEN];
    u8 stringpos = BMP280_INIT_VALUE;
    array[BMP280_INIT_VALUE] = reg_addr;
    for (stringpos = BMP280_INIT_VALUE; stringpos < cnt; stringpos++)
    {
        array[stringpos] = *(reg_data + stringpos);
    }

    u8 offset = BMP280_INIT_VALUE;
    for (offset = 0; offset < cnt; offset++)
    {
        wiringPiI2CWriteReg8(fd, reg_addr + offset, array[BMP280_INIT_VALUE + offset]);
    }

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

int getWiringPiI2CHandle()
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


int getBMP280TemperatureAtmosphericPressure(int *temperature, int *pressure, int debug)
{
    int fd = getWiringPiI2CHandle();

    int chip_id = wiringPiI2CReadReg8(fd, BMP280_CHIP_ID_REG);
    if (BMP280_CHIP_ID != chip_id)
    {
        fprintf(stderr, "Unsupported chip: expected id=0x%02x, found id=0x%02x\n", BMP280_CHIP_ID, chip_id);
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

    if (debug) printf("BMP280: Temperature=%5.2fC Pressure=%5.2f hPa\n", (v_actual_temp_s32 / 100.0), (v_actual_press_u32 / 100.0));

    *pressure = (int)v_actual_press_u32;
    *temperature = (int)v_actual_temp_s32;
    return com_rslt;
}
