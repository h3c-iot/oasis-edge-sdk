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

#include "oasis_tools.h"

int TOOLS_Get_UUID(char buf[37])
{
    const char *c = "89ab";
    char *p = buf;
    int n;

    for (n = 0; n < 16; ++n)
    {
        int b = rand() % 255;

        switch (n)
        {
        case 6:
            sprintf(p, "4%x", b % 15);
            break;
        case 8:
            sprintf(p, "%c%x", c[rand() % strlen(c)], b % 15);
            break;
        default:
            sprintf( p, "%02x", b);
            break;
        }

        p += 2;

        switch (n)
        {
        case 3:
        case 5:
        case 7:
        case 9:
            *p++ = '-';
            break;
        }
    }

    *p = 0;

    return SUCCESS;
}

int TOOLS_Get_Time_ISO8601(char *data)
{
    int ret = SUCCESS;
    time_t time_utc;
    struct tm tm_local;
    
    time(&time_utc);
    localtime_r(&time_utc, &tm_local);
    
    ret = snprintf(data, 30, "%04d-%02d-%02dT%02d:%02d:%02dZ",
                   tm_local.tm_year + 1900,
                   tm_local.tm_mon + 1,
                   tm_local.tm_mday,
                   tm_local.tm_hour,
                   tm_local.tm_min,
                   tm_local.tm_sec);

    return ret;
}

int TOOLS_Get_Time_Unix()
{
    time_t now;
    int unixtime = time(&now);
    return unixtime;
}

#ifdef __cplusplus
}
#endif