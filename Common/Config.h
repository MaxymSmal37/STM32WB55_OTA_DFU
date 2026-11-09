
/**
 * Config.h
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "FwVersion.h"

#define USE_DEFAULT_SERIAL_NUMBER   1
#define USE_DEFAULT_HW_REVISION     1

#if USE_DEFAULT_SERIAL_NUMBER 
    #define DEVICE_SN "FA171026B1E7"
#endif

#if USE_DEFAULT_HW_REVISION
    #define HW_REVISION "A2314V05"
#endif

#define BOOT_REQUEST_TIMEOUT_MS 2000U

#endif /* CONFIG_H */
