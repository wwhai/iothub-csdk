#ifndef IOTHUB_PROPERTY_H
#define IOTHUB_PROPERTY_H
#include "cJSON.h"
#include "iothubschema.h"

// 上报属性数据JSON生成, 具体函数实现由生成器自动生成
char *SDKBuildPropertyMsg(iothub_property msg);
// 指令下发后设备端回复消息
char *SDKBuildPropertyReplyMsg(iothub_reply_msg msg);
// 生成属性的 data 字段, 然后填充到root结构体里面, 该函数自动生成
int SDKBuildPropertyData(cJSON *root, iothub_property msg);

#endif