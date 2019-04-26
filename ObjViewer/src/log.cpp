#include <stdio.h>
#include <stdarg.h>

#include "log.h"

static FILE *log_file;

void init_logger()
{
    log_file = fopen("log.txt", "a");
    
    if (!log_file)
    {
        fprintf(stderr, "[ERROR] Cannot initialize logger\n");
        return;
    }

    fprintf(log_file, "\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

void log(LoggerSeverity severity, const char *filename, int line, const char* fmt, ...)
{
    if (log_file)
    {
        char msg[4096] = {0};

        va_list args;
        va_start(args, fmt);

        vsnprintf(msg, sizeof(msg), fmt, args);

        va_end(args);
        
        fprintf(log_file, "%s:%d\n", filename, line);

        switch (severity)
        {
            case LoggerSeverity::LOG_ERROR:
                fprintf(log_file, "[ERROR] - ");
                break;
            case LoggerSeverity::LOG_WARNING:
                fprintf(log_file, "[WARNING] - ");
                break;
            case LoggerSeverity::LOG_INFO:
                fprintf(log_file, "[INFO] - ");
                break;
            case LoggerSeverity::LOG_DEBUG:
                fprintf(log_file, "[DEBUG] - ");
                break;
            default:
                fprintf(log_file, "[UNKNOWN] - ");
                break;
        }
        fprintf(log_file, "%s\n\n", msg);
        
        fflush(log_file);
    }
    else
    {
        fprintf(stderr, "[WARNING] logger not initialized!\n");
    }
}

void end_logger()
{
    if (log_file)
    {
        fclose(log_file);
        log_file = nullptr;
    }
}
