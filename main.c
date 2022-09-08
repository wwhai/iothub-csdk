#include "iothub.h"
#include "iothubschema.h"
#include <stdlib.h>
/**
 * 消息回调
 *
 */
PRIVATEFUNC void OnMessage(struct iothubsdk *sdk, char *message)
{

    iothub_reply_msg msg = {
        .method = "property_reply",
        .id = 10086,
        .code = 10086,
        .timestamp = 10086,
        .status = "TEST-OK",
    };
    SDKPropertyReply(sdk, msg);
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
    log_info("set struct iothubsdk callback");
    if (SDKSetCallback(sdk, OnMessage, OnClosed, OnDeliver) != 0)
    {
        log_fatal("callback create failed");
        exit(0);
    }
    else
    {
        log_info("callback create successfully");
    }
    if (SDKStart(sdk) != 0)
    {
        log_fatal("sdk start failed");
        exit(0);
    }
    else
    {
        log_info("sdk start successfully");
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
            exit(0);
        }
        if (cmd[0] == 'H' || cmd[0] == 'h')
        {
            printf("# SDK for linux C, Version:0.0.1.\n");
        }
        else
        {
            printf("# help: ['Q' or 'q' for exit]\n");
        }
    }
    SDKStop(sdk);
    return 0;
}
