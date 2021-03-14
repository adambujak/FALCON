#ifndef DEVICE_COM_H
#define DEVICE_COM_H

#include "falcon_common.h"

void device_com_send_frame(uint8_t *data);
void device_com_task(void *pvParameters);
void device_com_setup (void);
void device_com_rx (uint32_t source, uint8_t *data, uint16_t length);
uint32_t device_com_register_channel (void);


#endif // DEVICE_COM_H
