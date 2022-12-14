#ifndef IOTHUB_H
#define IOTHUB_H
#define PRIVATE static
#define MUTE_STRING const char * // 不可变静态字符串
#include "MQTTClient.h"
#include "log.h"
#include "iothubschema.h"
#include "iothub_property.h"
#include "iothubconfig.h"
// 回调
//

// SDK 本体
struct iothubsdk
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts;
    iothub_property *property;
    iothub_action_param *action;
    // 公共消息响应, 当发来非iothub格式的数据时走这个函数
    void (*OnMessage)(struct iothubsdk *sdk, char *message); // 需要注意, 该函数不可阻塞,必须每个消息一次返回, 所以建议用多线程处理消息并且及时返回
    // 下发消息
    void (*OnProperty)(struct iothubsdk *sdk, iothub_down_msg msg); // 需要注意, 该函数不可阻塞,必须每个消息一次返回, 所以建议用多线程处理消息并且及时返回
    // 动作调用
    void (*OnAction)(struct iothubsdk *sdk, iothub_down_msg msg); // 需要注意, 该函数不可阻塞,必须每个消息一次返回, 所以建议用多线程处理消息并且及时返回
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
                   void (*OnDeliver)(struct iothubsdk *sdk, int token),
                   void (*OnProperty)(struct iothubsdk *sdk, iothub_down_msg msg),
                   void (*OnAction)(struct iothubsdk *sdk, iothub_down_msg msg));
// 启动
int SDKStart(struct iothubsdk *sdk);
// 设置属性
PRIVATE int SDKSetProperty(struct iothubsdk *sdk, iothub_property *p);
PRIVATE int SDKSetAction(struct iothubsdk *sdk, iothub_action_param *a);
// 属性上报
int SDKPropertyUp(struct iothubsdk *sdk);
// 属性回复
int SDKPropertyReply(struct iothubsdk *sdk, iothub_reply_msg msg);
// 动作回复
int SDKActionReply(struct iothubsdk *sdk, iothub_reply_msg msg);
// 解析把JSON格式的属性解析成结构体
int SDKParseDownMsg(iothub_down_msg *msg, char *payload);
// 解析参数, 分配到具体的结构体上, 该函数非常重要, 主要留给生成器生成具体的代码
// 实际上该函数也是iothub的物模型的核心功能: 代码生成
PRIVATE int SDKParseParams(iothub_down_msg *msg, cJSON *data);
// 停止SDK
int SDKStop(struct iothubsdk *sdk);
#endif