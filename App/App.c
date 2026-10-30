#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "main.h"
#include "App.h"
#include "BootloaderLib.h"
#include "BootloaderConfig.h"

typedef AppState (*AppHandler)(void);



App_s app;

static void App_EraseFlashInit(void);
static void App_WriteFlashAbort(void);
static EraseFlashStatus_s App_ProcessEraseFlash(void);
static WriteFlashStatus_s AppProcessWritingPages(void);

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
  return update_requested ? APP_START_UPDATE : APP_IDLE;
}

static AppState App_Updating(void)
{
  if (AppProcessWritingPages() == WRITE_STATUS_OK)
  {
    return APP_END_UPDATE;
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
  App_EraseFlashInit();

  return APP_ERASE_FLASH;
}

static AppState App_EraseFlash(void)
{
  if (App_ProcessEraseFlash() == ERASE_STATUS_OK)
  {
    return APP_UPDATING;
  }
  else
  {
    return APP_ERASE_FLASH;
  }
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

    case APP_LED_SLOW_BLINK:
      if (app.led_blink_ms == 0)
      {
        HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
        app.led_blink_ms = APP_LED_BLINK_SLOW_INTERVAL_MS;
      }
      break;

  case APP_LED_FAST_BLINK:
    if (app.led_blink_ms == 0)
    {
      HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
      app.led_blink_ms = APP_LED_BLINK_FAST_INTERVAL_MS;
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

static void App_EraseFlashInit(void)
{
  app.erase_flash.current_state = ERASE_START_PAGE_PROCESSING;
  app.erase_flash.page_number = FIRST_MEMORY_PAGE;
  app.erase_flash.status = ERASE_STATUS_INPROGESS;
}

static EraseFlashStatus_s App_ProcessEraseFlash(void)
{
  switch (app.erase_flash.current_state)
  {
  case ERASE_FLASH_IDLE:
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
      app.erase_flash.status = ERASE_STATUS_OK;
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

  return app.erase_flash.status;
}

uint8_t App_FlashWriteBantch(uint32_t address, uint8_t size, uint8_t *data)
{
  if (app.write_flash.current_state != WRITE_FLASH_IDLE)
  {
    return -1;
  }

  app.write_flash.address = address;
  app.write_flash.data = data;
  app.write_flash.size = size;
  app.write_flash.offset = 0;
  app.write_flash.status = WRITE_STATUS_BUSY;

  app.write_flash.state_after_waiting = WRITE_FLASH_PREPARE;
  app.write_flash.current_state = WRITE_FLASH_WAITING;

  return 0;
}

static void App_WriteFlashAbort(void)
{
  FLASH->CR &= ~FLASH_CR_PG;
  flash_lock();
  app.write_flash.status = WRITE_STATUS_ERROR;
  app.write_flash.current_state = WRITE_FLASH_IDLE;
  app.state = APP_ERROR;
}

static WriteFlashStatus_s AppProcessWritingPages(void)
{
  switch (app.write_flash.current_state)
  {
  case WRITE_FLASH_IDLE:
    break;

  case WRITE_FLASH_WAITING:
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
      AppProcessWritingPages();
    }
    else
    {
      if (FLASH->SR & FLASH_SR_EOP)
        FLASH->SR = FLASH_SR_EOP;
      app.write_flash.current_state = app.write_flash.state_after_waiting;
    }
    break;
  }

  case WRITE_FLASH_PREPARE:
    if (flash_unlock() != FLASH_OK)
    {
      AppProcessWritingPages();
      break;
    }
    FLASH->CR |= FLASH_CR_PG;
    app.write_flash.current_state = WRITE_FLASH_WRITE_CHUNK;
    break;

  case WRITE_FLASH_WRITE_CHUNK:
  {
    if (app.write_flash.offset >= app.write_flash.size)
    {
      app.write_flash.current_state = WRITE_FLASH_FINISH;
      break;
    }

    uint64_t chunk = 0xFFFFFFFFFFFFFFFFULL;
    uint32_t copy_size = app.write_flash.size - app.write_flash.offset;
    if (copy_size > FLASH_WRITE_CHUNK)
      copy_size = FLASH_WRITE_CHUNK;

    memcpy(&chunk, &app.write_flash.data[app.write_flash.offset], copy_size);

    *(volatile uint64_t *)(app.write_flash.address + app.write_flash.offset) = chunk;
    app.write_flash.offset += copy_size;

    app.write_flash.state_after_waiting = WRITE_FLASH_WRITE_CHUNK;
    app.write_flash.current_state = WRITE_FLASH_WAITING;
    break;
  }

  case WRITE_FLASH_FINISH:
    FLASH->CR &= ~FLASH_CR_PG;
    flash_lock();
    app.write_flash.status = WRITE_STATUS_OK;
    app.write_flash.current_state = WRITE_FLASH_IDLE;
    break;

  default:
    break;
  }

  return app.write_flash.status;
}

void App_SetUpdFwSize(uint32_t size)
{
  app.fwInfo.size = size;
}

void App_SetUpdFwHash(uint8_t part, uint8_t *hashData)
{
  if (part >= APP_HASH_PARTS_COUNT)
  {
    return;
  }
  memcpy(&app.fwInfo.hash[part * APP_HASH_PART_SIZE], hashData, APP_HASH_PART_SIZE);
}

void App_GetUpdStatus(uint8_t* data)
{
  data[0] = (uint8_t)app.updateStatus.generalStatus;
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

void App_SetState(AppState state)
{
  if (state == APP_START_UPDATE && app.state != APP_IDLE)
  {
    return;
  }

  app.state = state;
}

void App_Tick(void)
{
  if (app.led_blink_ms > 0)
  {
    app.led_blink_ms--;
  }
}