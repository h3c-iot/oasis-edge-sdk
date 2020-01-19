/**************************************************************************************
*     Copyright (c) 2004-2019 New H3C Technologies Co., Ltd. All rights reserved.     *
**************************************************************************************/

#ifndef _OAISIS_SHADOW_H_
#define _OAISIS_SHADOW_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define DEV_INACTIVE  "inactive"
#define DEV_OFFLINE   "offline"
#define DEV_ONLINE    "online"
#define DEV_DISABLED  "disabled"
#define DEV_NOTFOUND  "notfound"

#define FLAG_REQ  1
#define FLAG_RSP  0

#define TOPIC_REQ_ONLINE         "app/request/oasis/online"
#define TOPIC_REQ_OFFLINE        "app/request/oasis/offline"
#define TOPIC_REQ_SET_SHADOW     "app/request/oasis/updateShadow"
#define TOPIC_REQ_KEEPALIVE      "app/request/oasis/keepalive"

#define TOPIC_SUB_CNT 5
#define TOPIC_RSP_ONLINE            "oasis/response/app/online"
#define TOPIC_RSP_ONLINE_QOS        MQTT_QOS0
#define TOPIC_RSP_OFFLINE           "oasis/response/app/offline"
#define TOPIC_RSP_OFFLINE_QOS       MQTT_QOS0
#define TOPIC_RSP_SET_SHADOW        "oasis/response/app/updateShadow"
#define TOPIC_RSP_SET_SHADOW_QOS    MQTT_QOS0
#define TOPIC_RSP_KEEPALIVE         "oasis/response/app/keepalive"
#define TOPIC_RSP_KEEPALIVE_QOS     MQTT_QOS0
#define TOPIC_UPDATE_SHADOW         "oasis/notify/app/updateShadow"
#define TOPIC_UPDATE_SHADOW_QOS     MQTT_QOS0

#define LENGTH_PRODUCTKEY 20
#define LENGTH_DEVICEID   20
#define LENGTH_SWITCH     10
#define LENGTH_TOKEN      37
#define LENGTH_TIMESTAMP  30
#define LENGTH_STATUS     11

typedef struct tagDEV_SHADOW_ATTRIBUTE
{
    char sw[LENGTH_SWITCH];
    unsigned long swTimestamp;
} DEV_SHADOW_ATTRIBUTE_S;

typedef struct tagDEV_SHADOW_STATE
{
    DEV_SHADOW_ATTRIBUTE_S *desired;
    DEV_SHADOW_ATTRIBUTE_S *reported;
    DEV_SHADOW_ATTRIBUTE_S *delta;
} DEV_SHADOW_STATE_S;

typedef struct tagDEV_SHADOW_TAG
{
    int online;
    int offline;
    int keepalive;
    int setShadow;
} DEV_SHADOW_FLAG_S;

typedef struct tagDEV_SHADOW
{
    char productKey[LENGTH_PRODUCTKEY];
    char deviceID[LENGTH_DEVICEID];
    char timestamp[LENGTH_TIMESTAMP];
    char token[LENGTH_TOKEN];
    char status[LENGTH_STATUS];
    long version;
    DEV_SHADOW_FLAG_S *flag;
    DEV_SHADOW_STATE_S *state;
} DEV_SHADOW_S;

DEV_SHADOW_S* SHADOW_Init();
void SHADOW_Destroy(DEV_SHADOW_S* shadow);
int SHADOW_Connect_Req(void *context, char *topic, DEV_SHADOW_S *shadow);
int SHADOW_Connect_Rsp(char *payload, char *topic, DEV_SHADOW_S *shadow);
int SHADOW_Set_Req(void *context, DEV_SHADOW_S *shadow);
int SHADOW_Set_Rsp(char *payload, DEV_SHADOW_S *shadow);
int SHADOW_Update(char *payload, DEV_SHADOW_S *shadow);

#ifdef __cplusplus
}
#endif


#endif /* _SHADOW_H_ */
