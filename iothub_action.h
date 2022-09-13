#ifndef IOTHUB_ACTION_H
#define IOTHUB_ACTION_H
#include "cJSON.h"
#include "iothubschema.h"

// 指令下发后设备端回复消息
char *SDKBuildActionReplyMsg(iothub_reply_msg msg);
// 生成Action的 out 字段, 然后填充到root结构体里面, 该函数自动生成
int SDKBuildActionOut(cJSON *root, iothub_action_param p);
#endif