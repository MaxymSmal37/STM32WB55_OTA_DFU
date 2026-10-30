
#include "Common.h"
#include "Config.h"
#include "shci.h"

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

uint8_t Bootloader_getWirelessFwVersion(uint8_t *wirelessVer)
{
    WirelessFwInfo_t info;

    memset(&info, 0, sizeof(info));

    if (SHCI_GetWirelessFwInfo(&info) != SHCI_Success)
    {
        return 0U;
    }

    int n = snprintf((char *)wirelessVer, WIRELESS_FW_VERSION_MAX_LEN + 1U,
                     "%u.%u.%u",
                     (unsigned)info.VersionMajor,
                     (unsigned)info.VersionMinor,
                     (unsigned)info.VersionSub);

    return (n > 0) ? (uint8_t)n : 0U;
}