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
#include <fcntl.h>
#include <time.h>

#include "MQTTAsync.h"
#include "cJSON.h"
#include "oasis_log.h"
#include "oasis_tools.h"
#include "oasis_shadow.h"

int SHADOW_Connect_Req(void *context, char *topic, char *productKey, char *deviceID, int *tag)
{
    int ret;
    char *out;
    char uuid[LOG_LENGTH_UUID] = {0};
    char timestamp[30] = {0};

    MQTTAsync c = (MQTTAsync)context;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;

    cJSON *root = NULL;
    cJSON *deviceInfo = NULL, *deviceList = NULL;

    ret = TOOLS_Get_Time_ISO8601(timestamp);
    ret = TOOLS_Get_UUID(uuid);

    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "timestamp", timestamp);
    cJSON_AddStringToObject(root, "token", uuid);
    cJSON_AddItemToObject(root, "deviceInfo", deviceInfo = cJSON_CreateArray());
    cJSON_AddItemToArray(deviceInfo, deviceList = cJSON_CreateObject());

    cJSON_AddStringToObject(deviceList, "productKey", productKey);
    cJSON_AddStringToObject(deviceList, "deviceID", deviceID);

    out = cJSON_PrintUnformatted(root);

    pubmsg.payload = out;
    pubmsg.payloadlen = strlen(out);
    pubmsg.qos = 0;
    pubmsg.retained = 0;

    cJSON_Delete(root);

    ret = MQTTAsync_send(c, topic, pubmsg.payloadlen, pubmsg.payload, pubmsg.qos, pubmsg.retained, NULL);
    if (ret != MQTTASYNC_SUCCESS)
    {
        LOG_WARNNING("Failed to send mqtt message.");
        MQTTAsync_destroy(&c);
        free(out);
        return ERROR;
    }

    *tag = TAG_REQ;

    LOG_DEBUG("publish topic: %s, payload:%s", topic, out);

    free(out);
    return SUCCESS;
}

int SHADOW_Connect_Rsp(char *payload, char *productKey, char *deviceID, DEV_SHADOW_S *devShadow, int *tag)
{
    cJSON *root, *devinfo, *devinfo_list, *devinfo_item;
    cJSON *rsp_productKey, *rsp_deviceID, *state;
    int devinfo_size, i;
    char *devinfo_fmt;

    root = cJSON_Parse(payload);
    if(!root)
    {
        LOG_WARNNING("Failed to parse json payload, can't get root.");
        return ERROR;
    }
    devinfo = cJSON_GetObjectItem(root, "deviceInfo");
    if(!devinfo)
    {
        LOG_WARNNING("Failed to parse json payload, can't get deviceInfo.");
        cJSON_Delete(root);
        return ERROR;
    }
    devinfo_size = cJSON_GetArraySize(devinfo);
    for(i = 0; i < devinfo_size; i++) 
    {
        devinfo_list = cJSON_GetArrayItem(devinfo, i);
        if(!devinfo_list)
        {
            LOG_WARNNING("Failed to parse json payload, can't get devinfo_list");
            cJSON_Delete(root);
            return ERROR;
        }
        devinfo_fmt = cJSON_PrintUnformatted(devinfo_list);
        if(!devinfo_fmt)
        {
            LOG_WARNNING("Failed to parse json payload, can't get devinfo_fmt.");
            cJSON_Delete(root);
            return ERROR;
        }
        devinfo_item = cJSON_Parse(devinfo_fmt);
        if(!devinfo_item)
        {
            LOG_WARNNING("Failed to parse json payload, can't get devinfo_item.");
            cJSON_Delete(root);
            return ERROR;
        }
        rsp_productKey = cJSON_GetObjectItem(devinfo_item, "productKey");
        if(!rsp_productKey)
        {
            LOG_WARNNING("Failed to parse json payload, can't get productKey.");
            cJSON_Delete(root);
            return ERROR;
        }
        rsp_deviceID = cJSON_GetObjectItem(devinfo_item, "deviceID");
        if(!rsp_deviceID)
        {
            LOG_WARNNING("Failed to parse json payload, can't get deviceID.");
            cJSON_Delete(root);
            return ERROR;
        }
        state = cJSON_GetObjectItem(devinfo_item, "state");
        if(!state)
        {
            LOG_WARNNING("Failed to parse json payload, can't get state.");
            cJSON_Delete(root);
            return ERROR;
        }

        if (0 == strcmp(productKey, rsp_productKey->valuestring) && 
            0 == strcmp(deviceID, rsp_deviceID->valuestring) && 
            NULL != state->valuestring)
        {
            snprintf(devShadow->status, LENGTH_STATUS, "%s", state->valuestring);
        }        
    }
    cJSON_Delete(root);

    *tag = TAG_RSP;

    return SUCCESS;
}

