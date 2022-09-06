#if !defined(IOTHUB_H)
#define IOTHUB_H

#include "MQTTClient.h"
typedef struct
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts;

} iothubsdk;

typedef struct
{
    (int *)OnMessage(char *);
    (int *)OnClosed(char *);
    (int *)OnDelived(int);
} iothubsdk_callback;
iothubsdk *NewMqttDevice(char *deviceid, char *username, char *password);
int SetCallback(iothubsdk *sdk, iothubsdk_callback *callback);
int Start(iothubsdk *sdk);
int Stop(iothubsdk *sdk);
#endif