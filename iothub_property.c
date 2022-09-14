#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "iothubschema.h"
#include "iothub.h"
#include "utils.h"
#include "iothub_property.h"

/// @brief
/// @param msg
/// @return
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
    SDKBuildPropertyData(root, msg);
    char *string = cJSON_PrintUnformatted(root);
    log_debug("SDKBuildPropertyMsg: %s", string);
    return string;
}
/// @brief
/// @param msg
/// @return
char *SDKBuildPropertyReplyMsg(iothub_reply_msg msg)
{
    // 构建外层JSON
    cJSON *root = cJSON_CreateObject();
    cJSON *method = cJSON_CreateString(msg.method);
    cJSON *id = cJSON_CreateString(msg.id);
    cJSON *code = cJSON_CreateNumber(msg.code);
    cJSON *timestamp = cJSON_CreateNumber(msg.timestamp);
    cJSON *status = cJSON_CreateString(msg.status);
    cJSON_AddItemToObject(root, "method", method);
    cJSON_AddItemToObject(root, "id", id);
    cJSON_AddItemToObject(root, "timestamp", timestamp);
    cJSON_AddItemToObject(root, "code", code);
    cJSON_AddItemToObject(root, "status", status);
    char *jsons = cJSON_PrintUnformatted(root);
    log_debug("SDKBuildPropertyReplyMsg: %s", jsons);
    return jsons;
}

/// @brief
/// @param msg
/// @return
int SDKBuildPropertyData(cJSON *root, iothub_property msg)
{
    cJSON *pData = cJSON_CreateObject();
    {
        //------------------------------------------------------------------------------------------
        // #AUTO-GEN-BEGIN
        //------------------------------------------------------------------------------------------
        cJSON *a = cJSON_CreateNumber(msg.a);
        cJSON_AddItemToObject(pData, "a", a);
        cJSON *b = cJSON_CreateNumber(msg.b);
        cJSON_AddItemToObject(pData, "b", b);
        cJSON *ar = cJSON_CreateArray();
        cJSON_AddItemToArray(ar, msg.ar[0]);
        cJSON_AddItemToArray(ar, msg.ar[1]);
        cJSON *values = cJSON_CreateString(cJSON_PrintUnformatted(pData));
        cJSON_AddItemToObject(root, "data", values);
        //------------------------------------------------------------------------------------------
        // #AUTO-GEN-END
        //------------------------------------------------------------------------------------------
    }
}
