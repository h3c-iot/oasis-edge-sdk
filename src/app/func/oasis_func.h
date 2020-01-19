/**************************************************************************************
*     Copyright (c) 2004-2019 New H3C Technologies Co., Ltd. All rights reserved.     *
**************************************************************************************/

#ifndef _OASIS_FUNC_H_
#define _OASIS_FUNC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define TOPIC_REQ_CALL_FUNCTION  "app/request/oasis/callFunction"
#define TOPIC_RSP_CALL_FUNCTION  "oasis/response/app/callFunction"
#define TOPIC_RSP_CALL_FUNCTION_QOS     MQTT_QOS0

#define FUNC_NAME  "test_demo"
#define FUNC_TYPE_PYTHON3  "python3"

int FUNC_Call_Req(void *context, int payload[2]);
int FUNC_Call_Rsp(char *payload);

#ifdef __cplusplus
}
#endif

#endif
