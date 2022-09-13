#include "iothub_action.h"
#include "utils.h"
#include "log.h"
// 指令下发后设备端回复消息
char *SDKBuildActionReplyMsg(iothub_reply_msg msg)
{
    // 构建外层JSON
    cJSON *root = cJSON_CreateObject();
    cJSON *method = cJSON_CreateString("action_reply"); // 固定值
    cJSON *id = cJSON_CreateString(msg.id);
    cJSON *actionid = cJSON_CreateString(msg.actionid);
    cJSON *code = cJSON_CreateNumber(msg.code);
    cJSON *timestamp = cJSON_CreateNumber(getMicrotime()); // 时间戳
    cJSON *status = cJSON_CreateString(msg.status);
    cJSON_AddItemToObject(root, "method", method);
    cJSON_AddItemToObject(root, "id", id);
    cJSON_AddItemToObject(root, "actionid", actionid);
    cJSON_AddItemToObject(root, "timestamp", timestamp);
    cJSON_AddItemToObject(root, "code", code);
    cJSON_AddItemToObject(root, "status", status);
    SDKBuildActionOut(root, msg.out);
    char *jsons = cJSON_PrintUnformatted(root);
    log_debug("SDKBuildPropertyReplyMsg: %s", jsons);
    return jsons;
}
// 生成Action的 data 字段, 然后填充到root结构体里面, 该函数自动生成
int SDKBuildActionOut(cJSON *root, iothub_action_param p)
{
    cJSON *pData = cJSON_CreateObject();
    {
        //------------------------------------------------------------------------------------------
        // #AUTO-GEN-BEGIN
        //------------------------------------------------------------------------------------------
        cJSON *action_p1 = cJSON_CreateNumber(p.action_p1);
        cJSON_AddItemToObject(pData, "action_p1", action_p1);
        cJSON *action_p2 = cJSON_CreateNumber(p.action_p2);
        cJSON_AddItemToObject(pData, "action_p2", action_p2);
        cJSON *values = cJSON_CreateString(cJSON_PrintUnformatted(pData));
        cJSON_AddItemToObject(root, "out", values);
        //------------------------------------------------------------------------------------------
        // #AUTO-GEN-END
        //------------------------------------------------------------------------------------------
    }
    return 0;
}
