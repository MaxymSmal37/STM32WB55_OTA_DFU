/**
 * @file App.h
 *
 * @brief Bootloader application header file.
 * @author Maxym Smal
 */

#ifndef APP_H
#define APP_H

#include <stdint.h> 

#define APP_LED_BLINK_INTERVAL_MS 100

#define APP_FW_HASH_SIZE 32U

#define APP_HASH_PART_SIZE 16
#define APP_HASH_PARTS_COUNT 2
#define APP_HASH_PART_1 0
#define APP_HASH_PART_2 1
#define APP_FW_SIZE_LENGH 4
#define FIRST_MEMORY_PAGE 0

typedef enum
{
  APP_INIT = 0,
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
  ERASE_FLASH_IDLE = 0,
  ERASE_WAITING,
  ERASE_START_PAGE_PROCESSING,
  ERASE_PAGE_PROCESSING,
  ERASE_FINISH_PAGE_PROCESSING,
} EraseFlashState_s;

typedef enum
{
  ERASE_STATUS_OK = 0,
  ERASE_STATUS_INPROGESS,
  ERASE_STATUS_FAULT
} EraseFlashStatus_s;

typedef enum
{
  WRITE_FLASH_IDLE,
  WRITE_FLASH_WAITING,       
  WRITE_FLASH_PREPARE,        
  WRITE_FLASH_WRITE_CHUNK,    
  WRITE_FLASH_FINISH,         
} WriteFlashState_t;

typedef enum
{
  WRITE_STATUS_IDLE = 0, 
  WRITE_STATUS_BUSY, 
  WRITE_STATUS_OK, 
  WRITE_STATUS_ERROR
} WriteFlashStatus_s;

typedef enum
{
  BOOTLOADER_CRC_FAILED = 0,
  BOOTLOADER_HASH_SET_FAILED
} BootloaderAppStatusCode_s;

typedef enum
{
  UG_STATUS_OK    =  0,
  UG_STATUS_ERROR = -1,
} UpdateGeneralStatus_t;

typedef struct
{
  uint32_t current_address;
  EraseFlashState_s current_state;
  EraseFlashState_s state_after_waiting;
  EraseFlashStatus_s status;
  uint32_t page_number;
} erase_flashing_s;

typedef struct 
{
  WriteFlashState_t    current_state;
  WriteFlashState_t    state_after_waiting;
  WriteFlashStatus_s   status;

  uint32_t       address;   /* base address of this write */
  const uint8_t *data;      /* must stay valid until status != BUSY */
  uint32_t       size;
  uint32_t       offset;    /* replaces loop variable i */
} write_flashing_s;

typedef struct
{
  uint8_t  hash[APP_FW_HASH_SIZE];
  uint32_t size;
} AppUpdateFwInfo_s;

typedef struct
{
  UpdateGeneralStatus_t generalStatus;
  uint32_t currentPageNumber;
} UpdateStatusFrame_s;


typedef struct
{
  erase_flashing_s erase_flash;
  write_flashing_s write_flash;
  AppState state;
  AppUpdateFwInfo_s fwInfo;
  volatile uint16_t led_blink_ms;
  UpdateStatusFrame_s updateStatus;
} App_s;



void App_Init(void);
void App_Run(void);
AppState App_GetState(void);
void App_SetState(AppState state);
void App_ProcessLed(AppLedMode_s led_mode);
void App_Tick(void);
void App_DeviceConnected(void);
void App_DeviceDisconnected(void);
void App_SetUpdFwSize(uint32_t size);
void App_SetUpdFwHash(uint8_t part, uint8_t *hashData);
void App_GetUpdStatus(uint8_t* data);
uint8_t App_FlashWriteBantch(uint32_t address, uint8_t size, uint8_t *data);


#endif // APP_H