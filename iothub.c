#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iothub.h"
#include "iothubconfig.h"
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

PRIVATEFUNC int msg_arrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    log_debug("message arrived, topic:%s, message id: %ld, payload: %s", topicName, message->msgid, message->payload);
    struct iothubsdk *sdk = (struct iothubsdk *)context;
    sdk->OnMessage(sdk, (char *)message->payload);
    return 1;
}

PRIVATEFUNC void conn_lost(void *context, char *cause)
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
    sdk->connected = 1;
    log_debug("sdk create successfully");
    return sdk;
}
// 回调
int SDKSetCallback(struct iothubsdk *sdk,
                   void (*OnMessage)(struct iothubsdk *sdk, char *message),
                   void (*OnClosed)(struct iothubsdk *sdk, char *reason),
                   void (*OnDeliver)(struct iothubsdk *sdk, int token))
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
            char *json = SDKBuildReplyMsg(msg);
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
// 停止
int SDKStop(struct iothubsdk *sdk)
{
    MQTTClient_disconnect(sdk->client, 5000);
    MQTTClient_destroy(&sdk->client);
    free(sdk);
}