int SHADOW_Set_Req(void *context, DEV_SHADOW_S *devShadow, int *tag)
{
    MQTTAsync c = (MQTTAsync)context;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    int ret;
    char timestamp[30] = {0};
    char uuid[LOG_LENGTH_UUID] = {0};
    char *out;

    cJSON *root = NULL;
    cJSON *state = NULL, *metadata = NULL;
    cJSON *stateDesired = NULL, *stateReported = NULL;
    cJSON *metadataDesired = NULL, *metadataReported = NULL;
    cJSON *metadataDesiredSwitch = NULL, *metadataReportedSwitch = NULL;

    ret = TOOLS_Get_Time_ISO8601(timestamp);
    ret = TOOLS_Get_UUID(uuid);

    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "productKey", devShadow->productKey);
    cJSON_AddStringToObject(root, "deviceID", devShadow->deviceID);
    cJSON_AddNumberToObject(root, "version", devShadow->version);
    cJSON_AddStringToObject(root, "timestamp", timestamp);
    cJSON_AddStringToObject(root, "token", uuid);

    cJSON_AddItemToObject(root, "state", state = cJSON_CreateObject());
    cJSON_AddItemToObject(state, "reported", stateReported = cJSON_CreateObject()); 
    cJSON_AddStringToObject(stateReported, "switch", devShadow->state->reported->sw);
    cJSON_AddItemToObject(root, "metadata", metadata = cJSON_CreateObject());
    cJSON_AddItemToObject(metadata, "reported", metadataReported = cJSON_CreateObject());
    cJSON_AddItemToObject(metadataReported, "switch", metadataReportedSwitch = cJSON_CreateObject());
    cJSON_AddStringToObject(metadataReportedSwitch, "timestamp", devShadow->state->reported->swTimestamp);

    out = cJSON_PrintUnformatted(root);

    pubmsg.payload = out;
    pubmsg.payloadlen = strlen(out);
    pubmsg.qos = 0;
    pubmsg.retained = 0;

    cJSON_Delete(root);

    ret = MQTTAsync_send(c, TOPIC_REQ_SET_SHADOW, pubmsg.payloadlen, pubmsg.payload, pubmsg.qos, pubmsg.retained, NULL);
    if (ret != MQTTASYNC_SUCCESS)
    {
        LOG_WARNNING("Failed to send mqtt message.");
        MQTTAsync_destroy(&c);
        free(out);
        return ERROR;
    }

    *tag = TAG_REQ;

    LOG_DEBUG("publish topic:%s payload:%s", TOPIC_REQ_SET_SHADOW, out);

    free(out);
    return SUCCESS;
}

int SHADOW_Set_Rsp(char *payload, char *productKey, char *deviceID, DEV_SHADOW_S *devShadow, int *tag)
{
    cJSON *root;
    cJSON *rsp_productKey, *rsp_deviceID, *statusCode, *statusStr, *version;

    root = cJSON_Parse(payload);
    if(!root)
    {
        LOG_WARNNING("Failed to parse json payload, can't get root.");
        return ERROR;
    }

    rsp_productKey = cJSON_GetObjectItem(root, "productKey");
    if(!rsp_productKey)
    {
        LOG_WARNNING("Failed to parse json payload, can't get productKey.");
        cJSON_Delete(root);
        return ERROR;
    }
    rsp_deviceID = cJSON_GetObjectItem(root, "deviceID");
    if(!rsp_deviceID)
    {
        LOG_WARNNING("Failed to parse json payload, can't get deviceID.");
        cJSON_Delete(root);
        return ERROR;
    }
    statusCode = cJSON_GetObjectItem(root, "statusCode");
    if(!statusCode)
    {
        LOG_WARNNING("Failed to parse json payload, can't get productKey.");
        cJSON_Delete(root);
        return ERROR;
    }
    statusStr = cJSON_GetObjectItem(root, "deviceID");
    if(!statusStr)
    {
        LOG_WARNNING("Failed to parse json payload, can't get deviceID.");
        cJSON_Delete(root);
        return ERROR;
    }
    version = cJSON_GetObjectItem(root, "version");
    if(!version)
    {
        LOG_WARNNING("Failed to parse json payload, can't get version.");
        cJSON_Delete(root);
        return ERROR;
    }

    if (0 == strcmp(productKey, rsp_productKey->valuestring) && 
        0 == strcmp(deviceID, rsp_deviceID->valuestring))
    {
        if(200 == statusCode->valueint)
        {
            if(version->valueint == devShadow->version)
            {
                *tag = TAG_RSP;
            }
            else
            {
                LOG_WARNNING("Shadow version is not match.");
            }                      
        }
        else
        {
            LOG_WARNNING("Can't set shadow: %s.", statusStr->valuestring);
        }
    }

    cJSON_Delete(root);

    return SUCCESS;
}

