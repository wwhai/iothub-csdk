#ifndef IOTHUBSCHEMA_H
#define IOTHUBSCHEMA_H

// TODO 根据物模型生成结构体 这部分工作可以交给前端来做(`>`) 实际上前端来做更简单
// 属性
// #AUTO-GEN-BEGIN
typedef struct iothub_property
{
    // 属性列表
    int a;
    int b;
} iothub_property;
// ##AUTO-GEN-END
//
// 回复类消息
//
typedef struct iothub_reply_msg
{
    char *method;
    long long id;
    int code;
    long long timestamp;
    const char *status;
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

// 上报属性数据JSON生成, 具体函数实现由生成器自动生成
void build_iothub_property_msg(iothub_property_msg msg);

#endif
