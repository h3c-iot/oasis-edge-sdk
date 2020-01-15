/**************************************************************************************
*     Copyright (c) 2004-2019 New H3C Technologies Co., Ltd. All rights reserved.     *
**************************************************************************************/

#ifndef _OASIS_TOOLS_H_
#define _OASIS_TOOLS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define SUCCESS 0
#define ERROR -1  

int TOOLS_Get_Time_ISO8601(char *data);
int TOOLS_Get_Time_Unix();
int TOOLS_Get_UUID(char buf[37]);

#endif

#ifdef __cplusplus
}
#endif