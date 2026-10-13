/**
 * 
 */

 #include "Common.h"
 #include "Config.h"

 #include "stm32wbxx_hal.h"

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

uint32_t Boootloader_getMCUId(void)
{
    return HAL_GetDEVID();
}

uint32_t Bootloader_getSerialNumber(void)
{
#if USE_DEFAULT_SERIAL_NUMBER
    return DEFAULT_SERIAL_NUMBER;
#else
    // Read the unique device ID from the STM32WB55 memory
#endif
}
    