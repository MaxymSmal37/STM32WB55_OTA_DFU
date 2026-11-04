/**
 * @file AppComm.h
 *
 * @brief Bootloader application communication header file.
 * @author Maxym Smal
 */

#ifndef APP_COMM_H
#define APP_COMM_H

#include <stdint.h>

#define APP_COMMAND_FRAME_FIELDS_SIZE 5
#define APP_COMMAND_DATA_MAX_LENGHT (23 - APP_COMMAND_FRAME_FIELDS_SIZE)
#define APP_COMM_STATUS_SIZE (1U + sizeof(uint32_t))

typedef  enum
{
   FRAME_SOF,
   FRAME_COMMAND,
   FRAME_LEGTH,
   FRAME_DATA
} FrameStructure_t;

typedef enum
{
  COMMAND_START_UPDATE,
  COMMAND_STOP_UPDATE,
  COMMAND_GET_STATUS,
  COMMAND_SET_FW_SIZE,
  COMMAND_SET_FW_HASH
} CommandIDs_t;

typedef enum
{
  DB_ADDRESS    = 0,
  DB_BATCH_SIZE = 4,
  DB_DATA_START = 5
} DataBatchFrame_t;

typedef enum 
{
  RESPONSE_COMMAND_OK = 0,
  RESPONSE_COMMAND_FAILED = 1
} ResponseIDs_t;

typedef struct
{
  uint8_t sof;
  uint8_t command;
  uint16_t length;
  uint8_t data[APP_COMMAND_DATA_MAX_LENGHT];
  uint8_t crc;
} CommandFrame_s;

typedef struct
{
  uint8_t sof;
  uint8_t length;
  uint8_t *data;
  uint8_t crc;
} DataFrame_s;


typedef struct
{
  uint8_t sof;
  uint16_t Error;
  uint8_t Ack;
  uint8_t crc;
} StatusFrame_s;

typedef struct
{
  uint8_t deseriliaseState;
} AppComm_s;

void AppComm_Init(void);
void AppComm_ParseCommandMessage(uint8_t *data, uint8_t length);
void AppComm_WriteDataBanch(uint32_t address, uint8_t size, uint8_t *data);
void AppComm_ProcessCommand(CommandFrame_s *command_frame);
void AppComm_ParseDataBanchMessage(uint8_t *data, uint8_t length);
void AppComm_GetUpdateStatus(uint8_t *data);
void AppComm_SendUpdateStatus(void);
void AppComm_GetEraseStatus(uint8_t *data);
void AppComm_SendEraseStatus(void);


#endif // APP_COMM_H