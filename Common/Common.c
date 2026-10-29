
#include "Common.h"
#include "Config.h"

#include <string.h>
#include <stdint.h>

uint8_t Bootloader_getFirmwareVersion(uint8_t *fw_version)
{
    static const uint8_t version[] = FW_VERSION;

    memcpy(fw_version, version, sizeof(version) - 1U);

    return (uint8_t)(sizeof(version) - 1U);
}

uint8_t Bootloader_getHwRevision(uint8_t *hwRev)
{
    static const uint8_t revision[] = HW_REVISION;

    memcpy(hwRev, revision, sizeof(revision) - 1U);

    return (uint8_t)(sizeof(revision) - 1U);
}

uint8_t Bootloader_getSerialNumber(uint8_t *serialNumber)
{
    static const uint8_t serial[] = DEVICE_SN;

    memcpy(serialNumber, serial, sizeof(serial) - 1U);

    return (uint8_t)(sizeof(serial) - 1U);
}
