/**************************************************************************************
*     Copyright (c) 2004-2019 New H3C Technologies Co., Ltd. All rights reserved.     *
**************************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>
#include <sys/time.h>

#include "oasis_log.h"
#include "oasis_tools.h"

void LOG_App(char *filename, int lineno, int LOG_level, char *format, ...)
{
    static char msg_buf[LOG_BUF_MAX];
    va_list args;
    struct timeb ts;
    struct tm *timeinfo;
    static char *levelList[] = {"none", "fatal", "error", "warnning", "info", "debug", "trace"};

    if (LOG_LEVEL_MAX <= LOG_level || LOG_level <= LOG_LEVEL_NONE)
    {
        return;
    }

    ftime(&ts);
    timeinfo = localtime(&ts.time);
    strftime(msg_buf, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

    sprintf(&msg_buf[strlen(msg_buf)], LOG_PREFIX_FORMAT, ts.millitm, levelList[LOG_level], filename, lineno);

    va_start(args, format);
    vsnprintf(&msg_buf[strlen(msg_buf)], sizeof(msg_buf) - strlen(msg_buf), format, args);
    va_end(args);

    printf("%s\n", msg_buf);
    fflush(stdout);
}

#ifdef __cplusplus
}
#endif