#include "grinbuf.h"
#include "unity.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#define LOG_DEBUG(fmt, ...)              \
  do {                                   \
    printf((fmt), ## __VA_ARGS__); \
  } while (0)
#else
#define LOG_DEBUG(...) do {} while (0)
#endif

static void assertBufferIsVal(uint8_t *buffer, uint32_t bufferSize, uint8_t val)
{
  for (uint32_t i = 0; i < bufferSize; i++) {
    TEST_ASSERT_EQUAL(buffer[i], val);
  }
}

static void fillBufferRandom(uint8_t *buffer, uint32_t bufferSize)
{
  for (uint32_t i = 0; i < bufferSize; i++) {
    buffer[i] = rand() & 0xFF;
  }
}

static void printBuffer(uint8_t *buffer, uint32_t bufferSize)
{
  (void) buffer;
  LOG_DEBUG("\n{\n");
  for (uint32_t i = 0; i < bufferSize; i++) {
    LOG_DEBUG("%x, ", buffer[i]);
  }
  LOG_DEBUG("\n}\n\n");
}

static void printBufferDetailed(uint8_t *buffer, uint32_t bufferSize)
{
  (void) buffer;
  LOG_DEBUG("\n{\n");
  for (uint32_t i = 0; i < bufferSize; i++) {
    LOG_DEBUG("[%u]: %x, \n", i, buffer[i]);
  }
  LOG_DEBUG("\n}\n\n");
}

void setUp(void)
{
}

void tearDown(void)
{
}

void test_read(void)
{
  #define SIZE 1000
  uint8_t grinbufBuffer[SIZE] = { 0 };
  grinbuf_t buffer;
  grinbuf_init(&buffer, grinbufBuffer, SIZE);
  #undef SIZE

  #define READ_SIZE 100

  uint8_t writeData[READ_SIZE];
  fillBufferRandom(writeData, READ_SIZE);
  TEST_ASSERT_EQUAL(READ_SIZE, grinbuf_write(&buffer, writeData, READ_SIZE));

  uint8_t readData[READ_SIZE];
  TEST_ASSERT_EQUAL(READ_SIZE, grinbuf_read(&buffer, readData, READ_SIZE));

  TEST_ASSERT_EQUAL(0, memcmp(writeData, readData, READ_SIZE));
  TEST_ASSERT_EQUAL(0, grinbuf_read(&buffer, readData, 1));
  TEST_ASSERT_EQUAL(0, grinbuf_getBytesUsed(&buffer));
  #undef READ_SIZE
}

void test_write(void)
{
  #define SIZE 1000
  uint8_t grinbufBuffer[SIZE] = { 0 };
  grinbuf_t buffer;
  grinbuf_init(&buffer, grinbufBuffer, SIZE);

  #define WRITE_SIZE 100
  uint8_t writeData[WRITE_SIZE];

  for (uint32_t i = 0; i < (SIZE/WRITE_SIZE); i++) {
    fillBufferRandom(writeData, WRITE_SIZE);
    TEST_ASSERT_EQUAL(WRITE_SIZE, grinbuf_write(&buffer, writeData, WRITE_SIZE));

    TEST_ASSERT_EQUAL(0, memcmp(&grinbufBuffer[WRITE_SIZE * i], writeData, WRITE_SIZE));
    TEST_ASSERT_EQUAL(WRITE_SIZE*(i+1), grinbuf_getBytesUsed(&buffer));
  }

  #undef READ_SIZE
  #undef SIZE
}

void test_readEmptyBuffer(void)
{
  #define SIZE 190
  uint8_t data[SIZE] = { 0 };
  grinbuf_t buffer;
  grinbuf_init(&buffer, data, SIZE);
  #undef SIZE
  uint8_t readData[3] = { 0 };

  TEST_ASSERT_EQUAL(0, grinbuf_getBytesUsed(&buffer));
  TEST_ASSERT_EQUAL(0, grinbuf_read(&buffer, readData, 3));
}

void test_writeMoreThanSizeOfBuffer(void)
{
  #define SIZE 100
  uint8_t grinbufBuffer[SIZE];
  grinbuf_t buffer;
  grinbuf_init(&buffer, grinbufBuffer, SIZE);

  memset(grinbufBuffer, 0, SIZE);
  assertBufferIsVal(grinbufBuffer, SIZE, 0);

  uint8_t writeData[SIZE*2];
  memset(writeData, 1, SIZE*2);
  assertBufferIsVal(writeData, SIZE*2, 1);

  TEST_ASSERT_EQUAL(0, grinbuf_write(&buffer, writeData, SIZE*2));
  TEST_ASSERT_EQUAL(0, grinbuf_getBytesUsed(&buffer));

  assertBufferIsVal(grinbufBuffer, SIZE, 0);
  #undef SIZE
}

void test_writeWrapAround(void)
{
  #define SIZE 100
  uint8_t grinbufBuffer[SIZE] = { 0 };
  memset(grinbufBuffer, 0, SIZE);
  grinbuf_t buffer;
  grinbuf_init(&buffer, grinbufBuffer, SIZE);

  #define WRITE_SIZE_0 50
  #define WRITE_SIZE_1 82
  #define WRITE_SIZE_2 12

  uint8_t writeData0[WRITE_SIZE_0];
  uint8_t writeData1[WRITE_SIZE_1];
  uint8_t writeData2[WRITE_SIZE_2];

  uint8_t readData0[WRITE_SIZE_0];
  uint8_t readData1[WRITE_SIZE_1];
  uint8_t readData2[WRITE_SIZE_2];

  fillBufferRandom(writeData0, WRITE_SIZE_0);
  fillBufferRandom(writeData1, WRITE_SIZE_1);
  fillBufferRandom(writeData2, WRITE_SIZE_2);

  TEST_ASSERT_EQUAL(WRITE_SIZE_0, grinbuf_write(&buffer, writeData0, WRITE_SIZE_0));
  TEST_ASSERT_EQUAL(WRITE_SIZE_0, grinbuf_getBytesUsed(&buffer));
  TEST_ASSERT_EQUAL(WRITE_SIZE_0, grinbuf_read(&buffer, readData0, WRITE_SIZE_0));
  TEST_ASSERT_EQUAL(0, grinbuf_getBytesUsed(&buffer));
  TEST_ASSERT_EQUAL(0, memcmp(writeData0, readData0, WRITE_SIZE_0));

  TEST_ASSERT_EQUAL(WRITE_SIZE_1, grinbuf_write(&buffer, writeData1, WRITE_SIZE_1));
  TEST_ASSERT_EQUAL(WRITE_SIZE_1, grinbuf_getBytesUsed(&buffer));

  TEST_ASSERT_EQUAL(WRITE_SIZE_2, grinbuf_write(&buffer, writeData2, WRITE_SIZE_2));
  TEST_ASSERT_EQUAL(WRITE_SIZE_1 + WRITE_SIZE_2, grinbuf_getBytesUsed(&buffer));

  TEST_ASSERT_EQUAL(WRITE_SIZE_1, grinbuf_read(&buffer, readData1, WRITE_SIZE_1));
  TEST_ASSERT_EQUAL(WRITE_SIZE_2, grinbuf_getBytesUsed(&buffer));

  TEST_ASSERT_EQUAL(WRITE_SIZE_2, grinbuf_read(&buffer, readData2, WRITE_SIZE_2));
  TEST_ASSERT_EQUAL(0, grinbuf_getBytesUsed(&buffer));

  TEST_ASSERT_EQUAL(0, memcmp(writeData1, readData1, WRITE_SIZE_1));
  TEST_ASSERT_EQUAL(0, memcmp(writeData2, readData2, WRITE_SIZE_2));

  #undef WRITE_SIZE_1
  #undef WRITE_SIZE_2
  #undef SIZE
}

void test_peekAndPopWrapAround(void)
{
  #define SIZE 100
  uint8_t grinbufBuffer[SIZE] = { 0 };
  memset(grinbufBuffer, 0, SIZE);
  grinbuf_t buffer;
  grinbuf_init(&buffer, grinbufBuffer, SIZE);

  #define WRITE_SIZE_0 50
  #define WRITE_SIZE_1 82
  #define WRITE_SIZE_2 12

  uint8_t writeData0[WRITE_SIZE_0];
  uint8_t writeData1[WRITE_SIZE_1];

  fillBufferRandom(writeData0, WRITE_SIZE_0);
  fillBufferRandom(writeData1, WRITE_SIZE_1);

  // Offset write index to middle of buffer
  TEST_ASSERT_EQUAL(WRITE_SIZE_0, grinbuf_write(&buffer, writeData0, WRITE_SIZE_0));
  TEST_ASSERT_EQUAL(WRITE_SIZE_0, grinbuf_getBytesUsed(&buffer));
  TEST_ASSERT_EQUAL(WRITE_SIZE_0, grinbuf_pop(&buffer, WRITE_SIZE_0));
  TEST_ASSERT_EQUAL(0, grinbuf_getBytesUsed(&buffer));

  #define READ_SIZE WRITE_SIZE_1
  uint8_t readData[READ_SIZE];

  TEST_ASSERT_EQUAL(WRITE_SIZE_1, grinbuf_write(&buffer, writeData1, WRITE_SIZE_1));
  TEST_ASSERT_EQUAL(WRITE_SIZE_1, grinbuf_getBytesUsed(&buffer));

  TEST_ASSERT_EQUAL(WRITE_SIZE_1, grinbuf_peek(&buffer, readData, WRITE_SIZE_1));

  printBufferDetailed(grinbufBuffer, SIZE);
  printBuffer(writeData1, WRITE_SIZE_1);

  TEST_ASSERT_EQUAL(WRITE_SIZE_1, grinbuf_getBytesUsed(&buffer));
  TEST_ASSERT_EQUAL(0, memcmp(writeData1, readData, READ_SIZE));


  TEST_ASSERT_EQUAL(READ_SIZE, grinbuf_peek(&buffer, readData, READ_SIZE));
  TEST_ASSERT_EQUAL(WRITE_SIZE_1, grinbuf_getBytesUsed(&buffer));
  TEST_ASSERT_EQUAL(0, memcmp(writeData1, readData, READ_SIZE));

  TEST_ASSERT_EQUAL(10, grinbuf_pop(&buffer, 10));

  TEST_ASSERT_EQUAL(WRITE_SIZE_1 - 10, grinbuf_getBytesUsed(&buffer));
  TEST_ASSERT_EQUAL(WRITE_SIZE_1 - 10, grinbuf_read(&buffer, readData, WRITE_SIZE_1 - 10));
  TEST_ASSERT_EQUAL(0, memcmp(writeData1+10, readData, WRITE_SIZE_1 - 10));
  TEST_ASSERT_EQUAL(0, grinbuf_getBytesUsed(&buffer));

  #undef WRITE_SIZE_0
  #undef WRITE_SIZE_1
  #undef READ_SIZE
  #undef SIZE
}