int SHADOW_Update(char *payload, DEV_SHADOW_S *devShadow)
{
    cJSON *root = NULL;
    cJSON *productKey = NULL, *deviceID = NULL, *state = NULL, *metadata = NULL, *timestamp = NULL, *version = NULL;
    cJSON *stateDesired = NULL;
    cJSON *stateDesiredSwitch = NULL;
    cJSON *metadataDesired = NULL;
    cJSON *metadataDesiredSwitch = NULL;
    cJSON *metadataDesiredSwitchTimestamp = NULL;
    
    char *tmpSw = "\0", *tmpTimestamp = "\0";
    
    root = cJSON_Parse(payload);
    if(!root)
    {
        LOG_WARNNING("Failed to parse json payload, can't get root.");
        return ERROR;
    }
    timestamp = cJSON_GetObjectItem(root, "timestamp");
    if(!timestamp)
    {
        LOG_WARNNING("Failed to parse json payload, can't get productKey.");
        return ERROR;
    }

    if(0 > strcmp(timestamp->valuestring, devShadow->timestamp))
    {
        LOG_WARNNING("Failed to set shadow, the message is too late.");
        return ERROR;
    }
    version = cJSON_GetObjectItem(root, "version");
    if(!version)
    {
        LOG_WARNNING("Failed to parse json payload, can't get productKey.");
        return ERROR;
    }
    else if(version->valueint < devShadow->version)
    {
        LOG_WARNNING("Failed to set shadow, the version is too small.");
        return ERROR;
    }
    productKey = cJSON_GetObjectItem(root, "productKey");
    if(!productKey)
    {
        LOG_WARNNING("Failed to parse json payload, can't get productKey.");
        return ERROR;
    }
    if(0 != strcmp(productKey->valuestring, devShadow->productKey))
    {
        LOG_DEBUG("The productKey does not match.");
        return SUCCESS;
    }
    deviceID = cJSON_GetObjectItem(root, "deviceID");
    if(!deviceID)
    {
        LOG_WARNNING("Failed to parse json payload, can't get deviceID.");
        return ERROR;
    }
    if(0 != strcmp(deviceID->valuestring, devShadow->deviceID))
    {
        LOG_DEBUG("The deviceID does not match.");
        return SUCCESS;
    }
    state = cJSON_GetObjectItem(root, "state");
    if(!state)
    {
        LOG_WARNNING("Failed to parse json payload, can't get state.");
        return ERROR;
    }
    stateDesired = cJSON_GetObjectItem(state, "desired");
    if(!stateDesired)
    {
        LOG_WARNNING("Failed to parse json payload, can't get stateDesired.");
        return ERROR;
    }
    stateDesiredSwitch = cJSON_GetObjectItem(stateDesired, "switch");
    if(!stateDesiredSwitch)
    {
        LOG_WARNNING("Failed to parse json payload, can't get stateDesiredSwitch.");
        return ERROR;
    }

    metadata = cJSON_GetObjectItem(root, "metadata");
    if(!metadata)
    {
        LOG_WARNNING("Failed to parse json payload, can't get metadata.");
        return ERROR;
    }
    metadataDesired = cJSON_GetObjectItem(metadata, "desired");
    if(!metadataDesired)
    {
        LOG_WARNNING("Failed to parse json payload, can't get metadataDesired.");
        return ERROR;
    }
    metadataDesiredSwitch = cJSON_GetObjectItem(metadataDesired, "switch");
    if(!metadataDesiredSwitch)
    {
        LOG_WARNNING("Failed to parse json payload, can't get metadataDesiredSwitch.");
        return ERROR;
    }
    metadataDesiredSwitchTimestamp = cJSON_GetObjectItem(metadataDesiredSwitch, "timestamp");
    if(!metadataDesiredSwitchTimestamp)
    {
        LOG_WARNNING("Failed to parse json payload, can't get metadataDesiredSwitchTimestamp.");
        return ERROR;
    }

    snprintf(devShadow->state->desired->sw, LENGTH_SWITCH, "%s", stateDesiredSwitch->valuestring);
    snprintf(devShadow->state->desired->swTimestamp, LENGTH_TIMESTAMP, "%s", metadataDesiredSwitchTimestamp->valuestring);
    devShadow->version = version->valueint;

    cJSON_Delete(root);
    
    return SUCCESS;
}

