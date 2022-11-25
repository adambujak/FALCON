/******************************************************************************
* @file     baro.h
* @brief    Barometer Header File (MPL3115A2)
* @version  1.0
* @date     2020-11-11
* @author   Devin Bell, Adam Bujak
******************************************************************************/
#ifndef BARO_H
#define BARO_H

int baro_init(int sample_rate);
int baro_calibrate(void);
void baro_get_altitude(float *alt_data);

#endif // BARO_H
