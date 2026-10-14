/**
 * BootloaderLib.h
 *
 * @brief Bootloader library header file.
 */

#ifndef BOOTLOADERLIB_H
#define BOOTLOADERLIB_H

#include <stdint.h>

typedef enum
{
  FLASH_OK = 0,
  FLASH_ERROR,
  FLASH_BUSY,
  FLASH_TIMEOUT,
} flash_status_t;

flash_status_t flash_wait_for_last_operation(void);
flash_status_t flash_erase(void);
void flash_write(uint32_t address, uint8_t *data, uint32_t size);
flash_status_t flash_lock(void);
flash_status_t flash_unlock(void);
flash_status_t bootloader_update_batch(uint8_t *data, uint8_t size);
void bootloader_jump_to_application(void);

#endif /* BOOTLOADERLIB_H */