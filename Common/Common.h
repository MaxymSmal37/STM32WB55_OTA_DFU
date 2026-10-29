/**
* @file Common.h
*
* @brief Common header file for the bootloader application.
*/

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

uint8_t Bootloader_getFirmwareVersion(uint8_t *fw_version);
uint8_t Bootloader_getSerialNumber(uint8_t *serial_number);
uint8_t Bootloader_getHwRevision(uint8_t *hwRev);
uint32_t Boootloader_getMCUId(void);

#endif // COMMON_H