#include <string.h>

#include "AppComm.h"
#include "App.h"
#include "ble.h"
#include "custom_stm.h"

#define APP_COMM_STATUS_INTERVAL_BATCHES 10U

static uint8_t AppComm_BatchCount;

static uint8_t AppComm_CheckCRC8(CommandFrame_s *frame)
{
  return 0;
}

void AppComm_ParseCommandMessage(uint8_t *data)
{
  CommandFrame_s frame;

  frame.sof = data[FRAME_SOF];
  frame.command = data[FRAME_COMMAND];
  frame.length = data[FRAME_LEGTH];

  memcpy(frame.data, &data[FRAME_DATA], frame.length);

  frame.crc = data[FRAME_DATA + frame.length];

  if (AppComm_CheckCRC8(&frame) == 0)
  {
    AppComm_ProcessCommand(&frame);
  }
}

void AppComm_ParseDataBanchMessage(uint8_t *data, uint8_t length)
{
  if (data == NULL || length < DB_DATA_START)
  {
    return;
  }

  uint8_t batch_size = data[DB_BATCH_SIZE];
  if (batch_size > length - DB_DATA_START)
  {
    return;
  }

  if (App_FlashWriteBantch(data[DB_ADDRESS], batch_size, &data[DB_DATA_START]) == 0)
  {
    AppComm_BatchCount++;
    if (AppComm_BatchCount >= APP_COMM_STATUS_INTERVAL_BATCHES)
    {
      AppComm_BatchCount = 0;
      AppComm_SendUpdateStatus();
    }
  }
}

void AppComm_WriteDataBanch(uint32_t address, uint8_t size, uint8_t *data)
{
 
}

void AppComm_ProcessCommand(CommandFrame_s *command_frame)
{
  switch (command_frame->command)
  {
  case COMMAND_START_UPDATE:
    AppComm_BatchCount = 0;
    App_SetState(APP_START_UPDATE);
    break;

  case COMMAND_STOP_UPDATE:
    App_SetState(APP_END_UPDATE);
    break;

  case COMMAND_GET_STATUS:
    /* code */
    break;

  case COMMAND_SET_FW_SIZE:

    uint32_t size =  (uint32_t)command_frame->data[0]
                  | ((uint32_t)command_frame->data[1] << 8)
                  | ((uint32_t)command_frame->data[2] << 16)
                  | ((uint32_t)command_frame->data[3] << 24);

    App_SetUpdFwSize(size);
    break;

  case COMMAND_SET_FW_HASH:
    App_SetUpdFwHash(command_frame->data[0], &command_frame->data[1]);
    break;

  default:
    break;
  }
}

void AppComm_GetUpdateStatus(uint8_t *data)
{
  App_GetUpdStatus(data);
}

void AppComm_SendUpdateStatus(void)
{
  uint8_t status = 0;
  AppComm_GetUpdateStatus(&status);
  Custom_STM_App_Update_Char(CUSTOM_STM_STATUS, &status);
}