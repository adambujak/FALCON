/******************************************************************************
 * @file     i2c_master.h
 * @brief    Falcon I2C Hardware Abstraction Layer Header File
 * @version  1.0
 * @date     2020-04-01
 * @author   Adam Bujak
 ******************************************************************************/

#ifndef FI2C_MASTER_HAL
#define FI2C_MASTER_HAL

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#define FI2C_DEFAULT_CONFIG                  \
  {                                          \
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,       \
        3000000,                             \
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,   \
        0,                                   \
        EUSCI_B_I2C_NO_AUTO_STOP             \
  }

#define FI2C_MODULE_0 EUSCI_B0_BASE
#define FI2C_MODULE_1 EUSCI_B1_BASE
#define FI2C_MODULE_2 EUSCI_B2_BASE
#define FI2C_MODULE_3 EUSCI_B3_BASE

typedef uint32_t fi2c_module_t;
typedef uint8_t  fi2c_addr_t;
typedef uint8_t  fi2c_reg_t;

typedef struct fi2c_struct fi2c_t;

typedef enum
{
  FI2C_RETVAL_SUCCESS = 0,
  FI2C_RETVAL_ERROR,
  FI2C_RETVAL_CNT
} fi2c_retval_e;

// typedef enum
// {
//   FI2C_TRANSFER_TYPE_WRITE = 0,
//   FI2C_TRANSFER_TYPE_READ,
//   FI2C_TRANSFER_CNT
// } fi2c_transfer_type_e;

// typedef struct  
// {
//   fi2c_addr_t   readAddr;
//   fi2c_reg_t  * rxArray;
//   uint32_t      transferCnt;
// } fi2c_read_packet_t;

// typedef struct  
// {
//   fi2c_addr_t   writeAddr;
//   fi2c_reg_t  * txArray;
//   uint32_t      transferCnt;
// } fi2c_write_packet_t;

// typedef union 
// {
//   fi2c_read_packet_t  readPacket;
//   fi2c_write_packet_t writePacket;
// } fi2c_transfer_u;

// typedef struct 
// {
//   fi2c_transfer_u      transferVals;
//   fi2c_transfer_type_e type;
// } fi2c_transfer_t;

// typedef struct 
// {
//   fi2c_addr_t     slaveAddr;
//   fi2c_transfer_t transfer;
// } fi2c_packet_t;

struct fi2c_struct
{
  fi2c_module_t      module;
  fi2c_retval_e  ( * blockingWrite )    ( fi2c_t * instance, fi2c_addr_t slaveAddr, fi2c_reg_t * txArray, uint32_t size );
  fi2c_retval_e  ( * blockingRead )     ( fi2c_t * instance, fi2c_addr_t slaveAddr, fi2c_reg_t * rxArray, uint32_t size );
  fi2c_retval_e  ( * deinitialize )     ( fi2c_t * instance );
};


fi2c_retval_e  FI2C_Initialize ( fi2c_t * instance, fi2c_module_t module, const eUSCI_I2C_MasterConfig * config );

#endif
