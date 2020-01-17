/**************************************************************************************
*     Copyright (c) 2004-2019 New H3C Technologies Co., Ltd. All rights reserved.     *
**************************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "MQTTAsync.h"
#include "cJSON.h"
#include "oasis_func.h"
#include "oasis_log.h"
#include "oasis_tools.h"

int FUNC_Call_Req(void *context, int payload[2])
{
    MQTTAsync c = (MQTTAsync)context;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    int ret;
    char *out;
    char uuid[LOG_LENGTH_UUID] = {0};
    char timestamp[LOG_LENGTH_TIMESTAMP] = {0};

    cJSON *root = NULL;
    cJSON *param = NULL;

    ret = TOOLS_Get_Time_ISO8601(timestamp);
    ret = TOOLS_Get_UUID(uuid);

    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "timestamp", timestamp);
    cJSON_AddStringToObject(root, "token", uuid);
    cJSON_AddStringToObject(root, "name", FUNC_NAME);
    cJSON_AddStringToObject(root, "service", FUNC_TYPE_PYTHON3);
    cJSON_AddItemToObject(root, "param", param = cJSON_CreateIntArray(payload, 2));
    out = cJSON_PrintUnformatted(root);

    pubmsg.payload = out;
    pubmsg.payloadlen = strlen(out);
    pubmsg.qos = 0;
    pubmsg.retained = 0;

    ret = MQTTAsync_send(c, TOPIC_REQ_CALL_FUNCTION, pubmsg.payloadlen, pubmsg.payload, pubmsg.qos, pubmsg.retained, NULL);
    if (ret != MQTTASYNC_SUCCESS)
    {
        LOG_WARNNING("Failed to send mqtt message.");
        MQTTAsync_destroy(&c);
        return ERROR;
    }

    LOG_DEBUG("publish topic: %s, payload:%s", TOPIC_REQ_CALL_FUNCTION, out);

    return SUCCESS;
}

int FUNC_Call_Rsp(char *payload)
{
    cJSON *root;
    cJSON *name, *service, *statusCode, *statusStr, *token, *timestamp;

    root = cJSON_Parse(payload);
    if(!root)
    {
        LOG_WARNNING("Failed to parse json payload, can't get root.");
        return ERROR;
    }

    name = cJSON_GetObjectItem(root, "name");
    if(!name)
    {
        LOG_WARNNING("Failed to parse json payload, can't get name.");
        cJSON_Delete(root);
        return ERROR;
    }

    service = cJSON_GetObjectItem(root, "service");
    if(!service)
    {
        LOG_WARNNING("Failed to parse json payload, can't get service.");
        cJSON_Delete(root);
        return ERROR;
    }

    statusCode = cJSON_GetObjectItem(root, "statusCode");
    if(!statusCode)
    {
        LOG_WARNNING("Failed to parse json payload, can't get statusCode.");
        cJSON_Delete(root);
        return ERROR;
    }

    statusStr = cJSON_GetObjectItem(root, "statusStr");
    if(!statusStr)
    {
        LOG_WARNNING("Failed to parse json payload, can't get statusStr.");
        cJSON_Delete(root);
        return ERROR;
    }

    token = cJSON_GetObjectItem(root, "token");
    if(!token)
    {
        LOG_WARNNING("Failed to parse json payload, can't get token.");
        cJSON_Delete(root);
        return ERROR;
    }

    timestamp = cJSON_GetObjectItem(root, "timestamp");
    if(!timestamp)
    {
        LOG_WARNNING("Failed to parse json payload, can't get timestamp.");
        cJSON_Delete(root);
        return ERROR;
    }

    if (0 == strcmp(name->string, FUNC_NAME) && 
        0 == strcmp(service->string, FUNC_TYPE_PYTHON3))
    {
        if(200 == statusCode->valueint)
        {
            cJSON_Delete(root);
            return SUCCESS;
        }
        else
        {
            LOG_WARNNING("Can't get function response message.");
            cJSON_Delete(root);
            return ERROR;
        }
    }

    cJSON_Delete(root);

    return ERROR;
}


#ifdef __cplusplus
}
#endif