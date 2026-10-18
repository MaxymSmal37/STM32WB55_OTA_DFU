#include "App.h"
#include <stdbool.h>
#include <stdint.h>
#include "main.h"
#include "BootloaderLib.h"
#include "BootloaderConfig.h"

typedef AppState (*AppHandler)(void);

typedef struct
{
  erase_flashing_s erase_flash;
  AppState state;
  uint16_t led_blink_ms;
} App_s;

App_s app;

void App_Init(void)
{
  app.state = APP_INIT;
  app.led_blink_ms = 0;
}

static AppState App_Initialize(void)
{
  App_Init();
  return APP_WAITING_FOR_CONNECTION;
}

static AppState App_WaitingForConnection(void)
{
  App_ProcessLed(APP_LED_FAST_BLINK);
  return APP_WAITING_FOR_CONNECTION;
}

static AppState App_Idle(void)
{
  bool update_requested = false;
  return update_requested ? APP_UPDATING : APP_IDLE;
}

static AppState App_Updating(void)
{
  bool done = true;

  if (done)
  {
    return APP_IDLE;
  }

  return APP_UPDATING;
}

static AppState App_Error(void)
{
  bool recovered = false;
  return recovered ? APP_INIT : APP_ERROR;
}

static AppState App_StartUpdate(void)
{
  return APP_ERASE_FLASH;
}

static AppState App_EraseFlash(void)
{
  bool erase_done = true;
  return erase_done ? APP_UPDATING : APP_ERASE_FLASH;
}

static AppState App_EndUpdate(void)
{
  return APP_IDLE;
}

void App_ProcessLed(AppLedMode_s led_mode)
{
  switch (led_mode)
  {
  case APP_LED_OFF:
    HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, GPIO_PIN_RESET);
    break;

  case APP_LED_ON:
    HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, GPIO_PIN_SET);
    break;

  case APP_LED_FAST_BLINK:
    if (app.led_blink_ms == 0)
    {
      HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
      app.led_blink_ms = APP_LED_BLINK_INTERVAL_MS;
    }
    break;

  default:
    break;
  }
}

void App_DeviceConnected(void)
{
  app.state = APP_IDLE;
  App_ProcessLed(APP_LED_ON);
}

void App_DeviceDisconnected(void)
{
  app.state = APP_WAITING_FOR_CONNECTION;
  App_ProcessLed(APP_LED_FAST_BLINK);
}

void App_ProcessEraseFlash(void)
{
  switch (app.erase_flash.current_state)
  {
  case ERASE_FLASH_IDLE:
    /* code */
    break;

  case ERASE_WAITING:
  {
    if (FLASH->SR & FLASH_SR_BSY)
    {
      break;
    }

    uint32_t errors = FLASH->SR & (FLASH_SR_PROGERR | FLASH_SR_WRPERR |
                                   FLASH_SR_PGAERR | FLASH_SR_SIZERR |
                                   FLASH_SR_PGSERR);
    if (errors)
    {
      FLASH->SR = errors;
      FLASH->CR &= ~FLASH_CR_PER;
      flash_lock();
      app.state = APP_ERROR;
      app.erase_flash.current_state = ERASE_FLASH_IDLE;
    }
    else
    {
      app.erase_flash.current_state = app.erase_flash.state_after_waiting;
    }
    break;
  }

  case ERASE_START_PAGE_PROCESSING:
  {
    if (app.erase_flash.page_number * FLASH_PAGE_SIZE >= APP_FLASH_SIZE)
    {
      app.erase_flash.current_state = ERASE_FLASH_IDLE;
      /// @todo Add exit from erase state.
    }
    else
    {
      app.erase_flash.current_address =
          APP_FLASH_START + (app.erase_flash.page_number * FLASH_PAGE_SIZE);
      app.erase_flash.state_after_waiting = ERASE_PAGE_PROCESSING;
      app.erase_flash.current_state = ERASE_WAITING;
    }
    break;
  }

  case ERASE_PAGE_PROCESSING:
  {
    if (flash_unlock() != FLASH_OK)
    {
      app.state = APP_ERROR;
      app.erase_flash.current_state = ERASE_FLASH_IDLE;
      break;
    }

    uint32_t page_number =
        (app.erase_flash.current_address - FLASH_BASE) / FLASH_PAGE_SIZE;

    FLASH->CR &= ~FLASH_CR_PNB;
    FLASH->CR |= (page_number << FLASH_CR_PNB_Pos) & FLASH_CR_PNB;
    FLASH->CR |= FLASH_CR_PER;
    FLASH->CR |= FLASH_CR_STRT;

    app.erase_flash.state_after_waiting = ERASE_FINISH_PAGE_PROCESSING;
    app.erase_flash.current_state = ERASE_WAITING;
    break;
  }

  case ERASE_FINISH_PAGE_PROCESSING:
    FLASH->CR &= ~FLASH_CR_PER;
    flash_lock();
    app.erase_flash.page_number++; 
    app.erase_flash.current_state = ERASE_START_PAGE_PROCESSING;
    break;

  default:
    break;
  }
}

//-----------------------------------
//--------Dispatcher-----------------
//-----------------------------------

static const AppHandler handlers[APP_STATE_COUNT] = {
    [APP_INIT] = App_Initialize,
    [APP_WAITING_FOR_CONNECTION] = App_WaitingForConnection,
    [APP_IDLE] = App_Idle,
    [APP_START_UPDATE] = App_StartUpdate,
    [APP_ERASE_FLASH] = App_EraseFlash,
    [APP_UPDATING] = App_Updating,
    [APP_END_UPDATE] = App_EndUpdate,
    [APP_ERROR] = App_Error,
};

void App_Run(void)
{
  app.state = handlers[app.state]();
}

AppState App_GetState(void)
{
  return app.state;
}

void App_Tick(void)
{
  if (app.led_blink_ms > 0)
  {
    app.led_blink_ms--;
  }
}