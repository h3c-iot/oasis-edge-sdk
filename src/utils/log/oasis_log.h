/**************************************************************************************
*     Copyright (c) 2004-2019 New H3C Technologies Co., Ltd. All rights reserved.     *
**************************************************************************************/

#ifndef _OASIS_LOG_H_
#define _OASIS_LOG_H_

#ifdef __cplusplus
extern "C"
{
#endif

enum LOG_LEVELS {
    INVALID_LEVEL = -1,
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE,
    LOG_LEVEL_MAX
};

#define LOG_BUF_MAX 1024
#define LOG_PREFIX_FORMAT  ".%.3hu [%s] %s(%d): "
#define LOG_LENGTH_UUID      37
#define LOG_LENGTH_TIMESTAMP 30

#define LOG_FATAL(a,...)    LOG_App(__FILE__, __LINE__, LOG_LEVEL_FATAL, a, ##__VA_ARGS__)
#define LOG_ERROR(a,...)    LOG_App(__FILE__, __LINE__, LOG_LEVEL_ERROR, a, ##__VA_ARGS__)
#define LOG_WARNNING(a,...) LOG_App(__FILE__, __LINE__, LOG_LEVEL_WARNNING, a, ##__VA_ARGS__)
#define LOG_INFO(a,...)     LOG_App(__FILE__, __LINE__, LOG_LEVEL_INFO, a, ##__VA_ARGS__)
#define LOG_DEBUG(a,...)    LOG_App(__FILE__, __LINE__, LOG_LEVEL_DEBUG, a, ##__VA_ARGS__)
#define LOG_TRACE(a,...)    LOG_App(__FILE__, __LINE__, LOG_LEVEL_TRACE, a, ##__VA_ARGS__)

void LOG_App(char *filename, int lineno, int LOG_level, char *format, ...);

#endif

#ifdef __cplusplus
}
#endif