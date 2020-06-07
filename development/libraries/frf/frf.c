/******************************************************************************
 * @file     frf.c 
 * @brief    Falcon RF Library Source File  
 * @version  1.0
 * @date     2020-06-07
 * @author   Adam Bujak
 ******************************************************************************/

#include "frf.h"

#include "nRF24L01.h"
#include "nRF24L01_hw.h"

#include <stdbool.h>
#include <intrins.h>

#define FRF_DEFAULT_SIZE_PACKET 32
#define FRF_NB_BITS_FOR_ERROR_RATE_CALC 100000
#define FRF_NB_BYTES_FOR_ERROR_RATE_CALC 12500
#define FRF_MAX_SIZE_PACKET 32
#define FRF_ADDR_WIDTH 5


#define FRF_ENABLE() (RFCKEN = 1);
#define FRF_IRQ_ENABLE() (RF = 1);
#define FRF_IRQ_DISABLE() (RF = 0);


void frf_test_init(frf_config_t config);

driver_init()
{
  CE_LOW();
  nRF24L01_set_output_power(pwr);
  nRF24L01_set_datarate(datarate);
  nRF24L01_set_address_width(nRF24L01_AW_5BYTES);
  nRF24L01_flush_rx();
  nRF24L01_set_operation_mode(nRF24L01_PRX); 
  nRF24L01_close_pipe(nRF24L01_ALL);                // first close all radio pipes...
  nRF24L01_open_pipe(nRF24L01_PIPE0, false);        // Turn off all auto acknowledge functionality
  nRF24L01_set_auto_retr(nRF24L01_PIPE0,0);
  nRF24L01_set_rf_channel(ch);
  nRF24L01_set_pll_mode(false);
  nRF24L01_set_crc_mode(nRF24L01_CRC_OFF);
  nRF24L01_set_address(nRF24L01_PIPE0, address);

  if (0xFF == rf_test_ctx.actual_packet_size)
  {
    //the size and expected packet has not been set
    rf_test_sensitivity_set_expected_data(DEFAULT_SIZE_PACKET, default_payload); //set the default values
  }
  else
  {
    rf_test_sensitivity_set_expected_data(0, NULL); //otherwise reset the previous values
  }
  CE_HIGH();
}


