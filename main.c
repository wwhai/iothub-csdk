#include "iothub.h"
#include "iothubschema.h"
#include <stdlib.h>
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
    SDKPropertyReply(sdk, rMsg);
    // SDKPropertyUp(sdk);
};
PRIVATE void OnAction(struct iothubsdk *sdk, iothub_down_msg msg)
{
    iothub_reply_msg rMsg = {
        .method = "action_reply",
        .id = "10086",
        .code = 10086,
        .timestamp = 10086,
        .actionid = "test-action",
        .status = "TEST-OK",
        .out = {
            .action_p1 = 1001,
            .action_p2 = 2001,
        }};
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
    char cmd[8] = {0};
    printf("SDK terminal [STTY] @linux\n");
    for (;;)
    {
        printf("Input cmd $> ");
        scanf("%s", cmd);
        if (cmd[0] == 'Q' || cmd[0] == 'q')
        {
            log_info("exit");
            SDKStop(sdk);
            return 0;
        }
        if (cmd[0] == 'H' || cmd[0] == 'h')
        {
            printf("# SDK for linux C, Version:0.0.1\n");
        }
        if (cmd[0] == 'P' || cmd[0] == 'p')
        {
            printf("# MQTT_H: %s\n", MQTT_HOST);
            printf("# MQTT_P: %s\n", MQTT_P);
            printf("# MQTT_U: %s\n", MQTT_U);
            printf("# MQTT_C: %s\n", MQTT_C);
            printf("# STATE: %d\n", MQTTClient_isConnected(sdk->client));
        }
        else
        {
            printf("## command help ##\n1. 'Q' or 'q' for exit\n2. 'H' or 'h' help message\n");
        }
    }
    SDKStop(sdk);
    return 0;
}
