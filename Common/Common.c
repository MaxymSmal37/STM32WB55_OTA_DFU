/**
 * 
 */

 #include "Common.h"
 #include "Config.h"

Botloader_FwVersion Bootloader_getFirmwareVersion(void)
{
    Botloader_FwVersion version = 
    { 
        .prefix_major = FW_VERSION_PREFIX_MAJOR[0],
        .prefix_minor = FW_VERSION_PREFIX_MINOR[0],
        .major = FW_VERSION_MAJOR,
        .minor = FW_VERSION_MINOR,
        .patch = FW_VERSION_PATCH
    };

    return version;
}
