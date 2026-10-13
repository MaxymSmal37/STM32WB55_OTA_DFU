#include "App.h"
#include <stdbool.h>

typedef AppState (*AppHandler)(void);

void App_Init(void)
{
}

static AppState App_Initialize(void)
{
  App_Init();
  return APP_WAITING_FOR_CONNECTION;
}

static AppState App_WaitingForConnection(void)
{
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

//-----------------------------------
//--------Dispatcher-----------------
//-----------------------------------

static const AppHandler handlers[APP_STATE_COUNT] = {
    [APP_INIT] = App_Initialize,
    [APP_WAITING_FOR_CONNECTION] = App_WaitingForConnection,
    [APP_IDLE] = App_Idle,
    [APP_UPDATING] = App_Updating,
    [APP_ERROR] = App_Error,
};

static AppState current = APP_INIT;

void App_Run(void)
{
  current = handlers[current]();
}

AppState App_GetState(void)
{
  return current;
}