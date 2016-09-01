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

static int fd = 0;

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
    printf("Reading device=0x%02x register=0x%02x count=%d\n", dev_addr, reg_addr, cnt);
    BMP280_delay_msek(250);

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

    printf("Result: ");
    for (stringpos = BMP280_INIT_VALUE; stringpos < cnt; stringpos++)
    {
        printf("%02x ", array[stringpos]);
        *(reg_data + stringpos) = array[stringpos];
    }
    printf("\n\n");
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
    printf("Writing device=0x%02x register=0x%02x value=0x%02x count=%d\n", dev_addr, reg_addr, *reg_data, cnt);
    BMP280_delay_msek(250);

    s32 iError = BMP280_INIT_VALUE;
    u8 array[I2C_BUFFER_LEN];
    u8 stringpos = BMP280_INIT_VALUE;
    array[BMP280_INIT_VALUE] = reg_addr;
    for (stringpos = BMP280_INIT_VALUE; stringpos < cnt; stringpos++)
    {
        array[stringpos + BMP280_DATA_INDEX] = *(reg_data + stringpos);
    }

    //printf("Writing value=0x%02x\n", array[BMP280_INIT_VALUE + BMP280_DATA_INDEX]);
    wiringPiI2CWriteReg8(fd, reg_addr, array[BMP280_INIT_VALUE + BMP280_DATA_INDEX]);
    BMP280_delay_msek(250);

    return (s8)iError;
}

/*--------------------------------------------------------------------------*
*	The following function is used to map the I2C bus read, write, delay and
*	device address with global structure bmp280_t
*-------------------------------------------------------------------------*/
s8 I2C_routine(void)
{
    /*--------------------------------------------------------------------------*
 *  By using bmp280 the following structure parameter can be accessed
 *	Bus write function pointer: BMP280_WR_FUNC_PTR
 *	Bus read function pointer: BMP280_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *	I2C address: dev_addr
 *--------------------------------------------------------------------------*/
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

void initBMP280()
{
    if ((fd = wiringPiI2CSetup(BMP280_I2C_ADDRESS)) < 0)
    {
        fprintf(stderr, "Unable to open I2C device: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int chip_id = wiringPiI2CReadReg8(fd, BMP280_CHIP_ID_REG);
    if (BMP280_CHIP_ID != chip_id)
    {
        fprintf(stderr, "Unsupported chip: expected id=%d, found id=%d\n", BMP280_CHIP_ID, chip_id);
        exit(EXIT_FAILURE);
    }

    I2C_routine();

    /* The variable used to assign the standby time*/
    u8 v_standby_time_u8 = BMP280_INIT_VALUE;

    /* The variables used in individual data read APIs*/
    /* The variable used to read uncompensated temperature*/
    s32 v_data_uncomp_tem_s32 = BMP280_INIT_VALUE;
    /* The variable used to read uncompensated pressure*/
    s32 v_data_uncomp_pres_s32 = BMP280_INIT_VALUE;
    /* The variable used to read real temperature*/
    s32 v_actual_temp_s32 = BMP280_INIT_VALUE;
    /* The variable used to read real pressure*/
    u32 v_actual_press_u32 = BMP280_INIT_VALUE;

    /* The variables used in combined data read APIs*/
    /* The variable used to read uncompensated temperature*/
    s32 v_data_uncomp_tem_combined_s32 = BMP280_INIT_VALUE;
    /* The variable used to read uncompensated pressure*/
    s32 v_data_uncomp_pres_combined_s32 = BMP280_INIT_VALUE;
    /* The variable used to read real temperature*/
    s32 v_actual_temp_combined_s32 = BMP280_INIT_VALUE;
    /* The variable used to read real pressure*/
    u32 v_actual_press_combined_u32 = BMP280_INIT_VALUE;

    s32 com_rslt = ERROR;
    com_rslt = bmp280_init(&bmp280);
    if (BMP280_CHIP_ID_READ_SUCCESS == com_rslt)
    {
        printf("Found Chip ID=0x%02x, address=0x%02x\n", bmp280.chip_id, bmp280.dev_addr);
    }
    com_rslt += bmp280_set_power_mode(BMP280_FORCED_MODE);
    com_rslt += bmp280_set_work_mode(BMP280_ULTRA_LOW_POWER_MODE);
    com_rslt += bmp280_set_standby_durn(BMP280_STANDBY_TIME_250_MS);

    com_rslt += bmp280_get_standby_durn(&v_standby_time_u8);
    if (com_rslt > 0) exit(EXIT_FAILURE);

    /* API is used to read the uncompensated temperature*/
    com_rslt += bmp280_read_uncomp_temperature(&v_data_uncomp_tem_s32);
    if (com_rslt > 0) exit(EXIT_FAILURE);

    /* API is used to read the uncompensated pressure*/
    com_rslt += bmp280_read_uncomp_pressure(&v_data_uncomp_pres_s32);
    if (com_rslt > 0) exit(EXIT_FAILURE);

    /* API is used to read the true temperature*/
    /* Input value as uncompensated temperature*/
    v_actual_temp_s32 = bmp280_compensate_temperature_int32(v_data_uncomp_tem_s32);
    if (com_rslt > 0) exit(EXIT_FAILURE);

    /* API is used to read the true pressure*/
    /* Input value as uncompensated pressure*/
    v_actual_press_u32 = bmp280_compensate_pressure_int32(v_data_uncomp_pres_s32);
    if (com_rslt > 0) exit(EXIT_FAILURE);

    /* API is used to read the uncompensated temperature and pressure*/
    com_rslt += bmp280_read_uncomp_pressure_temperature(&v_data_uncomp_pres_combined_s32,
                                                        &v_data_uncomp_tem_combined_s32);

    /* API is used to read the true temperature and pressure*/
    com_rslt += bmp280_read_pressure_temperature(&v_actual_press_combined_u32,
                                                 &v_actual_temp_combined_s32);

    com_rslt += bmp280_set_power_mode(BMP280_SLEEP_MODE);

    printf("Temperature=%d Pressure=%d, combined temperature=%d and pressure=%d\n", v_actual_temp_s32, v_actual_press_u32, v_actual_temp_combined_s32, v_actual_press_combined_u32);
}
