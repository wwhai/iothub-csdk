#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iothub.h"
#include "iothubconfig.h"
#include "iothubschema.h"
#include "cJSON.h"
#include "utils.h"
#include "log.h"
// 属性上报
extern const char *PROPERTY_UP;
extern const char *PROPERTY_DOWN;
extern const char *PROPERTY_REPLY;
extern const char *ACTION_DOWN;
extern const char *ACTION_REPLY;
//
PRIVATEFUNC void delivered(void *context, MQTTClient_deliveryToken dt)
{

    log_debug("message delivered with token:%d", dt);
    struct iothubsdk *sdk = (struct iothubsdk *)context;
    sdk->OnDeliver(sdk, (int)dt);
}
// 这里需要解析消息来判断类型
// 如果是属性, 就调用属性的回调: onProperty(propertyJson)
// 如果是动作, 就调用动作的回调: onAction(actionJson)
//
PRIVATEFUNC int msg_arrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{

    log_debug("message arrived, topic:%s, message id: %ld, payload: %s", topicName, message->msgid, message->payload);
    struct iothubsdk *sdk = (struct iothubsdk *)context;

    iothub_down_msg downMsg;
    int rc = SDKParseDownMsg(&downMsg, message->payload);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        log_error("failed to SDKParseDownMsg, return code %d", rc);
        return rc;
    }
    if (strcmp(downMsg.method, "property") == 0)
    {
        sdk->OnProperty(sdk, downMsg);
        return 0;
    }
    if (strcmp(downMsg.method, "action") == 0)
    {
        sdk->OnAction(sdk, downMsg);
        return 0;
    }
    sdk->OnMessage(sdk, message->payload);
    return 0;
}

PRIVATEFUNC void conn_lost(void *context, char *cause)
{
    log_warn("connection lost cause:%s", cause);
    struct iothubsdk *sdk = (struct iothubsdk *)context;
    sdk->connected = 0;
    sdk->OnClosed(sdk, cause);
}
//
struct iothubsdk *SDKNewMqttDevice()
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = MQTT_U;
    conn_opts.password = MQTT_P;
    conn_opts.keepAliveInterval = 30;
    conn_opts.cleansession = 1;
    conn_opts.retryInterval = 5;
    int rc;

    if ((rc = MQTTClient_create(&client, MQTT_HOST, MQTT_C,
                                MQTTCLIENT_PERSISTENCE_DEFAULT,
                                MQTTCLIENT_PERSISTENCE_DEFAULT)) != MQTTCLIENT_SUCCESS)
    {
        log_error("failed to create sdk, return code %d", rc);
        return NULL;
    }
    struct iothubsdk *sdk = (struct iothubsdk *)malloc(sizeof(struct iothubsdk));
    sdk->client = client;
    sdk->conn_opts = conn_opts;
    sdk->connected = 0;
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
    if (sdk != NULL)
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
    return MQTTCLIENT_FAILURE;
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
    {
        if ((rc = MQTTClient_subscribe(sdk->client, PROPERTY_DOWN, 1)) != MQTTCLIENT_SUCCESS)
        {
            log_error("failed to subscribe, return code %d", rc);
            return rc; // -1
        }
    }
    {
        if ((rc = MQTTClient_subscribe(sdk->client, ACTION_DOWN, 1)) != MQTTCLIENT_SUCCESS)
        {
            log_error("failed to subscribe, return code %d", rc);
            return rc; // -1
        }
    }
    sdk->connected = 1;
    return MQTTCLIENT_SUCCESS;
}

//
// 设置属性
//
int SDKSetProperty(struct iothubsdk *sdk, iothub_property *p)
{
    if (sdk != NULL)
    {
        sdk->property = p;
        return MQTTCLIENT_SUCCESS;
    }
    return MQTTCLIENT_FAILURE;
}
// 属性上报
int SDKPropertyUp(struct iothubsdk *sdk)
{
    if (sdk != NULL)
    {
        if (sdk->connected)
        {
            char *json = SDKBuildPropertyMsg((*(sdk->property)));
            int rc = MQTTClient_publish(sdk->client, PROPERTY_UP, strlen(json), json, 1, 0, NULL);
            free(json);
            if (rc != MQTTCLIENT_SUCCESS)
            {
                log_error("failed to publish, return code %d", rc);
                return rc; // -1
            }
        }
        else
        {
            log_error("sdk has disconnected from broker, check your connection.");
            return MQTTCLIENT_FAILURE; // -1
        }
        return MQTTCLIENT_SUCCESS;
    }
    return MQTTCLIENT_FAILURE;
}
// 回复
int SDKPropertyReply(struct iothubsdk *sdk, iothub_reply_msg msg)
{
    if (sdk != NULL)
    {
        if (sdk->connected)
        {
            // 属性回复 method 为固定值 'property_reply' 外部不可更改
            msg.method = "property_reply";
            char *json = SDKBuildPropertyReplyMsg(msg);
            int rc = MQTTClient_publish(sdk->client, PROPERTY_REPLY, strlen(json), json, 1, 0, NULL);
            free(json);
            if (rc != MQTTCLIENT_SUCCESS)
            {
                log_error("failed to publish, return code %d", rc);
                return rc; // -1
            }
        }
        else
        {
            log_error("sdk has disconnected from broker, check your connection.");
            return MQTTCLIENT_FAILURE; // -1
        }
        return MQTTCLIENT_SUCCESS;
    }
    return MQTTCLIENT_FAILURE;
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
PRIVATEFUNC int SDKParseParams(iothub_down_msg *msg, cJSON *data)
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
        cJSON *a = cJSON_GetObjectItemCaseSensitive(data, "a");
        if (!cJSON_IsNumber(a))
        {
            status = -1;
            log_error("'a' must be json number");
            goto END;
        }
        msg->action_data.action_p1 = a->valueint;
        cJSON *b = cJSON_GetObjectItemCaseSensitive(data, "b");
        if (!cJSON_IsNumber(b))
        {
            status = -1;
            log_error("'b' must be json number");
            goto END;
        }
        msg->action_data.action_p2 = b->valueint;
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
