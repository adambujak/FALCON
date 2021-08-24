#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

void flash_write_start(uint8_t sector);
void flash_write_end(uint8_t sector);
void flash_sector_erase(uint8_t sector);

#endif // FLASH_H
