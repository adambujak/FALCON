/******************************************************************************
 * @file     i2c_master.h
 * @brief    Falcon I2C Hardware Abstraction Layer Header File
 * @version  1.0
 * @date     2020-04-01
 * @author   Adam Bujak
 ******************************************************************************/

#ifndef FI2C_MASTER_HAL_H
#define FI2C_MASTER_HAL_H

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "fln_bsp.h"
#include "fiodt.h"
#include <stdbool.h>
#include <stdlib.h>

typedef uint8_t  fi2c_addr_t;
typedef uint8_t  fi2c_reg_t;

typedef struct fi2c_struct fi2c_t;

 typedef enum
 {
   FI2C_TRANSFER_TYPE_WRITE = 0,
   FI2C_TRANSFER_TYPE_READ,
   FI2C_TRANSFER_CNT
 } fi2c_transfer_type_e;

 typedef struct
 {
   fi2c_addr_t   readReg;
   fi2c_reg_t  * rxArray;
 } fi2c_read_packet_t;

 typedef struct
 {
   fi2c_addr_t   writeReg;
   fi2c_reg_t  * txArray;
 } fi2c_write_packet_t;

 typedef union
 {
   fi2c_read_packet_t  readPacket;
   fi2c_write_packet_t writePacket;
 } fi2c_packet_u;

 typedef struct
 {
   fi2c_packet_u        packetUnion;
   fi2c_transfer_type_e type;
 } fi2c_packet_t;

 typedef struct
 {
   fi2c_addr_t     slaveAddr;
   fi2c_packet_t   packet;
   uint32_t        size;
   uint32_t        xferIndex;
 } fi2c_transfer_data_t;

struct fi2c_struct
{
  bio_module_t           module;
  bool                   isInitialized;
  bool                   isTransferring;
  fi2c_transfer_data_t   currentTransferData;
  fio_retval_e       ( * blockingWrite )    ( fi2c_t * instance, fi2c_addr_t slaveAddr, fi2c_reg_t writeReg, fi2c_reg_t * txArray, uint32_t size );
  fio_retval_e       ( * blockingRead )     ( fi2c_t * instance, fi2c_addr_t slaveAddr, fi2c_reg_t readReg,  fi2c_reg_t * rxArray, uint32_t size );
  fio_retval_e       ( * deinitialize )     ( fi2c_t * instance );
};


fi2c_t * FI2C_Initialize ( fio_module_t module, const eUSCI_I2C_MasterConfig * config );

#endif // FI2C_MASTER_HAL_H
