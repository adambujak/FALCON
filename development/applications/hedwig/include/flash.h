#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

void flash_write_lock(void);
void flash_write_unlock(void);
void flash_start_write(uint8_t sector);
void flash_end_write(uint8_t sector);
void flash_erase_sector(uint8_t sector);

#endif // FLASH_H
