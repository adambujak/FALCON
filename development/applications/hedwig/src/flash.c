#include "flash.h"
#include "board.h"

static void wait_for_busy(void)
{
  // wait for busy bit to be cleared
  while (FLASH->SR & (1 << 16));
}

static void sector_select(uint8_t sector)
{
  // clear sector bits in CR register
  FLASH->CR &= ~(0x7 << 3);
  // set sector to operate on in CR register
  FLASH->CR |= (sector << 3);
}

static void sector_erase(uint8_t sector)
{
  sector_select(sector);
  // set sector erase bit
  FLASH->CR |= (1 << 1);
  // set strt bit
  FLASH->CR |= (1 << 16);
}

static void sector_program(uint8_t sector)
{
  sector_select(sector);
  // set program bit
  FLASH->CR |= (1 << 0);
  // set strt bit
  FLASH->CR |= (1 << 16);
}

static void write_lock(void)
{
  FLASH->CR |= (1 << 31);
}

static void write_unlock(void)
{
  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;
}

void flash_sector_erase(uint8_t sector)
{
  write_unlock();
  sector_erase(sector);
  wait_for_busy();
  write_lock();
}

void flash_write_start(uint8_t sector)
{
  write_unlock();
  sector_erase(sector);
  wait_for_busy();
  sector_program(sector);
}

void flash_write_end(uint8_t sector)
{
  (void)sector;
  wait_for_busy();
  write_lock();
}
