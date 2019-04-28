#pragma once

#include <assert.h>

enum class LoggerSeverity
{
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG
};

void init_logger();

void log(LoggerSeverity severity, const char *filename, int line, const char* fmt, ...);

void end_logger();

#define LOG_E(msg, ...) do { log(LoggerSeverity::LOG_ERROR,   __FILE__, __LINE__, msg, __VA_ARGS__); assert(false && "Check logfile for info"); } while (0);
#define LOG_W(msg, ...) log(LoggerSeverity::LOG_WARNING, __FILE__, __LINE__, msg, __VA_ARGS__)
#define LOG_I(msg, ...) log(LoggerSeverity::LOG_INFO,    __FILE__, __LINE__, msg, __VA_ARGS__)
#define LOG_D(msg, ...) log(LoggerSeverity::LOG_DEBUG ,  __FILE__, __LINE__, msg, __VA_ARGS__)

