/**************************************************************************************
*     Copyright (c) 2004-2019 New H3C Technologies Co., Ltd. All rights reserved.     *
**************************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "MQTTAsync.h"
#include "oasis_led.h"
#include "oasis_spi.h"
#include "oasis_shadow.h"
#include "oasis_log.h"
#include "oasis_tools.h"
#include "cJSON.h"

#define LED_OFF   1
#define LED_ON    0
#define LED_REG         0x34
#define LED_STATE_MASK  1

DEV_SHADOW_S *ledShadow = NULL;

int Led_Get_State(uint8_t *state)
{
    int ret = SUCCESS;
    uint8_t value = 0;
    
    ret = SPI_Read(LED_REG, &value);
    if (SUCCESS != ret)
    {
        LOG_WARNNING("failed to read spi register.");
        return ERROR;
    }

    *state = value & LED_STATE_MASK;

    // if (LED_ON == *state)
    // {
    //     LOG_DEBUG("Led state is on.");
    // }
    // else
    // {
    //     LOG_DEBUG("Led state is off.");
    // }

    return SUCCESS;
}

int Led_Update_State(uint8_t state)
{
    uint8_t value = 0;
    uint8_t setValue = 0;
    int ret = SUCCESS;

    switch (state)
    {
        case LED_ON:
            setValue = LED_ON;
            break;
        case LED_OFF:
            setValue = LED_OFF;
            break;
        default:
            LOG_WARNNING("Cann't update led state, parameter error.");
            return ERROR;
    }

    ret = SPI_Read(LED_REG, &value);
    if (SUCCESS != ret)
    {
        LOG_WARNNING("Cann't update led state.");
        return ERROR;
    }

    value &= ~setValue;
    value |= setValue;

    ret = SPI_Write(LED_REG, value);
    if (SUCCESS != ret)
    {
        LOG_WARNNING("Cann't update led state.");
        return ERROR;
    }

    //LOG_DEBUG("Updateing led state successfully.");
    
    return SUCCESS;
}

void Led_Trans_State(char *status, uint8_t *state)
{
    if(0 == strcmp("on",status))
    {
        *state = LED_ON;
    }
    else if(0 == strcmp("off",status))
    {
        *state = LED_OFF;
    }
    return;
}

int LED_Online_Req(void *context)
{    
    return SHADOW_Connect_Req(context, TOPIC_REQ_ONLINE, ledShadow);
}

int LED_Online_Rsp(char *payload)
{
    return SHADOW_Connect_Rsp(payload, TOPIC_RSP_ONLINE, ledShadow);
}

int LED_Offline_Req(void *context)
{
    return SHADOW_Connect_Req(context, TOPIC_REQ_OFFLINE, ledShadow);
}

int LED_Offline_Rsp(char *payload)
{
    return SHADOW_Connect_Rsp(payload, TOPIC_RSP_OFFLINE, ledShadow);
}

int Led_Set_Shadow_Req(void *context, DEV_SHADOW_S *ledShadow)
{
    return SHADOW_Set_Req(context, ledShadow);
}

int LED_Set_Shadow_Rsp(char *payload)
{
    return SHADOW_Set_Rsp(payload, ledShadow);
}

int LED_Update_Shadow(char *payload)
{
    int ret = SUCCESS;
    uint8_t state = 0;
    ret =  SHADOW_Update(payload, ledShadow);
    if(SUCCESS != ret)
    {
        return ret;
    }
    if(0 != strcmp(ledShadow->state->desired->sw, ledShadow->state->reported->sw))
    {
        Led_Trans_State(ledShadow->state->desired->sw, &state);
        ret = Led_Update_State(state);
    }
    return ret;
}

int LED_Keepalive_Req(void *context)
{
    return SHADOW_Connect_Req(context, TOPIC_REQ_KEEPALIVE, ledShadow);
}

int LED_Keepalive_Rsp(char *payload)
{
    return SHADOW_Connect_Rsp(payload, TOPIC_RSP_KEEPALIVE, ledShadow);
}

int LED_Keepalive_Status(MQTTAsync handle)
{
    int ret;
    while(1)
    {
        sleep(20);
        if (0 == strcmp(DEV_ONLINE, ledShadow->status) && FLAG_RSP == ledShadow->flag->keepalive)
        {
            if (FLAG_RSP != ledShadow->flag->setShadow)
            {
                ret = Led_Set_Shadow_Req(handle, ledShadow);
            }
            ret = LED_Keepalive_Req(handle);
        }
        else
        {
            ledShadow->flag->keepalive = FLAG_RSP;
            snprintf(ledShadow->status, LENGTH_STATUS, "%s", "offline");
            ret = LED_Online_Req(handle);
        }
    }
    return ret;
}

int LED_Monitor_State(MQTTAsync handle)
{
    uint8_t ledState = LED_OFF;
    char *state = NULL;
    char timestamp[LOG_LENGTH_TIMESTAMP] = {0}; 
    char uuid[LOG_LENGTH_UUID] = {0};
    int ret = SUCCESS;

    while (1)
    {
        usleep(100000L);
        //ret = Led_Get_State(&ledState);
        state = (ledState == LED_OFF)?"off":"on";
        if (SUCCESS != ret)
        {
            continue;
            LOG_WARNNING("Failed to get LED state!");
        }

        if (0 == strcmp(DEV_ONLINE, ledShadow->status) &&
            0 == strcmp(state, ledShadow->state->reported->sw) &&
            FLAG_RSP == ledShadow->flag->keepalive)
        {
            ret = TOOLS_Get_Time_ISO8601(timestamp);
            ret = TOOLS_Get_UUID(uuid);
            snprintf(ledShadow->state->reported->sw, LENGTH_SWITCH, "%s", state);
            snprintf(ledShadow->state->reported->swTimestamp, LENGTH_TIMESTAMP, "%s", timestamp); 
            snprintf(ledShadow->token, LENGTH_TOKEN, "%s", uuid);
            ledShadow->version++;
            
            if (0 != strcmp(ledShadow->state->reported->sw, ledShadow->state->desired->sw))
            {
                snprintf(ledShadow->state->delta->sw, LENGTH_SWITCH, "%s", state);
                snprintf(ledShadow->state->delta->swTimestamp, LENGTH_TIMESTAMP, "%s", timestamp); 
            }
            else
            {
                memset(ledShadow->state->delta->sw, 0, LENGTH_SWITCH * sizeof(char));
                memset(ledShadow->state->delta->swTimestamp, 0, LENGTH_TIMESTAMP * sizeof(char));
            }
           
            ret = Led_Set_Shadow_Req(handle, ledShadow);
        }
    }
    return ret;
}

int LED_Send_Data(MQTTAsync handle)
{
    MQTTAsync c = (MQTTAsync)handle;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    int ret;
    char *out;
    char *topic = "app/request/oasis/callFunction";
    int p[2] = {1,2};
    char uuid[LOG_LENGTH_UUID] = {0};
    char timestamp[30] = {0};

    cJSON *root = NULL;
    cJSON *param = NULL;

    ret = TOOLS_Get_Time_ISO8601(timestamp);
    ret = TOOLS_Get_UUID(uuid);

    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "timestamp", timestamp);
    cJSON_AddStringToObject(root, "token", uuid);
    cJSON_AddStringToObject(root, "name", "97");
    cJSON_AddStringToObject(root, "service", "python3");

    cJSON_AddItemToObject(root, "param", param = cJSON_CreateIntArray(p,2));

    out = cJSON_PrintUnformatted(root);

    while(1)
    {
        sleep(20);
        if (0 == strcmp(DEV_ONLINE, ledShadow->status) && FLAG_RSP == ledShadow->flag->keepalive)
        {
            pubmsg.payload = out;
            pubmsg.payloadlen = strlen(out);
            pubmsg.qos = 0;
            pubmsg.retained = 0;

            ret = MQTTAsync_send(c, topic, pubmsg.payloadlen, pubmsg.payload, pubmsg.qos, pubmsg.retained, NULL);
            if (ret != MQTTASYNC_SUCCESS)
            {
                LOG_WARNNING("Failed to send mqtt message.");
                MQTTAsync_destroy(&c);
                return ERROR;
            }

            LOG_DEBUG("publish topic: %s, payload:%s", topic, out);
        }
    }
    return ret;
}

int Led_Init()
{
    int ret = SUCCESS;
    uint8_t ledState;
    char *state = NULL;
    char timestamp[LOG_LENGTH_TIMESTAMP] = {0}; 
    char uuid[LOG_LENGTH_UUID] = {0};

    ledShadow->version = 0;
    ledShadow->flag->online = FLAG_RSP;
    ledShadow->flag->offline = FLAG_RSP;
    ledShadow->flag->keepalive = FLAG_RSP;
    ledShadow->flag->setShadow = FLAG_RSP;
    ret = TOOLS_Get_UUID(uuid); 
    ret = TOOLS_Get_Time_ISO8601(timestamp);
    // ret = Led_Get_State(&ledState);
    // if(SUCCESS != ret)
    // {
    //     LOG_ERROR("Can't get led state.");
    //     return ERROR;
    // }
    state = (ledState == LED_OFF)?"off":"on";
    snprintf(ledShadow->productKey, LENGTH_PRODUCTKEY, "%s", PRODUCTKEY);
    snprintf(ledShadow->deviceID, LENGTH_DEVICEID, "%s", DEVICEID);
    snprintf(ledShadow->state->reported->sw, LENGTH_SWITCH, "%s", state);
    snprintf(ledShadow->token, LENGTH_TOKEN, "%s", uuid);
    snprintf(ledShadow->state->reported->swTimestamp, LENGTH_TIMESTAMP, "%s", timestamp); 
    
    return SUCCESS;
}

int LED_Install()
{
    int ret = SUCCESS;

    ledShadow = SHADOW_Init();

    if(NULL == ledShadow)
    {
        LED_Uninstall();
        LOG_ERROR("Failed to install Led device.");
        return ERROR;
    }

    if(SUCCESS != Led_Init())
    {
        LED_Uninstall();
        LOG_ERROR("Failed to initialize Led device.");
        return ERROR;
    }

    return ret;
}

void LED_Uninstall()
{
    SHADOW_Destroy(ledShadow);

    LOG_DEBUG("Led has been uninstalled.");

    return; 
}

#ifdef __cplusplus
}
#endif