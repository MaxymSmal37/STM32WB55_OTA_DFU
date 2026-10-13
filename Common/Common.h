/**
* @file Common.h
*
* @brief Common header file for the bootloader application.
*/

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef struct {
    uint8_t prefix_major;
    uint8_t prefix_minor;
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} Botloader_FwVersion;

Botloader_FwVersion Bootloader_getFirmwareVersion(void);

#endif // COMMON_H