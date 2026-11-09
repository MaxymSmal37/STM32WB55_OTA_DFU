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
#include "AppComm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  /* SysInfo */
  /* FirmwareTransfer */
  uint8_t               Cmd_Notification_Status;
  uint8_t               Status_Notification_Status;
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
#define SYSINFO_STR_LEN   16U   /* FW version, wireless info, serial */
#define SYSINFO_HWREV_LEN  8U
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
/* FirmwareTransfer */
static void Custom_Cmd_Update_Char(void);

static void Custom_Cmd_Send_Notification(void);

static void Custom_Status_Update_Char(void);

static void Custom_Status_Send_Notification(void);

/* USER CODE BEGIN PFP */
tBleStatus BleApp_SetFwVersion(void);
tBleStatus BleApp_SetSerialNumber(void);
static tBleStatus BleApp_PublishSysInfo(void);
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

    case CUSTOM_STM_WIRELESS_FW_VER_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_WIRELESS_FW_VER_READ_EVT */

      /* USER CODE END CUSTOM_STM_WIRELESS_FW_VER_READ_EVT */
      break;

    case CUSTOM_STM_DEVICE_SN_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_DEVICE_SN_READ_EVT */

      /* USER CODE END CUSTOM_STM_DEVICE_SN_READ_EVT */
      break;

    case CUSTOM_STM_HW_REV_READ_EVT:
      /* USER CODE BEGIN CUSTOM_STM_HW_REV_READ_EVT */

      /* USER CODE END CUSTOM_STM_HW_REV_READ_EVT */
      break;

    /* FirmwareTransfer */
    case CUSTOM_STM_CMD_WRITE_EVT:
      /* USER CODE BEGIN CUSTOM_STM_CMD_WRITE_EVT */
      AppComm_ParseCommandMessage(pNotification->DataTransfered.pPayload,
                                  pNotification->DataTransfered.Length);
      /* USER CODE END CUSTOM_STM_CMD_WRITE_EVT */
      break;

    case CUSTOM_STM_CMD_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_CMD_NOTIFY_ENABLED_EVT */

      /* USER CODE END CUSTOM_STM_CMD_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_CMD_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_CMD_NOTIFY_DISABLED_EVT */

      /* USER CODE END CUSTOM_STM_CMD_NOTIFY_DISABLED_EVT */
      break;

    case CUSTOM_STM_DATA_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN CUSTOM_STM_DATA_WRITE_NO_RESP_EVT */
      AppComm_ParseDataBanchMessage(pNotification->DataTransfered.pPayload,
                                    pNotification->DataTransfered.Length);
      /* USER CODE END CUSTOM_STM_DATA_WRITE_NO_RESP_EVT */
      break;

    case CUSTOM_STM_STATUS_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_STATUS_NOTIFY_ENABLED_EVT */

      /* USER CODE END CUSTOM_STM_STATUS_NOTIFY_ENABLED_EVT */
      break;

    case CUSTOM_STM_STATUS_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN CUSTOM_STM_STATUS_NOTIFY_DISABLED_EVT */

      /* USER CODE END CUSTOM_STM_STATUS_NOTIFY_DISABLED_EVT */
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
  BleApp_PublishSysInfo();

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
/* FirmwareTransfer */
__USED void Custom_Cmd_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Cmd_UC_1*/

  /* USER CODE END Cmd_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_CMD, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Cmd_UC_Last*/

  /* USER CODE END Cmd_UC_Last*/
  return;
}

__USED void Custom_Cmd_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Cmd_NS_1*/

  /* USER CODE END Cmd_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_CMD, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Cmd_NS_Last*/

  /* USER CODE END Cmd_NS_Last*/

  return;
}

__USED void Custom_Status_Update_Char(void) /* Property Read */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Status_UC_1*/

  /* USER CODE END Status_UC_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_STATUS, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Status_UC_Last*/

  /* USER CODE END Status_UC_Last*/
  return;
}

__USED void Custom_Status_Send_Notification(void) /* Property Notification */
{
  uint8_t updateflag = 0;

  /* USER CODE BEGIN Status_NS_1*/

  /* USER CODE END Status_NS_1*/

  if (updateflag != 0)
  {
    Custom_STM_App_Update_Char(CUSTOM_STM_STATUS, (uint8_t *)UpdateCharData);
  }

  /* USER CODE BEGIN Status_NS_Last*/

  /* USER CODE END Status_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
tBleStatus BleApp_SetFwVersion(void)
{
  tBleStatus ret;
  uint8_t fwVersion[SYSINFO_STR_LEN] = {0U};
  uint8_t fwVersionLen;
  
  fwVersionLen = Bootloader_getFirmwareVersion(fwVersion);

  memcpy(UpdateCharData, fwVersion, fwVersionLen);

  ret = Custom_STM_App_Update_Char(CUSTOM_STM_BOOTLOADER_FW_VER, UpdateCharData);

  APP_DBG_MSG("BootloaderFwVer update ret=0x%02X\n", ret);
  return ret;
}

tBleStatus BleApp_SetHW_Revision(void)
{
  uint8_t hwRev[SYSINFO_HWREV_LEN] = {0U};
  uint8_t hwRevLen = Bootloader_getHwRevision(hwRev);

  memcpy(UpdateCharData, hwRev, hwRevLen);

  return Custom_STM_App_Update_Char(CUSTOM_STM_HW_REV, UpdateCharData);
}

tBleStatus BleApp_SetSerialNumber(void)
{
  uint8_t serialNumber[SYSINFO_STR_LEN] = {0U};
  uint8_t serialNumberLen = Bootloader_getSerialNumber(serialNumber);

  memcpy(UpdateCharData, serialNumber, serialNumberLen);

  return Custom_STM_App_Update_Char(CUSTOM_STM_DEVICE_SN, UpdateCharData);
}

static tBleStatus BleApp_PublishSysInfo(void)
{
  uint8_t buf[SYSINFO_STR_LEN];
  tBleStatus ret;

  memset(buf, 0, sizeof(buf));
  (void)Bootloader_getFirmwareVersion(buf);
  ret = Custom_STM_App_Update_Char(CUSTOM_STM_BOOTLOADER_FW_VER, buf);
  APP_DBG_MSG("FwVer update ret=0x%02X\n", ret);

  memset(buf, 0, sizeof(buf));
  (void)Bootloader_getWirelessFwVersion(buf);
  ret = Custom_STM_App_Update_Char(CUSTOM_STM_WIRELESS_FW_VER, buf);
  APP_DBG_MSG("WirelessVer update ret=0x%02X\n", ret);

  memset(buf, 0, sizeof(buf));
  (void)Bootloader_getSerialNumber(buf);
  ret = Custom_STM_App_Update_Char(CUSTOM_STM_DEVICE_SN, buf);
  APP_DBG_MSG("Serial update ret=0x%02X\n", ret);

  memset(buf, 0, sizeof(buf));
  (void)Bootloader_getHwRevision(buf);
  ret = Custom_STM_App_Update_Char(CUSTOM_STM_HW_REV, buf);
  APP_DBG_MSG("HwRev update ret=0x%02X\n", ret);

  return ret;
}
/* USER CODE END FD_LOCAL_FUNCTIONS*/
