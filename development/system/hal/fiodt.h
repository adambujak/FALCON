/******************************************************************************
 * @file     fiodt.h
 * @brief    Falcon Input/Output Data
 * @version  1.0
 * @date     2020-04-02
 * @author   Adam Bujak
 ******************************************************************************/

#ifndef FIODT_H
#define FIODT_H

typedef enum
{
  FIO_RETVAL_SUCCESS = 0,
  FIO_RETVAL_ERROR,
  FIO_RETVAL_CNT
} fio_retval_e;

typedef struct fio_struct fio_t;
typedef uint32_t fio_module_t;

#define FIO_MODULE_CNT  4

#define FIO_MODULE_0    0
#define FIO_MODULE_1    1
#define FIO_MODULE_2    2
#define FIO_MODULE_3    3

/* Board IO Module values */
typedef uint32_t bio_module_t;
#define BIO_MODULE_0    EUSCI_B0_BASE
#define BIO_MODULE_1    EUSCI_B1_BASE
#define BIO_MODULE_2    EUSCI_B2_BASE
#define BIO_MODULE_3    EUSCI_B3_BASE

//typedef enum
//{
//  FIO_TYPE_I2C,
//  FIO_TYPE_CNT
//} fio_type_e;
//
//struct fio_struct
//{
//  fio_module_t module;
//  fio_
//};

#endif // FIODT_H
