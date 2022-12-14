#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iothub.h"
#include "iothubconfig.h"
#include "iothubschema.h"
#include "cJSON.h"
#include "iothub_property.h"
#include "iothub_action.h"
#include "utils.h"
#include "log.h"
// 属性上报
extern MUTE_STRING PROPERTY_UP;
extern MUTE_STRING PROPERTY_DOWN;
extern MUTE_STRING PROPERTY_REPLY;
extern MUTE_STRING ACTION_DOWN;
extern MUTE_STRING ACTION_REPLY;
//
PRIVATE void delivered(void *context, MQTTClient_deliveryToken dt)
{

    log_debug("message delivered with token:%d", dt);
    struct iothubsdk *sdk = (struct iothubsdk *)context;
    sdk->OnDeliver(sdk, (int)dt);
}
//
// 这里需要解析消息来判断类型
// 如果是属性, 就调用属性的回调: onProperty(propertyJson)
// 如果是动作, 就调用动作的回调: onAction(actionJson)
//
PRIVATE int msg_arrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    log_debug("message arrived, topic:%s, message id: %ld, payload: %s", topicName, message->msgid, message->payload);
    struct iothubsdk *sdk = (struct iothubsdk *)context;

    iothub_down_msg downMsg;
    int rc = SDKParseDownMsg(&downMsg, message->payload);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        log_error("failed to SDKParseDownMsg, return code %d", rc);
        return 1;
    }
    if (strcmp(downMsg.method, "property") == 0)
    {
        sdk->OnProperty(sdk, downMsg);
        return 1;
    }
    if (strcmp(downMsg.method, "action") == 0)
    {
        sdk->OnAction(sdk, downMsg);
        return 1;
    }
    sdk->OnMessage(sdk, message->payload);
    return 1;
}

PRIVATE void conn_lost(void *context, char *cause)
{
    log_warn("connection lost cause:%s", cause);
    struct iothubsdk *sdk = (struct iothubsdk *)context;
    sdk->OnClosed(sdk, cause);
}
//
struct iothubsdk *SDKNewMqttDevice()
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = MQTT_U;
    conn_opts.password = MQTT_P;
    conn_opts.cleansession = 1;  // True
    conn_opts.retryInterval = 5; // 5s
    int rc;
    if ((rc = MQTTClient_create(&client, MQTT_HOST, MQTT_C,
                                MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        log_error("failed to create sdk, return code %d", rc);
        return NULL;
    }
    struct iothubsdk *sdk = (struct iothubsdk *)malloc(sizeof(struct iothubsdk));
    sdk->client = client;
    sdk->conn_opts = conn_opts;
    iothub_property *p = (iothub_property *)malloc(sizeof(iothub_property));
    p->a = 0;
    p->b = 0;
    sdk->property = p;
    log_debug("sdk create successfully");
    return sdk;
}
// 回调
int SDKSetCallback(struct iothubsdk *sdk,
                   void (*OnMessage)(struct iothubsdk *sdk, char *message),
                   void (*OnClosed)(struct iothubsdk *sdk, char *reason),
                   void (*OnDeliver)(struct iothubsdk *sdk, int token),
                   void (*OnProperty)(struct iothubsdk *sdk, iothub_down_msg msg),
                   void (*OnAction)(struct iothubsdk *sdk, iothub_down_msg msg))
{

    // set sdk to context
    int rc = MQTTClient_setCallbacks(sdk->client, sdk, conn_lost, msg_arrived, delivered);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        log_error("failed to setCallbacks, return code %d", rc);
        return rc;
    }
    else
    {
        sdk->OnMessage = OnMessage;
        sdk->OnClosed = OnClosed;
        sdk->OnDeliver = OnDeliver;
        sdk->OnProperty = OnProperty;
        sdk->OnAction = OnAction;
        return MQTTCLIENT_SUCCESS;
    }
}
// 启动
int SDKStart(struct iothubsdk *sdk)
{
    int rc;
    if ((rc = MQTTClient_connect(sdk->client, &sdk->conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        log_error("failed to connect mqtt server: %s, username: %s, clientid: %s, return code %d",
                  MQTT_U, MQTT_C, MQTT_HOST, rc);
        return rc; // -1
    }
    log_info("connect to %s success", MQTT_HOST);
    {
        if ((rc = MQTTClient_subscribe(sdk->client, PROPERTY_DOWN, 1)) != MQTTCLIENT_SUCCESS)
        {
            log_error("failed to subscribe, return code %d", rc);
            return rc; // -1
        }
        log_info("subscribe %s success", PROPERTY_DOWN);
    }
    {
        if ((rc = MQTTClient_subscribe(sdk->client, ACTION_DOWN, 1)) != MQTTCLIENT_SUCCESS)
        {
            log_error("failed to subscribe, return code %d", rc);
            return rc; // -1
        }
        log_info("subscribe %s success", ACTION_DOWN);
    }
    iothub_property *p = (iothub_property *)malloc(sizeof(iothub_property));
    iothub_action_param *a = (iothub_action_param *)malloc(sizeof(iothub_action_param));
    SDKSetProperty(sdk, p);
    SDKSetAction(sdk, a);
    return MQTTCLIENT_SUCCESS;
}

//
// 设置属性
//
int SDKSetProperty(struct iothubsdk *sdk, iothub_property *p)
{
    sdk->property = p;
    return MQTTCLIENT_SUCCESS;
}
int SDKSetAction(struct iothubsdk *sdk, iothub_action_param *a)
{
    sdk->action = a;
    return MQTTCLIENT_SUCCESS;
}
// 属性上报
int SDKPropertyUp(struct iothubsdk *sdk)
{
    char *json = SDKBuildPropertyMsg((*(sdk->property)));
    int rc = MQTTClient_publish(sdk->client, PROPERTY_UP, strlen(json), json, 0, 0, NULL);
    free(json);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        log_error("failed to publish, return code %d", rc);
        return rc; // -1
    }
    return MQTTCLIENT_SUCCESS;
}
// 回复
int SDKPropertyReply(struct iothubsdk *sdk, iothub_reply_msg msg)
{
    // 属性回复 method 为固定值 'property_reply' 外部不可更改
    msg.method = "property_reply";
    char *json = SDKBuildPropertyReplyMsg(msg);
    int rc = MQTTClient_publish(sdk->client, PROPERTY_REPLY, strlen(json), json, 0, 0, NULL);
    free(json);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        log_error("failed to publish, return code %d", rc);
        return rc; // -1
    }
    return MQTTCLIENT_SUCCESS;
}

