#include "fifo.h"

#include <stdbool.h>

#define IS_POWER_OF_TWO(num) (((num) & ((num) - 1)) == 0) ? true : false

int fifo_init(fifo_t *fifo, uint8_t *buffer, uint32_t size)
{
  if (!(IS_POWER_OF_TWO(size))) {
    return -1;
  }

  fifo->buffer = buffer;
  fifo->size = size;
  fifo->write_index = 0;
  fifo->read_index = 0;
  fifo->bytes_available = 0;
  return 0;
}

void fifo_push(fifo_t *fifo, uint8_t *buffer, uint32_t length)
{
  for (uint32_t i = 0; i < length; i++) {
    fifo->buffer[fifo->write_index] = buffer[i];
    fifo->write_index = (fifo->write_index + 1) & (fifo->size - 1);
  }

  fifo->bytes_available = (fifo->bytes_available + length) & (fifo->size - 1);
}

int fifo_pop(fifo_t *fifo, uint8_t *dest, uint32_t length)
{
  if (length > fifo->bytes_available) {
    return 0;
  }

  for (uint32_t i = 0; i < length; i++) {
    dest[i] = fifo->buffer[fifo->read_index];
    fifo->read_index = (fifo->read_index + 1) & (fifo->size - 1);
  }
  fifo->bytes_available -= length;
  return length;
}
