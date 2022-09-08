#ifndef IOTHUBSCHEMA_H
#define IOTHUBSCHEMA_H

// TODO 根据物模型生成结构体 这部分工作可以交给前端来做(`>`) 实际上前端来做更简单

// 属性列表,需要自动生成
// #AUTO-GEN-BEGIN
typedef struct iothub_property
{
    int a;
    int b;
    //...
} iothub_property;
// 动作出参,需要自动生成
typedef struct iothub_action_out
{
    int action1;
    int action2;
    //...
} iothub_action_out;
// ##AUTO-GEN-END
//
// 回复类消息
//
typedef struct iothub_reply_msg
{
    char *method;
    long id;
    int code;
    long timestamp;
    const char *status;
} iothub_reply_msg;
// 动作回复
typedef struct iothub_action_reply_msg
{
    iothub_reply_msg reply_msg; // 结构体组合
    //-------------------------------------------
    char *actionid;        // 动作的ID
    iothub_action_out out; // 动作的出参
    //-------------------------------------------
} iothub_action_reply_msg;
//
// 属性
//
typedef struct iothub_property_up_msg
{
    char *method;
    long long id;
    long long timestamp;
    iothub_property p;
} iothub_property_up_msg;

// 上报属性数据JSON生成, 具体函数实现由生成器自动生成
char *SDKBuildPropertyMsg(iothub_property msg);
// 构建上行回复消息
char *SDKBuildReplyMsg(iothub_reply_msg msg);
#endif
