/**
* @file App.h
*
* @brief Bootloader application header file.
*/

#ifndef APP_H
#define APP_H

#define APP_LED_BLINK_INTERVAL_MS 100

typedef enum {
    APP_INIT,
    APP_WAITING_FOR_CONNECTION,
    APP_IDLE,
    APP_START_UPDATE,
    APP_ERASE_FLASH,
    APP_UPDATING,
    APP_END_UPDATE,
    APP_ERROR,
    APP_STATE_COUNT
} AppState;

typedef enum {
    APP_LED_OFF = 0,
    APP_LED_ON,
    APP_LED_BLINK
} AppLedMode_s;

void App_Init(void);
void App_Run(void);     
AppState App_GetState(void);
void App_ProcessLed(AppLedMode_s led_mode);
void App_Tick(void);
void App_DeviceConnected(void);
void App_DeviceDisconnected(void);

#endif // APP_H