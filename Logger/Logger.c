/*
 * Logger.c
 *
 *  Created on: Dec 11, 2025
 *  Author: maxymsmal
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "rtc.h"

#include "main.h"
#include "Logger.h"


const char* logLevelToString(enum LogLevel level)
{
    switch (level) {
    	case LOG_DEBUG:    return "DEBUG";
        case LOG_INFO:     return "INFO";
        case LOG_WARNING:  return "WARNING";
        case LOG_ERROR:    return "ERROR";
        case LOG_CRITICAL: return "CRITICAL";
        case LOG_INFO_G:   return "INFO";
        default:           return "UNKNOWN";
    }
}

const char* logSetLogColor(enum LogLevel level)
{
    switch (level) {
    	case LOG_DEBUG:    return "\033[36m";      /* Cyan */
        case LOG_INFO:     return "\033[37m";      /* White */
        case LOG_WARNING:  return "\033[33m";      /* Yellow */
        case LOG_ERROR:    return "\033[31m";      /* Red */
        case LOG_CRITICAL: return "\033[1;31;47m"; /* White on Red bg */
        case LOG_INFO_G:   return "\033[32m";      /* Green */
        default:           return "\033[90m";      /* Grey */
    }
}

void logMessage(enum LogLevel level, const char* message)
{
    char time_buffer[20];

    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    snprintf(time_buffer, sizeof(time_buffer) + 1, "20%02d-%02d-%02d %02d:%02d:%02d",
             sDate.Year, sDate.Month, sDate.Date,
             sTime.Hours, sTime.Minutes, sTime.Seconds);

    printf("%s\n\r[%s] [%s] %s\n\r%s\n\r",
           logSetLogColor(level),
           time_buffer,
           logLevelToString(level),
           message,
           STANDART_LOG_COLOR);

}

void logMessageFormatted(enum LogLevel level, const char* format, ...)
{
    char buffer[LOG_BUFFER_SIZE];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    logMessage(level, buffer);
}