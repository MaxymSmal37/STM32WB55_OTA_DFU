/*
 * Logger.h
 *
 *  Created on: Dec 11, 2025
 *      Author: maxymsmal
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#define TIMESTUMPS_ENABLED 0
#define RTC_STM32_CUBE_MX 1

#define STANDART_LOG_COLOR "\033[0m"

#define LOG_BUFFER_SIZE 256   ///< Maximum log message length
#define LOG_TIMESTAMP_SIZE 24 ///< Timestamp buffer size

#define TIMESTUMPS_ENABLED 0

#define DEBUG_LOG(msg) logMessage(LOG_DEBUG, msg);
#define INFO_LOG(msg) logMessage(LOG_INFO, msg);
#define WARNING_LOG(msg) logMessage(LOG_WARNING, msg);
#define ERROR_LOG(msg) logMessage(LOG_ERROR, msg);
#define CRITICAL_LOG(msg) logMessage(LOG_CRITICAL, msg);
#define INFO_G_LOG(msg) logMessage(LOG_INFO_G, msg);

#define LOG_DEBUG(fmt, ...) logMessageFormatted(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) logMessageFormatted(LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_INFO_GREEN(fmt, ...) logMessageFormatted(LOG_INFO_G, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) logMessageFormatted(LOG_WARNING, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) logMessageFormatted(LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...) logMessageFormatted(LOG_CRITICAL, fmt, ##__VA_ARGS__)

#define LOG_IF(condition, level, fmt, ...)                  \
    do                                                      \
    {                                                       \
        if (condition)                                      \
        {                                                   \
            logMessageFormatted(level, fmt, ##__VA_ARGS__); \
        }                                                   \
    } while (0)

#define LOG_ASSERT(condition, msg)                                       \
    do                                                                   \
    {                                                                    \
        if (!(condition))                                                \
        {                                                                \
            logMessageFormatted(LOG_ERROR, "ASSERTION FAILED: %s", msg); \
        }                                                                \
    } while (0)

#define LOG_ASSERT_ACTION(condition, msg, action)                        \
    do                                                                   \
    {                                                                    \
        if (!(condition))                                                \
        {                                                                \
            logMessageFormatted(LOG_ERROR, "ASSERTION FAILED: %s", msg); \
            action;                                                      \
        }                                                                \
    } while (0)

enum LogLevel
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_CRITICAL,
    LOG_INFO_G
};

const char *logLevelToString(enum LogLevel level);
const char *logSetLogColor(enum LogLevel level);
void logMessage(enum LogLevel level, const char *message);
void logMessageFormatted(enum LogLevel level, const char *format, ...);
#endif /* _LOGGER_H_ */