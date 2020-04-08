/******************************************************************************
 * @file     fiom.h 
 * @brief    Falcon Input/Output Module 
 * @version  1.0
 * @date     2020-04-05
 * @author   Adam Bujak
 ******************************************************************************/

#ifndef FIOM_H
#define FIOM_H

#include "fio_hal.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct fiom_struct fiom_t;
typedef fio_error_t  ( * fiom_tf_t )   ( fiom_t * instance, fio_transfer_t * transferData );   /* Transfer function pointer type */

struct fiom_struct
{
  /* @public */
  fiom_tf_t           blockingTransfer;     /* Blocking transfer function pointer */
  fiom_tf_t           nonBlockingTransfer;  /* Non blocking transfer function pointer */

  /* @private */
  fio_hal_t         * halInstance;          /* FIO hal instance */
  // add transaction queue here
};

fio_error_t FIOM_Initialize   ( fiom_t * instance, fio_module_t module, const fio_config_t * config );
void        FIOM_Deinitialize ( fiom_t * instance );

#endif // FIOM_H
