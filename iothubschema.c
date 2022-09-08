#include "stdio.h"
#include "iothubschema.h"
#include <stdlib.h>
#include "string.h"
#include "cJSON.h"
#include "log.h"
#include "utils.h"
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
    log_debug("SDKBuildPropertyMsg: %s", string);
    return string;
}
/// @brief
/// @param msg
/// @return
char *SDKBuildReplyMsg(iothub_reply_msg msg)
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
    log_debug("SDKBuildReplyMsg: %s", jsons);
    return jsons;
}
/// @brief
/// @param msg
/// @param payload
/// @return
int SDKParsePropertyMsg(iothub_property_msg *msg, char *payload)
{
    int status = 0;
    // {"method":"property","id":1,"timestamp":1,"data":"{\"a\":1,\"b\":2}"}
    cJSON *payload_json = cJSON_Parse(payload);
    if (payload_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            log_error("stderr:%s, json parse error: %s\n", stderr, error_ptr);
        }
        return -1;
    }
    cJSON *method = cJSON_GetObjectItemCaseSensitive(payload_json, "method");
    if (!cJSON_IsString(method))
    {
        status = -1;
        log_error("'method' must be json string type");
        goto end;
    }
    cJSON *id = cJSON_GetObjectItemCaseSensitive(payload_json, "id");
    if (!cJSON_IsString(id))
    {
        status = -1;
        log_error("'id' must be json number type");
        goto end;
    }
    cJSON *timestamp = cJSON_GetObjectItemCaseSensitive(payload_json, "timestamp");
    if (!cJSON_IsNumber(timestamp))
    {
        status = -1;
        log_error("'timestamp' must be json number type");
        goto end;
    }
    cJSON *data = cJSON_GetObjectItemCaseSensitive(payload_json, "data");
    if (!cJSON_IsObject(data))
    {
        status = -1;
        log_error("'data' must be json object type");
        goto end;
    }
    printf("1\n");
    msg->method = (char *)malloc(sizeof(strlen(method->valuestring)));
    printf("2\n");
    msg->id = (char *)malloc(sizeof(strlen(id->valuestring)));
    strcpy(msg->method, method->valuestring);
    strcpy(msg->id, id->valuestring);
    msg->timestamp = timestamp->valueint;
    //
    // TODO msg->p= 这里的P是动态的, 也需要生成, 感觉问题变复杂了。。。
    // #AUTO-GEN-BEGIN
    {
        cJSON *a = cJSON_GetObjectItemCaseSensitive(data, "a");
        if (!cJSON_IsNumber(a))
        {
            status = -1;
            log_error("'a' must be json number");
            goto end;
        }
        msg->p.a = a->valueint;
        cJSON *b = cJSON_GetObjectItemCaseSensitive(data, "b");
        if (!cJSON_IsNumber(b))
        {
            status = -1;
            log_error("'b' must be json number");
            goto end;
        }
        msg->p.b = b->valueint;
    }
    // #AUTO-GEN-BEGIN
end:
    cJSON_Delete(payload_json);
    return status;
}
/// @brief
/// @param msg
/// @param payload
/// @return
int SDKParseActionMsg(iothub_action_msg *msg, char *payload)
{
    return 0;
}
/// @brief
/// @param msg
/// @return
char *SDKBuildActionReplyMsg(iothub_action_reply_msg msg)
{
    //{
    //     "method":"action_reply",
    //     "id":"1",
    //     "actionId": "get_state",
    //     "code": 0,
    //     "status":"message"
    //     "out": {
    //              "a":1,
    //              "b":1
    //      }
    //{
    cJSON *root = cJSON_CreateObject();
    cJSON *method = cJSON_CreateString(msg.reply_msg.method);
    cJSON *id = cJSON_CreateString(msg.reply_msg.id);
    cJSON *code = cJSON_CreateNumber(msg.reply_msg.code);
    cJSON *timestamp = cJSON_CreateNumber(msg.reply_msg.timestamp);
    cJSON *status = cJSON_CreateString(msg.reply_msg.status);
    cJSON *actionid = cJSON_CreateString(msg.actionid);
    //
    cJSON_AddItemToObject(root, "method", method);
    cJSON_AddItemToObject(root, "id", id);
    cJSON_AddItemToObject(root, "actionid", actionid);
    cJSON_AddItemToObject(root, "timestamp", timestamp);
    cJSON_AddItemToObject(root, "code", code);
    cJSON_AddItemToObject(root, "status", status);
    // TODO: 自动生成器
    //----------------------------------------------------------------------------------------------
    // Property BEGIN
    //----------------------------------------------------------------------------------------------
    {
        cJSON *out = cJSON_CreateObject();
        // action_p1
        cJSON *action_p1 = cJSON_CreateNumber(msg.out.action_p1);
        cJSON_AddItemToObject(out, "action_p1", action_p1);
        // action_p2
        cJSON *action_p2 = cJSON_CreateNumber(msg.out.action_p2);
        cJSON_AddItemToObject(out, "action_p2", action_p2);
        //
        cJSON_AddItemToObject(root, "out", out);
    }
    //----------------------------------------------------------------------------------------------
    // Property END
    //----------------------------------------------------------------------------------------------
    char *jsons = cJSON_PrintUnformatted(root);
    log_debug("SDKBuildActionReplyMsg: %s", jsons);
    return jsons;
}