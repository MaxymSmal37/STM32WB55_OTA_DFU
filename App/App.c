#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "main.h"
#include "App.h"
#include "AppComm.h"
#include "BootloaderLib.h"
#include "BootloaderConfig.h"
#include "Logger.h"
#include "shci.h"

typedef AppState (*AppHandler)(void);

App_s app;

static void App_EraseFlashInit(void);
static void App_WriteFlashAbort(void);
static void App_EraseFinish(EraseFlashStatus_s status);
static EraseFlashStatus_s App_ProcessEraseFlash(void);
static WriteFlashStatus_s AppProcessWritingPages(void);
static uint8_t App_ImageCheck(uint32_t *sp_out, uint32_t *rv_out);

void App_Init(void)
{
  app.state = APP_INIT;
  app.led_blink_ms = 0;
  app.flash_phase = FLASH_PHASE_PRESTINE;
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
  /// Some idle processing.
  return APP_IDLE;
}

static AppState App_Updating(void)
{
  App_ProcessLed(APP_LED_MODE_MIXED_BLINK);

  (void)AppProcessWritingPages();

  if (app.write_flash.status_pending)
  {
    app.write_flash.status_pending = false;
    app.write_flash.percentage = (uint8_t)((app.write_flash.total_written * 100U) / app.fwInfo.size);

    AppComm_SendUpdateStatus();
  }

  if (app.write_flash.status == WRITE_STATUS_ERROR)
  {
    return APP_ERROR;
  }

  return APP_UPDATING;
}

static AppState App_Error(void)
{
  /// Some Error Processing.
  return APP_ERROR;
}

static AppState App_StartUpdate(void)
{
  app.write_flash.total_written = 0U;
  app.write_flash.percentage = 0U;
  app.write_flash.status = WRITE_STATUS_IDLE;
  app.write_flash.generalStatus = UG_STATUS_OK;
  app.write_flash.current_state = WRITE_FLASH_IDLE;
  app.write_flash.status_pending = false;

  return APP_UPDATING;
}

static AppState App_EraseFlash(void)
{
  App_ProcessLed(APP_LED_SLOW_BLINK);
  switch (App_ProcessEraseFlash())
  {
  case ERASE_STATUS_OK:
    return APP_IDLE;

  case ERASE_STATUS_FAULT:
    return APP_ERROR;

  default:
    return APP_ERASE_FLASH;
  }
}

