#ifndef INFOS_H
#define INFOS_H

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

struct bmp280_infos getBMP280Infos();
void displayBMP280Infos(struct bmp280_infos);

#endif
