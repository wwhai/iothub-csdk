#include "stdio.h"
#include "iothubschema.h"
#include "cJSON.h"
#include "log.h"
#include "utils.h"
char *SDKBuildPropertyMsg(iothub_property msg)
{
    // 构建外层JSON
    long micro = getMicrotime();
    cJSON *root = cJSON_CreateObject();
    cJSON *method = cJSON_CreateString("property");
    cJSON *id = cJSON_CreateNumber(micro);
    cJSON *timestamp = cJSON_CreateNumber(micro);
    // 构建节点
    cJSON_AddItemToObject(root, "method", method);
    cJSON_AddItemToObject(root, "id", id);
    cJSON_AddItemToObject(root, "timestamp", timestamp);
    cJSON *data = cJSON_CreateObject();
    //----------------------------------------------------------------------------------------------
    // Property BEGIN
    //----------------------------------------------------------------------------------------------
    {
        cJSON *a = cJSON_CreateNumber(msg.a);
        cJSON_AddItemToObject(data, "a", a);
        cJSON *b = cJSON_CreateNumber(msg.b);
        cJSON_AddItemToObject(data, "b", b);
        cJSON *values = cJSON_CreateString(cJSON_PrintUnformatted(data));
        cJSON_AddItemToObject(root, "data", values);
    }
    //----------------------------------------------------------------------------------------------
    // Property END
    //----------------------------------------------------------------------------------------------
    char *string = cJSON_PrintUnformatted(root);
    log_debug("SDKBuildPropertyMsg: %s\n", string);
    return string;
}
//
char *SDKBuildReplyMsg(iothub_reply_msg msg)
{
    // 构建外层JSON
    cJSON *root = cJSON_CreateObject();
    cJSON *method = cJSON_CreateString(msg.method);
    cJSON *id = cJSON_CreateNumber(msg.id);
    cJSON *code = cJSON_CreateNumber(msg.code);
    cJSON *timestamp = cJSON_CreateNumber(msg.timestamp);
    cJSON *status = cJSON_CreateString(msg.status);
    cJSON_AddItemToObject(root, "method", method);
    cJSON_AddItemToObject(root, "id", id);
    cJSON_AddItemToObject(root, "timestamp", timestamp);
    cJSON_AddItemToObject(root, "code", code);
    cJSON_AddItemToObject(root, "status", status);
    char *jsons = cJSON_PrintUnformatted(root);
    log_debug("SDKBuildReplyMsg: %s", jsons);
    return jsons;
}