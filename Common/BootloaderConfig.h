/**
 * @file    BootloaderConfig.h
 * 
 * @brief   Bootloader configuration header file.
 * @author  maxymsmal
 */

#ifndef BOOTLOADERCONFIG_H
#define BOOTLOADERCONFIG_H


#define BOOTLOADER_SIZE   0x10000U

#define APP_FLASH_START   (FLASH_BASE + BOOTLOADER_SIZE)   /* 0x08010000 */
#define APP_FLASH_SIZE   (480U * 1024U)
#define APP_FLASH_END    (APP_FLASH_START + APP_FLASH_SIZE)
 
#define FLASH_WRITE_CHUNK 8U


#endif /* BOOTLOADERCONFIG_H */