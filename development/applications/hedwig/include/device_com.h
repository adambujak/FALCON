#ifndef DEVICE_COM_H
#define DEVICE_COM_H

#include "falcon_common.h"

void device_com_setup(void);
void device_com_start(void);
void device_com_send_packet(uint8_t *data, uint8_t length);

#endif // DEVICE_COM_H
