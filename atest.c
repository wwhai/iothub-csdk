#include "iothub.h"
#include "iothubschema.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
int publish();
/**
 * 消息回调
 *
 */

PRIVATE void OnProperty(struct iothubsdk *sdk, iothub_down_msg msg)
{
    iothub_reply_msg rMsg = {
        .method = "property_reply",
        .id = msg.id,
        .code = 10086,
        .timestamp = 123456,
        .status = "TEST-OK",
    };
    assert(msg.id[0] == 't');
    assert(msg.id[1] == 'e');
    assert(msg.id[2] == 's');
    assert(msg.id[3] == 't');

    assert(msg.timestamp == 159);
    assert(msg.property_data.a == 147);
    assert(msg.property_data.b == 258);
    SDKPropertyReply(sdk, rMsg);
};
PRIVATE void OnAction(struct iothubsdk *sdk, iothub_down_msg msg)
{
    iothub_reply_msg rMsg = {
        .method = "action_reply",
        .id = msg.id,
        .code = 10086,
        .timestamp = 123456,
        .actionid = "test",
        .status = "test",
        .out = {
            .action_p1 = 1001,
            .action_p2 = 2001,
        }};
    assert(msg.id[0] == 't');
    assert(msg.id[1] == 'e');
    assert(msg.id[2] == 's');
    assert(msg.id[3] == 't');
    assert(msg.action_data.action_p1 == 1002);
    assert(msg.action_data.action_p2 == 2001);

    SDKActionReply(sdk, rMsg);
};
/**
 * 未识别消息回调
 *
 */
PRIVATE void OnMessage(struct iothubsdk *sdk, char *message){

};
/**
 * 掉线回调
 *
 */
PRIVATE void OnClosed(struct iothubsdk *sdk, char *reason){

};
/**
 * 消息发布成功回调
 *
 */
PRIVATE void OnDeliver(struct iothubsdk *sdk, int token){

};
// 测试发送消息

int publish()
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = "test-sender";
    conn_opts.password = "test-sender";
    conn_opts.cleansession = 1;  // True
    conn_opts.retryInterval = 5; // 5s
    int rc;
    if ((rc = MQTTClient_create(&client, MQTT_HOST, "test-sender",
                                MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        log_error("failed to create sdk, return code %d", rc);
        return 0;
    }
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        log_error("failed to connect mqtt server: %s, username: %s, clientid: %s, return code %d",
                  MQTT_U, MQTT_C, MQTT_HOST, rc);
        return rc; // -1
    }
    const char *jsons1 = "{\"method\":\"action\",\"id\":\"test\",\"actionid\":\"test\",\"timestamp\":159,\"data\":{\"action_p1\":1002,\"action_p2\":2001}}";
    const char *jsons2 = "{\"method\":\"property\",\"id\":\"test\",\"timestamp\":159,\"data\":{\"a\":147,\"b\":258}}";
    MQTTClient_publish(client, "$thing/down/action/test/test1", strlen(jsons1), jsons1, 0, 0, NULL);
    MQTTClient_publish(client, "$thing/down/property/test/test1", strlen(jsons2), jsons2, 0, 0, NULL);
}
/**
 *
 *
 */
int main(int argc, char const *argv[])
{

    log_set_level(LOG_DEBUG);
    struct iothubsdk *sdk = SDKNewMqttDevice();
    if (sdk == NULL)
    {
        log_fatal("sdk create failed");
        return 0;
    }
    if (SDKSetCallback(sdk, OnMessage, OnClosed, OnDeliver, OnProperty, OnAction) != 0)
    {
        log_fatal("callback create failed");
        return 0;
    }
    if (SDKStart(sdk) != 0)
    {
        log_fatal("sdk start failed");
        return 0;
    }
    publish();
    sleep(2);
    SDKStop(sdk);
    return 0;
}
