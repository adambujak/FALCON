/******************************************************************************
 * @file     frf.h
 * @brief    Falcon RF Library Header File  
 * @version  1.0
 * @date     2020-06-07
 * @author   Adam Bujak
 ******************************************************************************/

#include "hal_nrf.h"

#include <stdbool.h>
#include <intrins.h>

#define RF_TEST_MAX_CHANNEL 81   //!< Upper radio channel

typedef enum {
  FRF_DIR_RX = 0,
  FRF_DIR_TX
} frf_direction_t;

typedef struct {
  frf_direction_t direction; 
  drv_gpio_t ce_set;
  drv_gpio_t cs_set;
} frf_config_t;

typedef struct {
  frf_direction_t direction;
  drv_gpio_t      CESet;
  nRF24L01_t     *rfInstance;
} frf_t;

void frf_init(frf_t *instance, frf_config_t config);
