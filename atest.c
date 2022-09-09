#include <stdio.h>
#include "iothub.h"
#include "atest.h"
#include "iothubschema.h"
#include <stdlib.h>
/**
 * 单元测试
 **/
int main(int argc, char const *argv[])
{

    log_set_level(LOG_DEBUG);
    iothub_property p = {
        1,
        2,
    };
    SDKBuildPropertyMsg(p);
    iothub_reply_msg m = {
        .method = "property_reply",
        .id = "id00000001",
        .code = 1,
        .timestamp = 1008,
        .status = "OK",
    };
    SDKBuildReplyMsg(m);
    //
    iothub_property_msg msg;
    SDKParsePropertyMsg(&msg, "{\"method\":\"property\",\"id\":\"abcde\",\"timestamp\":199,\"data\":{\"a\":1,\"b\":2}}");
    printf("SDKParsePropertyMsg TEST==> msg.id: %s, msg.method: %s, msg.timestamp: %ld, msg.p.a: %d, msg.p.b: %d\n", msg.id, msg.method, msg.timestamp, msg.p.a, msg.p.b);
    //
    iothub_action_reply_msg am = {
        .reply_msg = {
            .method = "action_reply",
            .id = "id00000001",
            .code = 1,
            .timestamp = 1,
            .status = "OK",
        },
        .actionid = "id00000001",
        .out = {
            .action_p1 = 1,
            .action_p2 = 2,
        },
    };
    SDKBuildActionReplyMsg(am);
    return 0;
}
