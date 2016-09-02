
#ifndef PI_H
#define PI_H

#define BMP280_I2C_ADDRESS BMP280_I2C_ADDRESS2
#define BMP280_CHIP_ID BMP280_CHIP_ID3

struct bmp280_infos
{
    u8 i2c_address;
    u8 chip_id;
    u8 status;
    u8 measuring;
    u8 im_update;
    u8 ctrl_meas;
    u8 power_mode;
    u8 osrs_t;
    u8 osrs_p;
    u8 config;
    u8 t_sb;
    u8 filter;
};

void initWiringPi();
void initBMP280();
int getBMP280Handle();
struct bmp280_infos getBMP280Infos();
void displayBMP280Infos(struct bmp280_infos);

#endif