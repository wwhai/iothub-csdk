#include "iothub.h"
#include <stdlib.h>
/**
 * 消息回调
 *
 */
PRIVATEFUNC void OnMessage(struct iothubsdk *sdk, char *message){
    //
    // SDKPropertyReply(sdk, "");
    //
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
    iothub_property_msg msg;
    msg.method = "property";
    msg.id = 1;
    msg.timestamp = 1;
    iothub_property p = {
        1,
        2,
    };
    msg.p = p;
    build_iothub_property_msg(msg);
    log_set_level(LOG_DEBUG);
    struct iothubsdk *sdk = SDKNewMqttDevice();
    if (sdk == NULL)
    {
        log_fatal("sdk create failed");
        return 0;
    }
    else
    {
        log_info("sdk create successfully");
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
    printf("Application terminal[VTTY]\n");
    for (;;)
    {
        printf("Input cmd $> ");
        scanf("%s", cmd);
        if (cmd[0] == 'Q' || cmd[0] == 'q')
        {
            log_info("application exit");
            SDKStop(sdk);
            exit(0);
        }
        else
        {
            printf("# help: ['Q' or 'q' for exit]\n");
        }
    }
    SDKStop(sdk);
    return 0;
}
