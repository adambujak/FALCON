#include "flash.h"
#include "board.h"

static void wait_for_busy(void)
{
  // wait for busy bit to be cleared
  while (FLASH->SR & (1 << 16));
}

static void select_sector(uint8_t sector)
{
  // clear sector bits in CR register
  FLASH->CR &= ~(0x7 << 3);
  FLASH->CR |= (sector << 3);
}

static void erase_sector(uint8_t sector)
{
  select_sector(sector);
  // set sector erase bit
  FLASH->CR |= (1 << 1);
  // set strt bit
  FLASH->CR |= (1 << 16);
}

static void program_sector(uint8_t sector)
{
  select_sector(sector);
  // set program bit
  FLASH->CR |= (1 << 0);
  // set strt bit
  FLASH->CR |= (1 << 16);
}

void flash_erase_sector(uint8_t sector)
{
  erase_sector(sector);
  wait_for_busy();
}

void flash_start_write(uint8_t sector)
{
  erase_sector(sector);
  wait_for_busy();
  program_sector(sector);
}

void flash_end_write(uint8_t sector)
{
  (void) sector;
  wait_for_busy();
}

void flash_write_lock(void)
{
  FLASH->CR |= (1 << 31);
}

void flash_write_unlock(void)
{
  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;
}
