#include "iothub.h"
#include "iothubschema.h"
#include <stdlib.h>
/**
 * 消息回调
 *
 */

PRIVATEFUNC void OnProperty(struct iothubsdk *sdk, iothub_down_msg msg)
{
    iothub_reply_msg rMsg = {
        .method = "property_reply",
        .id = msg.id,
        .code = 10086,
        .timestamp = 123456,
        .status = "TEST-OK",
    };
    SDKPropertyReply(sdk, rMsg);
};
PRIVATEFUNC void OnAction(struct iothubsdk *sdk, iothub_down_msg msg){
    // iothub_reply_msg msg = {
    //     .method = "action_reply",
    //     .id = "10086",
    //     .code = 10086,
    //     .timestamp = 10086,
    //     .actionid = "test-action",
    //     .status = "TEST-OK",
    // };
    // TODO SDKActionReply(sdk, msg);
};
/**
 * 未识别消息回调
 *
 */
PRIVATEFUNC void OnMessage(struct iothubsdk *sdk, char *message){

};
/**
 * 掉线回调
 *
 */
PRIVATEFUNC void OnClosed(struct iothubsdk *sdk, char *reason){

};
/**
 * 消息发布成功回调
 *
 */
PRIVATEFUNC void OnDeliver(struct iothubsdk *sdk, int token){

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
        else
        {
            printf("## command help ##\n1. 'Q' or 'q' for exit\n2. 'H' or 'h' help message\n");
        }
    }
    SDKStop(sdk);
    return 0;
}