int Shadow_Attribute_Init(DEV_SHADOW_STATE_S* state)
{
    state->reported = (DEV_SHADOW_ATTRIBUTE_S*)malloc(sizeof(DEV_SHADOW_ATTRIBUTE_S));
    if(NULL == state->reported)
    {
        LOG_ERROR("Reported attribute malloc failed.");
        return ERROR;
    }
    memset(state->reported, 0, sizeof(DEV_SHADOW_ATTRIBUTE_S));

    state->desired = (DEV_SHADOW_ATTRIBUTE_S*)malloc(sizeof(DEV_SHADOW_ATTRIBUTE_S));
    if(NULL == state->desired)
    {
        LOG_ERROR("Desired attribute malloc failed.");
        return ERROR;
    }
    memset(state->desired, 0, sizeof(DEV_SHADOW_ATTRIBUTE_S));

    state->delta = (DEV_SHADOW_ATTRIBUTE_S*)malloc(sizeof(DEV_SHADOW_ATTRIBUTE_S));
    if(NULL == state->delta)
    {
        LOG_ERROR("Delta attribute malloc failed.");
        return ERROR;
    }
    memset(state->delta, 0, sizeof(DEV_SHADOW_ATTRIBUTE_S));

    return SUCCESS;
}

void SHADOW_Release(DEV_SHADOW_S* shadow)
{
    if(NULL != shadow->state->reported)
    {
        free(shadow->state->reported);
        shadow->state->reported = NULL;
    }

    if(NULL != shadow->state->desired)
    {
        free(shadow->state->desired);
        shadow->state->desired = NULL;
    }

    if(NULL != shadow->state->delta)
    {
        free(shadow->state->delta);
        shadow->state->delta = NULL;
    }

    if(NULL != shadow->state)
    {
        free(shadow->state);
        shadow->state = NULL;
    }

    if(NULL != shadow->tag)
    {
        free(shadow->tag);
        shadow->tag = NULL;
    }

    if(NULL != shadow)
    {
        free(shadow);
        shadow = NULL;
    }

    return;
}

DEV_SHADOW_S* SHADOW_Init()
{
    DEV_SHADOW_S* shadow;
    shadow = (DEV_SHADOW_S*)malloc(sizeof(DEV_SHADOW_S));
    if(NULL == shadow)
    {
        LOG_ERROR("Shadow malloc failed.");
        return NULL;
    }
    memset(shadow, 0, sizeof(DEV_SHADOW_S));

    shadow->state = (DEV_SHADOW_STATE_S*)malloc(sizeof(DEV_SHADOW_STATE_S));
    if(NULL == shadow->state)
    {
        LOG_ERROR("Shadow state malloc failed.");
        return NULL;
    }
    memset(shadow->state, 0, sizeof(DEV_SHADOW_STATE_S));

    shadow->tag = (DEV_SHADOW_TAG_S*)malloc(sizeof(DEV_SHADOW_TAG_S));
    if(NULL == shadow->tag)
    {
        LOG_ERROR("Shadow flag malloc failed.");
        return NULL;
    }
    memset(shadow->tag, 0, sizeof(DEV_SHADOW_TAG_S));

    if (SUCCESS != Shadow_Attribute_Init(shadow->state))
    {
        LOG_WARNNING("Shadow initialized failed.");
        return NULL;
    }

    LOG_DEBUG("Shadow has been initialized successfully.");
    return shadow;
}

#ifdef __cplusplus
}
#endif