#include <string.h>

#include "AppComm.h"
#include "App.h"
#include "ble.h"
#include "custom_stm.h"

#define APP_COMM_STATUS_INTERVAL_BATCHES 10U
#define APP_COMMAND_SOF 0xAAU

static uint8_t AppComm_BatchCount;

static uint8_t AppComm_CheckCRC8(const uint8_t *data, uint8_t length)
{
  uint8_t crc = 0;

  for (uint8_t i = 0; i < length; i++)
  {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8; bit++)
    {
      crc = (crc & 0x80U) != 0U ? (uint8_t)((crc << 1) ^ 0x07U) : (uint8_t)(crc << 1);
    }
  }

  return crc;
}

static uint8_t AppComm_IsCommandLengthValid(const CommandFrame_s *frame)
{
  switch (frame->command)
  {
  case COMMAND_START_ERASE:
  case COMMAND_START_UPDATE:
  case COMMAND_STOP_UPDATE:
  case COMMAND_GET_STATUS:
    return frame->length == 0U;

  case COMMAND_SET_FW_SIZE:
    return frame->length == sizeof(uint32_t);

  case COMMAND_SET_FW_HASH:
    return frame->length == (APP_HASH_PART_SIZE + 1U) &&
           frame->data[0] < APP_HASH_PARTS_COUNT;

  default:
    return 0U;
  }
}

static void AppComm_BuildStatusResponse(uint8_t *frame, uint8_t sof, uint8_t b1, uint8_t b2)
{
  frame[0] = sof;
  frame[1] = b1;
  frame[2] = b2;
  frame[3] = AppComm_CheckCRC8(frame, 3U);
}

void AppComm_ParseCommandMessage(uint8_t *data, uint8_t length)
{
  CommandFrame_s frame;
  uint8_t response = RESPONSE_COMMAND_FAILED;

  if (data == NULL || length < (APP_COMMAND_FRAME_FIELDS_SIZE - 1U))
  {
    return;
  }

  if ((data[FRAME_SOF] != APP_COMMAND_SOF ||
      data[FRAME_LEGTH] > sizeof(frame.data) ||
      length != (uint8_t)(data[FRAME_LEGTH] + APP_COMMAND_FRAME_FIELDS_SIZE - 1U) ||
      AppComm_CheckCRC8(data, (uint8_t)(length - 1U)) != data[length - 1U]) == 0U)
  {
    frame.sof = data[FRAME_SOF];
    frame.command = data[FRAME_COMMAND];
    frame.length = data[FRAME_LEGTH];

    if (frame.length > 0U)
    {
      memcpy(frame.data, &data[FRAME_DATA], frame.length);
    }

    frame.crc = data[length - 1U];

    if (AppComm_IsCommandLengthValid(&frame) != 0U)
    {
      response = AppComm_ProcessCommand(&frame);
    }
  }

  Custom_STM_App_Update_Char_Variable_Length(CUSTOM_STM_CMD, &response, 1U);
}

void AppComm_ParseDataBanchMessage(uint8_t *data, uint8_t length)
{
  if (data == NULL || length < (DB_DATA_START + 2U))
  {
    return;
  }

  uint8_t size = data[DB_BATCH_SIZE];

  if (size == 0U || size > APP_MAX_BATCH_SIZE ||
      length != (uint8_t)(DB_DATA_START + size + 1U))
  {
    return;
  }

  if (AppComm_CheckCRC8(data, (uint8_t)(length - 1U)) != data[length - 1U])
  {
    return;
  }

  uint32_t offset =  (uint32_t)data[0]
                   | ((uint32_t)data[1] << 8)
                   | ((uint32_t)data[2] << 16)
                   | ((uint32_t)data[3] << 24);

  App_FlashWriteBantch(offset, size, &data[DB_DATA_START]);
}

ResponseIDs_t AppComm_ProcessCommand(CommandFrame_s *command_frame)
{
  uint8_t response = RESPONSE_COMMAND_FAILED;

  switch (command_frame->command)
  {
  case COMMAND_START_ERASE:
    App_SetState(APP_ERASE_FLASH);

    response = RESPONSE_COMMAND_OK;
    break;

  case COMMAND_START_UPDATE:
    AppComm_BatchCount = 0;
    App_SetState(APP_START_UPDATE);
    response = RESPONSE_COMMAND_OK;
    break;

  case COMMAND_STOP_UPDATE:
    App_SetState(APP_END_UPDATE);
    response = RESPONSE_COMMAND_OK;
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
    response = RESPONSE_COMMAND_OK;
    break;

  case COMMAND_SET_FW_HASH:
    App_SetUpdFwHash(command_frame->data[0], &command_frame->data[1]);
    response = RESPONSE_COMMAND_OK;
    break;

  default:
    response = RESPONSE_COMMAND_FAILED;
    break;
  }

  return (ResponseIDs_t)response;
}

void AppComm_GetUpdateStatus(uint8_t *data)
{
  App_GetUpdStatus(data);
}

void AppComm_GetEraseStatus(uint8_t *data)
{
  App_GetEraseStatus(data);
}

void AppComm_CommandResponse(uint8_t response)
{
  Custom_STM_App_Update_Char_Variable_Length(CUSTOM_STM_CMD, &response, 1U);
}

void AppComm_SendUpdateStatus(void)
{
  uint8_t statusPayload[APP_COMM_STATUS_PAYLOAD_SIZE] = {0U};
  uint8_t statusFrame[APP_COMM_STATUS_SIZE] = {0U};

  AppComm_GetUpdateStatus(statusPayload);
  AppComm_BuildStatusResponse(statusFrame, APP_COMM_SOF, statusPayload[0], statusPayload[1]);

  Custom_STM_App_Update_Char(CUSTOM_STM_STATUS, statusFrame);
}

void AppComm_SendEraseStatus(void)
{
  uint8_t statusPayload[APP_COMM_STATUS_PAYLOAD_SIZE] = {0U};
  uint8_t statusFrame[APP_COMM_STATUS_SIZE] = {0U};

  AppComm_GetEraseStatus(statusPayload);
  AppComm_BuildStatusResponse(statusFrame, APP_COMM_SOF, statusPayload[0], statusPayload[1]);

  Custom_STM_App_Update_Char(CUSTOM_STM_STATUS, statusFrame);
}
