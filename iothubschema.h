#ifndef IOTHUBSCHEMA_H
#define IOTHUBSCHEMA_H
// iothub 下来的数据类型
typedef enum MsgType
{
    T_ERROR = -1,       // 解析失败
    T_UNRECOGNIZED = 0, // 未识别的消息
    T_PROPERTY_DOWN,    // 属性下发
    T_ACTION_DOWN       // 动作调用
} MsgType;

// 动作出参,需要自动生成
typedef struct iothub_action_param
{
    int action_p1;
    int action_p2;
    //...
} iothub_action_param;
typedef struct iothub_property
{
    int a;
    int b;
    //...
} iothub_property;

//
// 属性
//
typedef struct iothub_down_msg
{
    char *method;                    // 方法名一般不会太长
    char *id;                        // Id
    long timestamp;                  // 时间戳
    char *actionid;                  // 当method==action表示动作的ID
    iothub_property property_data;   // 属性
    iothub_action_param action_data; // 动作
} iothub_down_msg;
//
typedef struct iothub_reply_msg
{
    char *method;
    char *id;
    int code;
    long timestamp;
    const char *status;
    char *actionid;          // 当method==action表示动作的ID
    iothub_action_param out; // 当method==action表示动作的出参
} iothub_reply_msg;
// 获取属性的ID
int SDKGetMsgId();
// 获取属性时间戳
int SDKGetMsgTimestamp();
// 获取属性方法
int SDKGetMsgMethod();
// 获取动作的ID
int SDKGetActionId();
#endif
