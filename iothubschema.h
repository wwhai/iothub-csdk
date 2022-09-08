#ifndef IOTHUBSCHEMA_H
#define IOTHUBSCHEMA_H

// TODO 根据物模型生成结构体
// 属性列表,需要自动生成
// #AUTO-GEN-BEGIN
typedef struct iothub_property
{
    int a;
    int b;
    //...
} iothub_property;
// 动作出参,需要自动生成
typedef struct iothub_action_param
{
    int action_p1;
    int action_p2;
    //...
} iothub_action_param;
// ##AUTO-GEN-END
//
// 回复类消息
//
typedef struct iothub_reply_msg
{
    char *method;
    char *id;
    int code;
    long timestamp;
    const char *status;
} iothub_reply_msg;
// 动作回复
typedef struct iothub_action_reply_msg
{
    iothub_reply_msg reply_msg; // 结构体组合
    //-------------------------------------------
    char *actionid;          // 动作的ID
    iothub_action_param out; // 动作的出参
    //-------------------------------------------
} iothub_action_reply_msg;
//
// 属性
//
typedef struct iothub_property_msg
{
    char *method;      // 方法名一般不会太长
    char *id;          // Id
    long timestamp;    // 时间戳
    iothub_property p; // 属性
} iothub_property_msg;
//
typedef struct iothub_action_msg
{
    char *method;             // 方法名一般不会太长
    char *id;                 // Id
    long timestamp;           // 时间戳
    iothub_action_param data; // 属性
} iothub_action_msg;
// 上报属性数据JSON生成, 具体函数实现由生成器自动生成
char *SDKBuildPropertyMsg(iothub_property msg);
// 构建上行回复消息
char *SDKBuildReplyMsg(iothub_reply_msg msg);
// 解析属性消息
int SDKParsePropertyMsg(iothub_property_msg *msg, char *payload);
// 解析Action
int SDKParseActionMsg(iothub_action_msg *msg, char *payload);
// 构建Action
char *SDKBuildActionReplyMsg(iothub_action_reply_msg msg);
//
#endif
