/**
* @file Common.h
*
* @brief Common header file for the bootloader application.
*/

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define WIRELESS_FW_VERSION_MAX_LEN 11U 

uint8_t Bootloader_getFirmwareVersion(uint8_t *fw_version);
uint8_t Bootloader_getSerialNumber(uint8_t *serial_number);
uint8_t Bootloader_getHwRevision(uint8_t *hwRev);
uint8_t Bootloader_getWirelessFwVersion(uint8_t *wirelessVer);

#endif // COMMON_H