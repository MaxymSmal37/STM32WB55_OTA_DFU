/**
 * @file    BootloaderConfig.h
 * 
 * @brief   Bootloader configuration header file.
 * @author  maxymsmal
 */

#ifndef BOOTLOADERCONFIG_H
#define BOOTLOADERCONFIG_H

#define APP_FLASH_START  0x08008000UL
#define APP_FLASH_SIZE   (480U * 1024U)
#define APP_FLASH_END    (APP_FLASH_START + APP_FLASH_SIZE)
 
#define FLASH_WRITE_CHUNK 8U


#endif /* BOOTLOADERCONFIG_H */