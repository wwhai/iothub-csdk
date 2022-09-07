#ifndef IOTHUBSCHEMA_H
#define IOTHUBSCHEMA_H

// TODO 根据物模型生成结构体 这部分工作可以交给前端来做(`>`) 实际上前端来做更简单
// 属性
// #BEGIN
typedef struct iothub_property
{
} iothub_property;

//
// 回复类消息
//
typedef struct iothub_reply_msg
{
    char *method;
    char *actionid; // 仅对Action有效
    long long id;
    int code;
    long long timestamp;
    char *status;
} iothub_reply_msg;
//
// 属性
//
typedef struct iothub_property_msg
{
    char *method;
    long long id;
    long long timestamp;
    iothub_property p;
} iothub_property_msg;

// #END
void build_iothub_property_msg(iothub_property_msg msg);

#endif
