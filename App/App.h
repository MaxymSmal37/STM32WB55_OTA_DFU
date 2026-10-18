/**
 * @file App.h
 *
 * @brief Bootloader application header file.
 */

#ifndef APP_H
#define APP_H

#include <stdint.h> 

#define APP_LED_BLINK_INTERVAL_MS 100

typedef enum
{
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

typedef enum
{
  APP_LED_OFF = 0,
  APP_LED_ON,
  APP_LED_FAST_BLINK
} AppLedMode_s;

typedef enum
{
  ERASE_FLASH_IDLE,
  ERASE_WAITING,
  ERASE_START_PAGE_PROCESSING,
  ERASE_PAGE_PROCESSING,
  ERASE_FINISH_PAGE_PROCESSING,
} EraseFlashState_s;

typedef struct
{
  uint32_t current_address;
  EraseFlashState_s current_state;
  EraseFlashState_s state_after_waiting;
  uint32_t page_number;
} erase_flashing_s;

void App_Init(void);
void App_Run(void);
AppState App_GetState(void);
void App_ProcessLed(AppLedMode_s led_mode);
void App_Tick(void);
void App_DeviceConnected(void);
void App_DeviceDisconnected(void);

#endif // APP_H