/**************************************************************************************
*     Copyright (c) 2004-2019 New H3C Technologies Co., Ltd. All rights reserved.     *
**************************************************************************************/

#ifndef _OASIS_LED_H_
#define _OASIS_LED_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define PRODUCTKEY "PhGITmeS"
#define DEVICEID "986753"

int LED_Install();
void LED_Uninstall();
int LED_Online_Req(void *context);
int LED_Online_Rsp(char *payload);
int LED_Offline_Req(void *context);
int LED_Offline_Rsp(char *payload);
int LED_Keepalive_Req(void *context);
int LED_Keepalive_Rsp(char *payload);
int LED_Keepalive_Status(MQTTAsync handle);
int LED_Send_Data(MQTTAsync handle);
int LED_Update_Shadow(char *payload);
int LED_Monitor_State(MQTTAsync handle);
int LED_Set_Shadow_Rsp(char *payload);

#ifdef __cplusplus
}
#endif

#endif