static AppState App_EndUpdate(void)
{
  uint32_t sp, rv;
  uint8_t chk = App_ImageCheck(&sp, &rv);

  if (chk == 0U)
  {
    app.flash_phase = FLASH_PHASE_UPDATED;
    App_JumpToApplication();
  }
  else
  {
    app.flash_phase = FLASH_PHASE_ERASED;
  }

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

  case APP_LED_MODE_MIXED_BLINK:
    if (app.led_blink_ms == 0)
    {
      HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
      app.led_blink_ms = (app.led_blink_mode_toggler & 1U) ? APP_LED_BLINK_FAST_INTERVAL_MS : APP_LED_BLINK_SLOW_INTERVAL_MS;

      app.led_blink_mode_toggler++;

      if (app.led_blink_mode_toggler >= 6U)
      {
        app.led_blink_mode_toggler = 0U;
      }
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
  app.erase_flash.timeout_ms = APP_ERASE_FLASH_TIMEOUT_MS; 
  SHCI_C2_FLASH_EraseActivity(ERASE_ACTIVITY_ON);    
}

static void App_EraseFinish(EraseFlashStatus_s status)
{
  FLASH->CR &= ~FLASH_CR_PER;
  flash_lock();

  SHCI_C2_FLASH_EraseActivity(ERASE_ACTIVITY_OFF);
  app.erase_flash.status = status;
  app.erase_flash.current_state = ERASE_FLASH_IDLE;

  app.state = APP_IDLE;

  if (status != ERASE_STATUS_OK)
  {
    app.state = APP_ERROR;
  }
}

static EraseFlashStatus_s App_ProcessEraseFlash(void)
{
  if (app.erase_flash.current_state == ERASE_FLASH_IDLE)
  {
    return app.erase_flash.status;
  }

  if (app.app_timer_ms == 0U)
  {
    app.app_timer_ms = ERASE_FLASH_PROCESSING_TIMEOUT_MS;

    switch (app.erase_flash.current_state)
    {
    case ERASE_START_PAGE_PROCESSING:
      if (app.erase_flash.page_number >= APP_FLASH_PAGES)
      {
        app.erase_flash.page_number = APP_FLASH_PAGES;
        App_EraseFinish(ERASE_STATUS_OK);
        break;
      }

      app.erase_flash.timeout_ms = APP_ERASE_FLASH_TIMEOUT_MS;
      app.erase_flash.current_address = APP_FLASH_START + app.erase_flash.page_number * FLASH_PAGE_SIZE;
      app.erase_flash.state_after_waiting = ERASE_PAGE_PROCESSING;
      app.erase_flash.current_state = ERASE_WAITING;
      break;

    case ERASE_WAITING:
    {
      if (FLASH->SR & (FLASH_BUSY_MASK | FLASH_SR_PESD))
      {
        if (app.erase_flash.timeout_ms == 0U)
        {
          app.erase_flash.fault_code = 0xF1U;
          app.erase_flash.fault_sr = FLASH->SR;
          app.erase_flash.intermidiate_erase_status = UG_STATUS_ERROR;
          App_EraseFinish(ERASE_STATUS_FAULT);
        }
        else
        {
          app.erase_flash.timeout_ms--;
        }
        break;
      }

      if (FLASH->SR & FLASH_ERR_MASK)
      {
        app.erase_flash.fault_code = 0xF2U;
        app.erase_flash.fault_sr = FLASH->SR;
        FLASH->SR = FLASH->SR & FLASH_ERR_MASK;
        app.erase_flash.intermidiate_erase_status = UG_STATUS_ERROR;
        App_EraseFinish(ERASE_STATUS_FAULT);
        break;
      }

      if (FLASH->SR & FLASH_SR_EOP)
      {
        FLASH->SR = FLASH_SR_EOP;
      }
      app.erase_flash.current_state = app.erase_flash.state_after_waiting;
      break;
    }

    case ERASE_PAGE_PROCESSING:
    {
      if (flash_unlock() != FLASH_OK)
      {
        app.erase_flash.fault_code = 0xF3U;
        app.erase_flash.fault_sr = FLASH->SR;
        app.erase_flash.intermidiate_erase_status = UG_STATUS_ERROR;
        App_EraseFinish(ERASE_STATUS_FAULT);
        break;
      }

      uint32_t pnb = (app.erase_flash.current_address - FLASH_BASE) / FLASH_PAGE_SIZE;

      FLASH->CR = (FLASH->CR & ~FLASH_CR_PNB) | (pnb << FLASH_CR_PNB_Pos) | FLASH_CR_PER;
      FLASH->CR |= FLASH_CR_STRT;

      app.erase_flash.state_after_waiting = ERASE_FINISH_PAGE_PROCESSING;
      app.erase_flash.current_state = ERASE_WAITING;
      break;
    }

    case ERASE_FINISH_PAGE_PROCESSING:
      FLASH->CR &= ~FLASH_CR_PER;

      flash_lock();
  
      app.erase_flash.page_number++;
      app.erase_flash.intermidiate_erase_status = UG_STATUS_OK;
      app.erase_flash.current_state = ERASE_START_PAGE_PROCESSING;
      app.flash_phase = FLASH_PHASE_PRESTINE;
      break;

    default:
      break;
    }

    app.erase_flash.percentage = (uint8_t)((app.erase_flash.page_number * 100U) / APP_FLASH_PAGES);

    if (app.erase_flash.percentage != app.erase_flash.last_sent_pct ||
        app.erase_flash.status != ERASE_STATUS_INPROGESS)
    {
      app.erase_flash.last_sent_pct = app.erase_flash.percentage;
      AppComm_SendEraseStatus();
    }
  }
  return app.erase_flash.status;
}
int8_t App_FlashWriteBantch(uint32_t offset, uint8_t size, const uint8_t *data)
{
  if (app.write_flash.current_state != WRITE_FLASH_IDLE)
  {
    return -1;
  }

  memcpy(app.write_flash.data, data, size);
  app.write_flash.address = APP_FLASH_START + offset;
  app.write_flash.size = size;
  app.write_flash.chunk_offset = 0U;
  app.write_flash.status = WRITE_STATUS_BUSY;
  app.app_timer_ms = APP_WRITE_FLASH_TIMEOUT_MS;
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

static uint8_t App_ImageCheck(uint32_t *sp_out, uint32_t *rv_out)
{
  uint32_t sp = *(volatile uint32_t *)(APP_FLASH_START + 0U);
  uint32_t rv = *(volatile uint32_t *)(APP_FLASH_START + 4U);

  if (sp_out)
  {
    *sp_out = sp;
  }
    
  if (rv_out)
  {
    *rv_out = rv;
  }

  if (sp == 0xFFFFFFFFU && rv == 0xFFFFFFFFU)
  {
    return 1;
  }
    
  if (sp < 0x20000000U || sp > 0x20040000U)
  {
    return 2;
  }
    
  if ((sp & 3U) != 0U)
  {
    return 3;
  }

  if ((rv & 1U) == 0U)
  {
    return 4;
  }
   
  if (rv < APP_FLASH_START || rv >= APP_FLASH_START + APP_FLASH_SIZE)
  {
    return 5;
  }
  
  return 0;
}

static void system_deinit(void)
{
  RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;

  GPIOE->ODR &= ~(1U << 4U);

  TIM2->CR1 = 0U;
  TIM2->DIER = 0U;
  RCC->APB1ENR1 &= ~RCC_APB1ENR1_TIM2EN;

  GPIOA->MODER |= (3U << 18) | (3U << 20);
  GPIOA->AFR[1] &= ~((0xFU << 4) | (0xFU << 8));
  GPIOA->PUPDR &= ~(3U << 20);

  RCC->CR |= RCC_CR_HSION;
  while (!(RCC->CR & RCC_CR_HSIRDY)) { }

  RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | (0U << RCC_CFGR_SW_Pos);
  while ((RCC->CFGR & RCC_CFGR_SWS) != (0U << RCC_CFGR_SWS_Pos)) { }

  RCC->CR &= ~RCC_CR_PLLON;
  while (RCC->CR & RCC_CR_PLLRDY) { }
}

void App_JumpToApplication(void)
{
  uint32_t app_stack_ptr = *(volatile uint32_t *)(APP_FLASH_START + 0U);
  uint32_t app_reset_addr = *(volatile uint32_t *)(APP_FLASH_START + 4U);

  __disable_irq();

  system_deinit();

  for (uint32_t i = 0U; i < 8U; i++)
  {
    NVIC->ICER[i] = 0xFFFFFFFFU;
    NVIC->ICPR[i] = 0xFFFFFFFFU;
  }

  SysTick->CTRL = 0U;
  SysTick->LOAD = 0U;
  SysTick->VAL = 0U;
  SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk | SCB_ICSR_PENDSVCLR_Msk;

  FLASH->ACR &= ~(FLASH_ACR_ICEN | FLASH_ACR_DCEN);
  FLASH->ACR |= (FLASH_ACR_ICRST | FLASH_ACR_DCRST);
  FLASH->ACR &= ~(FLASH_ACR_ICRST | FLASH_ACR_DCRST);
  FLASH->ACR |= (FLASH_ACR_ICEN | FLASH_ACR_DCEN);

  SCB->VTOR = APP_FLASH_START;

  __DSB();
  __ISB();
  __enable_irq();

  __asm volatile(
      "msr msp, %0 \n"
      "bx  %1      \n" ::"r"(app_stack_ptr),
      "r"(app_reset_addr) :);

  while (1)
  {
  }
}

static WriteFlashStatus_s AppProcessWritingPages(void)
{
  switch (app.write_flash.current_state)
  {
  case WRITE_FLASH_IDLE:
    break;

  case WRITE_FLASH_WAITING:
  {
    uint32_t sr = FLASH->SR;

    if (sr & (FLASH_BUSY_MASK | FLASH_SR_PESD))
    {
      if (app.app_timer_ms == 0U)
      {
        App_WriteFlashAbort();
      }
      break;
    }

    if (sr & FLASH_ERR_MASK)
    {
      FLASH->SR = sr & FLASH_ERR_MASK;
      App_WriteFlashAbort();
      break;
    }

    if (sr & FLASH_SR_EOP)
    {
      FLASH->SR = FLASH_SR_EOP;
    }
    app.write_flash.current_state = app.write_flash.state_after_waiting;
    break;
  }

  case WRITE_FLASH_PREPARE:
    if (flash_unlock() != FLASH_OK)
    {
      break;
    }

    FLASH->CR |= FLASH_CR_PG;
    app.write_flash.current_state = WRITE_FLASH_WRITE_CHUNK;
    break;

  case WRITE_FLASH_WRITE_CHUNK:
  {
    uint32_t words[2] = {0xFFFFFFFFU, 0xFFFFFFFFU};
    memcpy(words, &app.write_flash.data[app.write_flash.chunk_offset], FLASH_WRITE_CHUNK);

    volatile uint32_t *dst =
        (volatile uint32_t *)(app.write_flash.address + app.write_flash.chunk_offset);
    dst[0] = words[0];
    __ISB();
    dst[1] = words[1];

    app.write_flash.chunk_offset += FLASH_WRITE_CHUNK;
    app.write_flash.total_written += FLASH_WRITE_CHUNK;

    app.write_flash.state_after_waiting = (app.write_flash.chunk_offset < app.write_flash.size)
                                              ? WRITE_FLASH_WRITE_CHUNK
                                              : WRITE_FLASH_FINISH;
    app.app_timer_ms = APP_WRITE_FLASH_TIMEOUT_MS;
    app.write_flash.current_state = WRITE_FLASH_WAITING;
    break;
  }
  break;

  case WRITE_FLASH_FINISH:
    FLASH->CR &= ~FLASH_CR_PG;
    flash_lock();
    app.write_flash.status = WRITE_STATUS_OK;
    app.write_flash.current_state = WRITE_FLASH_IDLE;
    app.write_flash.status_pending = true;
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

void App_GetUpdStatus(uint8_t *data)
{
  data[0] = (uint8_t)app.write_flash.generalStatus;
  data[1] = app.write_flash.percentage;
}

void App_GetEraseStatus(uint8_t *data)
{
  data[0] = (uint8_t)app.erase_flash.intermidiate_erase_status;
  data[1] = app.erase_flash.percentage;
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
  if (state == APP_ERASE_FLASH)
  {
    App_EraseFlashInit();
  }
  app.state = state;
}

void App_Tick(void)
{
  if (app.led_blink_ms > 0)
  {
    app.led_blink_ms--;
  }

  if (app.app_timer_ms > 0)
  {
    app.app_timer_ms--;
  }
}