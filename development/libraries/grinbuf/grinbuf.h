/******************************************************************************
* @file     grinbuf.h
* @brief    grinbuf - modified nondestructive, static ringbuf library
* @version  1.0
* @date     2021-03-10
* @author   Adam Bujak
******************************************************************************/
#ifndef GRINBUF_H
#define GRINBUF_H

#include <stdint.h>

typedef struct
{
  uint8_t *buffer;
  uint32_t size;
  uint32_t bytesUsed;
  uint32_t writeIndex;
  uint32_t readIndex;
} grinbuf_t;

void grinbuf_init(grinbuf_t *instance, uint8_t *buffer, uint32_t size);
void grinbuf_reset(grinbuf_t *instance);
uint32_t grinbuf_write(grinbuf_t *instance, uint8_t *data, uint32_t length);
uint32_t grinbuf_read(grinbuf_t *instance, uint8_t *dest, uint32_t length);
uint32_t grinbuf_peek(grinbuf_t *instance, uint8_t *dest, uint32_t length);
uint32_t grinbuf_pop(grinbuf_t *instance, uint32_t length);
uint32_t grinbuf_getBytesUsed(grinbuf_t *instance);


#endif // GRINBUF_H
