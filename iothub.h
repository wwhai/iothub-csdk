#ifndef IOTHUB_H
#define IOTHUB_H

#include "MQTTClient.h"
#define PRIVATEFUNC static
#include "log.h"
#include "iothubschema.h"
// 回调
//

// SDK 本体
struct iothubsdk
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts;
    int connected;
    iothub_property *property;
    // 消息响应
    void (*OnMessage)(struct iothubsdk *sdk, char *message);
    // 断开连接
    void (*OnClosed)(struct iothubsdk *sdk, char *reason);
    // 消息发送成功
    void (*OnDeliver)(struct iothubsdk *sdk, int token);
} iothubsdk;

// 构造
struct iothubsdk *SDKNewMqttDevice();
// 设置回调
int SDKSetCallback(struct iothubsdk *sdk,
                   void (*OnMessage)(struct iothubsdk *sdk, char *message),
                   void (*OnClosed)(struct iothubsdk *sdk, char *reason),
                   void (*OnDeliver)(struct iothubsdk *sdk, int token));
// 启动
int SDKStart(struct iothubsdk *sdk);
// 设置属性
int SDKSetProperty(struct iothubsdk *sdk, iothub_property *p);
// 属性上报
int SDKPropertyUp(struct iothubsdk *sdk);
// 属性回复
int SDKPropertyReply(struct iothubsdk *sdk, iothub_reply_msg msg);
// 停止
int SDKStop(struct iothubsdk *sdk);
#endif