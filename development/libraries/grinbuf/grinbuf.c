#include "grinbuf.h"
#include <stdlib.h>
#include <string.h>

#define MAX(a, b)    (((a) > (b))?(a):(b))
#define MIN(a, b)    (((a) < (b))?(a):(b))

inline static void increment_index(grinbuf_t *instance, uint32_t *index, uint32_t value)
{
  *index += value;
  *index %= instance->size;
}

void grinbuf_init(grinbuf_t *instance, uint8_t *buffer, uint32_t size)
{
  instance->buffer     = buffer;
  instance->size       = size;
  instance->readIndex  = 0;
  instance->writeIndex = 0;
  instance->bytesUsed  = 0;
}

uint32_t grinbuf_write(grinbuf_t *instance, uint8_t *data, uint32_t length)
{
  if (length > instance->size) {
    return 0;
  }

  uint32_t bytesToWrite = MIN(length, instance->size - instance->writeIndex);
  uint32_t bytesWritten = 0;

  memcpy(instance->buffer + instance->writeIndex, data, bytesToWrite);
  increment_index(instance, &instance->writeIndex, bytesToWrite);
  bytesWritten += bytesToWrite;

  bytesToWrite = length - bytesWritten;
  memcpy(instance->buffer + instance->writeIndex, data + bytesWritten, bytesToWrite);
  increment_index(instance, &instance->writeIndex, bytesToWrite);
  instance->bytesUsed = MIN((instance->bytesUsed + length), instance->size);
  bytesWritten       += bytesToWrite;

  return bytesWritten;
}

uint32_t grinbuf_read(grinbuf_t *instance, uint8_t *dest, uint32_t length)
{
  if (length > instance->bytesUsed) {
    return 0;
  }

  uint32_t bytesToRead = MIN(length, instance->size - instance->readIndex);
  uint32_t bytesRead   = 0;

  memcpy(dest, instance->buffer + instance->readIndex, bytesToRead);
  increment_index(instance, &instance->readIndex, bytesToRead);
  bytesRead += bytesToRead;

  bytesToRead = length - bytesRead;

  memcpy(dest + bytesRead, instance->buffer + instance->readIndex, bytesToRead);
  increment_index(instance, &instance->readIndex, bytesToRead);
  bytesRead += bytesToRead;

  instance->bytesUsed -= bytesRead;
  return bytesRead;
}

uint32_t grinbuf_peek(grinbuf_t *instance, uint8_t *dest, uint32_t length)
{
  if (length > instance->bytesUsed) {
    return 0;
  }

  uint32_t tempReadIndex = instance->readIndex;
  uint32_t bytesToRead   = MIN(length, instance->size - instance->readIndex);
  uint32_t bytesRead     = 0;

  memcpy(dest, instance->buffer + tempReadIndex, bytesToRead);
  bytesRead += bytesToRead;
  increment_index(instance, &tempReadIndex, bytesRead);

  bytesToRead = length - bytesRead;
  memcpy(dest + bytesRead, instance->buffer + tempReadIndex, bytesToRead);
  bytesRead += bytesToRead;

  return bytesRead;
}

uint32_t grinbuf_pop(grinbuf_t *instance, uint32_t length)
{
  if (length > instance->bytesUsed) {
    return 0;
  }
  increment_index(instance, &instance->readIndex, length);
  instance->bytesUsed -= length;
  return length;
}

void grinbuf_reset(grinbuf_t *instance)
{
  instance->readIndex  = 0;
  instance->writeIndex = 0;
  instance->bytesUsed  = 0;
}

uint32_t grinbuf_getBytesUsed(grinbuf_t *instance)
{
  return instance->bytesUsed;
}