/// @brief
/// @param sdk
/// @param msg
/// @return
int SDKActionReply(struct iothubsdk *sdk, iothub_reply_msg msg)
{
    // 动作回复 method 为固定值 'action_reply' 外部不可更改
    msg.method = "action_reply";
    char *json = SDKBuildActionReplyMsg(msg);
    int rc = MQTTClient_publish(sdk->client, ACTION_REPLY, strlen(json), json, 0, 0, NULL);
    free(json);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        log_error("failed to publish, return code %d", rc);
        return rc; // -1
    }
    return MQTTCLIENT_SUCCESS;
}
/// @brief
/// @param msg
/// @param payload
/// @return
int SDKParseDownMsg(iothub_down_msg *msg, char *payload)
{
    int status = 0;
    // {"method":"property","id":"1662828896480999","timestamp":1662828896480999,"data":{"a":1}
    cJSON *payload_json = cJSON_Parse(payload);
    if (payload_json == NULL)
    {
        MUTE_STRING error_ptr = cJSON_GetErrorPtr();
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
    // 如果是动作类 需要给加上action id
    if (strcmp(method->valuestring, "action") == 0)
    {
        cJSON *actionid = cJSON_GetObjectItemCaseSensitive(payload_json, "actionid");
        msg->actionid = (char *)malloc(sizeof(strlen(actionid->valuestring))); // 防止泄露
        strcpy(msg->actionid, actionid->valuestring);
    }
    else
    {
        msg->actionid = NULL;
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
    msg->method = (char *)malloc(sizeof(strlen(method->valuestring))); // 防止泄露
    msg->id = (char *)malloc(sizeof(strlen(id->valuestring)));         // 防止泄露
    strcpy(msg->method, method->valuestring);
    strcpy(msg->id, id->valuestring);
    msg->timestamp = timestamp->valueint;
    if (SDKParseParams(msg, data) != 0)
    {
        status = -1;
        log_error("'SDKParseParams error");
        goto end;
    }
end:
    cJSON_Delete(payload_json);
    return status;
}
/// @brief
/// @param msg
/// @param payload
/// @return
PRIVATE int SDKParseParams(iothub_down_msg *msg, cJSON *data)
{
    int status = 0;
    // Property
    if (strcmp(msg->method, "property") == 0)
    {
        //------------------------------------------------------------------------------------------
        // #AUTO-GEN-BEGIN
        //------------------------------------------------------------------------------------------
        cJSON *a = cJSON_GetObjectItemCaseSensitive(data, "a");
        if (!cJSON_IsNumber(a))
        {
            status = -1;
            log_error("'a' must be json number");
            goto END;
        }
        msg->property_data.a = a->valueint;
        cJSON *b = cJSON_GetObjectItemCaseSensitive(data, "b");
        if (!cJSON_IsNumber(b))
        {
            status = -1;
            log_error("'b' must be json number");
            goto END;
        }
        msg->property_data.b = b->valueint;
        //------------------------------------------------------------------------------------------
        // #AUTO-GEN-END
        //------------------------------------------------------------------------------------------
        return 0;
    }
    // Action
    if (strcmp(msg->method, "action") == 0)
    {
        //------------------------------------------------------------------------------------------
        // #AUTO-GEN-BEGIN
        //------------------------------------------------------------------------------------------
        cJSON *action_p1 = cJSON_GetObjectItemCaseSensitive(data, "action_p1");
        if (!cJSON_IsNumber(action_p1))
        {
            status = -1;
            log_error("'action_p1' must be json number");
            goto END;
        }
        msg->action_data.action_p1 = action_p1->valueint;
        cJSON *action_p2 = cJSON_GetObjectItemCaseSensitive(data, "action_p2");
        if (!cJSON_IsNumber(action_p2))
        {
            status = -1;
            log_error("'action_p2' must be json number");
            goto END;
        }
        msg->action_data.action_p2 = action_p2->valueint;
        //------------------------------------------------------------------------------------------
        // #AUTO-GEN-END
        //------------------------------------------------------------------------------------------
    }
END:
    return status;
}
// 停止
int SDKStop(struct iothubsdk *sdk)
{
    MQTTClient_disconnect(sdk->client, 5000);
    MQTTClient_destroy(&sdk->client);
    free(sdk);
}
