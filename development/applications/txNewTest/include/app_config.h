/******************************************************************************
 * @file     app_config.h
 * @brief    RF app config header file
 * @version  1.0
 * @date     2020-07-06
 * @author   Adam Bujak
 ******************************************************************************/

#ifndef FALCON_RF_APP_CONFIG_H
#define FALCON_RF_APP_CONFIG_H

#define RF_SPI_INSTANCE    0
#define RF_SPI_MOSI_PIN    29
#define RF_SPI_MISO_PIN    30
#define RF_SPI_SCK_PIN     26
#define RF_SPI_SS_PIN      31
#define RF_CE_PIN          5
#define RF_IRQ_PIN         0

#define MAX_STRING_LENGTH  10
#define TEST_STRING       "TX String"

#endif // FALCON_RF_APP_CONFIG_H
