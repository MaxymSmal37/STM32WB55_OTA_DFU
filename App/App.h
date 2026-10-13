/**
* @file App.h
*
* @brief Bootloader application header file.
*/

#ifndef APP_H
#define APP_H

typedef enum {
    APP_INIT,
    APP_WAITING_FOR_CONNECTION,
    APP_IDLE,
    APP_UPDATING,
    APP_ERROR,
    APP_STATE_COUNT
} AppState;

void App_Init(void);
void App_Run(void);     
AppState App_GetState(void);

#endif // APP_H