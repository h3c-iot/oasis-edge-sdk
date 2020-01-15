/**************************************************************************************
*     Copyright (client) 2004-2019 New H3C Technologies Co., Ltd. All rights reserved.     *
**************************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include "MQTTAsync.h"
#include "oasis_tools.h"
#include "oasis_log.h"
#include "oasis_led.h"
#include "oasis_shadow.h"

#define WINAPI
#define MQTT_ADDRESS "tcp://localhost:1883"
#define MQTT_CLIENTID "edgeClientSample"
#define MQTT_USERNAME "test"
#define MQTT_PASSWORD "hahaha"
#define MQTT_WILLTOPIC "will topic"
#define MQTT_WILLMSG "will message"
#define MQTT_TIMEOUT 20
#define MQTT_QOS0 0
#define MQTT_QOS1 1

int subscribed = 0;
volatile int finished = 0;

int messageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    //MQTTAsync client = (MQTTAsync)context;
    int ret;
    char *payloadptr;

    payloadptr = message->payload;

    LOG_DEBUG("Mqtt message arrived, topic: %s, message: %s", topicName, payloadptr);

    if (0 == strcmp(TOPIC_RSP_ONLINE,topicName))
    {
        ret = LED_Online_Rsp(payloadptr);
    }
    else if (0 == strcmp(TOPIC_RSP_OFFLINE, topicName))
    {
        ret = LED_Offline_Rsp(payloadptr);
    }
    else if (0 == strcmp(TOPIC_UPDATE_SHADOW, topicName))
    {
        ret = LED_Update_Shadow(payloadptr);
    }
    else if (0 == strcmp(TOPIC_RSP_SET_SHADOW, topicName))
    {
        ret = LED_Set_Shadow_Rsp(payloadptr);
    }
    else if (0 == strcmp(TOPIC_RSP_KEEPALIVE, topicName))
    {
        ret = LED_Keepalive_Rsp(payloadptr);
    }
    (void)ret;

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    return 1;
}

void onSubscribe(void *context, MQTTAsync_successData *response)
 {
    LOG_DEBUG("Subscribe succeeded");
    subscribed = 1;
 }

 void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    LOG_DEBUG("Subscribe failed, rc %d", response ? response->code : 0);
    finished = 1;
}

void onConnect(void *context, MQTTAsync_successData *response)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;
    char *topics[TOPIC_SUB_CNT] = {TOPIC_RSP_ONLINE,
                                   TOPIC_RSP_OFFLINE,
                                   TOPIC_RSP_SET_SHADOW,
                                   TOPIC_RSP_KEEPALIVE,
                                   TOPIC_UPDATE_SHADOW};
    int qoss[TOPIC_SUB_CNT] = {TOPIC_RSP_ONLINE_QOS,
                               TOPIC_RSP_OFFLINE_QOS,
                               TOPIC_RSP_SET_SHADOW_QOS,
                               TOPIC_RSP_KEEPALIVE_QOS,
                               TOPIC_UPDATE_SHADOW_QOS};
    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;
    
    rc = MQTTAsync_subscribeMany(client, TOPIC_SUB_CNT, topics, qoss, &opts);
    
    if (rc != MQTTASYNC_SUCCESS)
    {
        LOG_WARNNING("Failed to start subscribe, return code %d", rc);
    }
}

void onConnectFailure(void *context, MQTTAsync_failureData *response)
{
    LOG_WARNNING("Connect failed, rc %d", response ? response->code : 0);
    finished = 1;
}

int main()
{
    MQTTAsync client;
    MQTTAsync_connectOptions opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_willOptions wopts = MQTTAsync_willOptions_initializer;
    int ret = SUCCESS;

    pthread_t led_monitor_thread;
    pthread_t led_keepalive_thread;
    pthread_t led_sendData_thread;

    int led_monitor_thread_ret;
    int led_keepalive_thread_ret;
    int led_sendData_thread_ret;

    LOG_DEBUG("Mqtt client is connecting.");

    ret = MQTTAsync_create(&client, MQTT_ADDRESS, MQTT_CLIENTID, MQTTCLIENT_PERSISTENCE_DEFAULT, NULL);
    if (ret != MQTTASYNC_SUCCESS)
    {
        LOG_ERROR("Failed to creat async mqtt client, ret = %d.", ret);
        goto exit;
    }

    ret = MQTTAsync_setCallbacks(client, client, NULL, messageArrived, NULL);
    if (ret != MQTTASYNC_SUCCESS)
    {
        LOG_ERROR("Failed to set callbacks, ret = %d.", ret);
        goto exit;
    }

    opts.keepAliveInterval = MQTT_TIMEOUT;
    opts.cleansession = 1;
    opts.username = MQTT_USERNAME;
    opts.password = MQTT_PASSWORD;
    opts.MQTTVersion = MQTTVERSION_DEFAULT;

    opts.will = &wopts;
    opts.will->message = MQTT_WILLMSG;
    opts.will->qos = MQTT_QOS0;
    opts.will->retained = 0;
    opts.will->topicName = MQTT_WILLTOPIC;
    opts.will = NULL;
    opts.onSuccess = onConnect;
    opts.onFailure = onConnectFailure;
    opts.context = client;

    ret = MQTTAsync_connect(client, &opts);
    if (ret != MQTTASYNC_SUCCESS)
    {
        LOG_ERROR("Failed to connect, ret = %d.", ret);
        goto exit;
    }

    while (!subscribed)
        usleep(10000L);

    ret = LED_Install();
    if (SUCCESS != ret)
    {
        LOG_ERROR("Failed to install led device.");
        goto exit;
    }
    
    ret = LED_Online_Req(client);

    if (SUCCESS != ret)
    {
        LOG_ERROR("Led device can't register.");
    }

    led_monitor_thread_ret = pthread_create(&led_monitor_thread, NULL, (void *)&LED_Monitor_State, (void *)client);

    led_keepalive_thread_ret = pthread_create(&led_keepalive_thread, NULL, (void *)&LED_Keepalive_Status, (void *)client);

    led_sendData_thread_ret = pthread_create(&led_sendData_thread, NULL, (void *)&LED_Send_Data, (void *)client);

    if (SUCCESS != led_monitor_thread_ret)
    {
        LOG_WARNNING("Failed to create led monitor thread.");
    }
    else
    {
        LOG_DEBUG("Led monitor thread has been created.");
    }

    if (SUCCESS != led_keepalive_thread_ret)
    {
        LOG_WARNNING("Failed to create led keepalive thread.");
    }
    else
    {
        LOG_DEBUG("Led keepalive thread has been created.");
    }

    while (!finished)
        usleep(10000L);
    
exit:
    LED_Uninstall();
    MQTTAsync_destroy(&client);
    return ERROR;
}

#ifdef __cplusplus
}
#endif