/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    App/custom_app.c
  * @author  MCD Application Team
  * @brief   Custom Example Application (Server)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "dbg_trace.h"
#include "ble.h"
#include "custom_app.h"
#include "custom_stm.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Common.h"
#include "BLE_AppConfig.h"
#include "App.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  /* SysInfo */
  /* USER CODE BEGIN CUSTOM_APP_Context_t */

  /* USER CODE END CUSTOM_APP_Context_t */

  uint16_t              ConnectionHandle;
} Custom_App_Context_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

static Custom_App_Context_t Custom_App_Context;

/**
 * END of Section BLE_APP_CONTEXT
 */

uint8_t UpdateCharData[512];
uint8_t NotifyCharData[512];
uint16_t Connection_Handle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* SysInfo */

/* USER CODE BEGIN PFP */
tBleStatus BleApp_SetFwVersion(void);
tBleStatus BleApp_SetMCUId(void);
tBleStatus BleApp_SetSerialNumber(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void Custom_STM_App_Notification(Custom_STM_App_Notification_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_1 */

  /* USER CODE END CUSTOM_STM_App_Notification_1 */
  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* USER CODE END CUSTOM_STM_App_Notification_Custom_Evt_Opcode */

    /* SysInfo */
    case CUSTOM_STM_BOOTLOADER_FW_VER_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_BOOTLOADER_FW_VER_READ_EVT */

      /* USER CODE END CUSTOM_STM_BOOTLOADER_FW_VER_READ_EVT */
      break;

    case CUSTOM_STM_MCU_ID_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_MCU_ID_READ_EVT */

      /* USER CODE END CUSTOM_STM_MCU_ID_READ_EVT */
      break;

    case CUSTOM_STM_DEVICE_SN_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_DEVICE_SN_READ_EVT */

      /* USER CODE END CUSTOM_STM_DEVICE_SN_READ_EVT */
      break;

    case CUSTOM_STM_NOTIFICATION_COMPLETE_EVT:
      /* USER CODE BEGIN CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */

      /* USER CODE END CUSTOM_STM_NOTIFICATION_COMPLETE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_STM_App_Notification_default */

      /* USER CODE END CUSTOM_STM_App_Notification_default */
      break;
  }
  /* USER CODE BEGIN CUSTOM_STM_App_Notification_2 */

  /* USER CODE END CUSTOM_STM_App_Notification_2 */
  return;
}

void Custom_APP_Notification(Custom_App_ConnHandle_Not_evt_t *pNotification)
{
  /* USER CODE BEGIN CUSTOM_APP_Notification_1 */

  /* USER CODE END CUSTOM_APP_Notification_1 */

  switch (pNotification->Custom_Evt_Opcode)
  {
    /* USER CODE BEGIN CUSTOM_APP_Notification_Custom_Evt_Opcode */

    /* USER CODE END P2PS_CUSTOM_Notification_Custom_Evt_Opcode */
    case CUSTOM_CONN_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_CONN_HANDLE_EVT */
      App_DeviceConnected();
      /* USER CODE END CUSTOM_CONN_HANDLE_EVT */
      break;

    case CUSTOM_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN CUSTOM_DISCON_HANDLE_EVT */
      App_DeviceDisconnected();
      /* USER CODE END CUSTOM_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN CUSTOM_APP_Notification_default */

      /* USER CODE END CUSTOM_APP_Notification_default */
      break;
  }

  /* USER CODE BEGIN CUSTOM_APP_Notification_2 */

  /* USER CODE END CUSTOM_APP_Notification_2 */

  return;
}

void Custom_APP_Init(void)
{
  /* USER CODE BEGIN CUSTOM_APP_Init */
  BleApp_SetFwVersion();
  BleApp_SetMCUId();
  BleApp_SetSerialNumber();
  APP_DBG_MSG("BootloaderFwVer update ret=0x%02X\n", ret);
  /* USER CODE END CUSTOM_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* SysInfo */

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
tBleStatus BleApp_SetFwVersion(void)
{
  tBleStatus ret;
  Botloader_FwVersion fwVersion = Bootloader_getFirmwareVersion();

  UpdateCharData[FW_VER_PREFIX_MAJOR] = fwVersion.prefix_major;   // prefix major
  UpdateCharData[FW_VER_PREFIX_MINOR] = fwVersion.prefix_minor;   // prefix minor
  UpdateCharData[FW_VER_MAJOR] = fwVersion.major;   // major
  UpdateCharData[FW_VER_MINOR] = fwVersion.minor;   // minor
  UpdateCharData[FW_VER_PATCH] = fwVersion.patch;   // patch

  ret = Custom_STM_App_Update_Char(CUSTOM_STM_BOOTLOADER_FW_VER, UpdateCharData);

  APP_DBG_MSG("BootloaderFwVer update ret=0x%02X\n", ret);
  return ret;
}

tBleStatus BleApp_SetMCUId(void)
{
  uint32_t mcuId = Boootloader_getMCUId();

  memcpy(UpdateCharData, &mcuId, sizeof(mcuId));
  return Custom_STM_App_Update_Char(CUSTOM_STM_MCU_ID, UpdateCharData);
}

tBleStatus BleApp_SetSerialNumber(void)
{
  uint32_t serialNumber = Bootloader_getSerialNumber();

  memcpy(UpdateCharData, &serialNumber, sizeof(serialNumber));
  return Custom_STM_App_Update_Char(CUSTOM_STM_DEVICE_SN, UpdateCharData);
}

/* USER CODE END FD_LOCAL_FUNCTIONS*/
