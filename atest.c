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
    SDKBuildPropertyReplyMsg(m);
    //
    iothub_down_msg msg;
    //{\"method\":\"property\",\"id\":\"1662828896480999\",\"timestamp\":1662828896480999,\"data\":{\"a\":1}
    SDKParseDownMsg(&msg, "{\"method\":\"action\",\"id\":\"abcde\",\"actionid\":\"actionid-1\",\"timestamp\":199,\"data\":{\"a\":1,\"b\":2}}");
    printf("SDKParseDownMsg TEST==> msg.id: %s, msg.method: %s, msg.actionid: %s, msg.timestamp: %ld, msg.p.a: %d, msg.p.b: %d\n",
           msg.id, msg.method,msg.actionid, msg.timestamp, msg.property_data.a, msg.property_data.b);
    //
    iothub_reply_msg am = {
        .method = "action_reply",
        .id = "id00000001",
        .code = 1,
        .timestamp = 1,
        .status = "OK",
        .actionid = "id00000001",
        .out = {
            .action_p1 = 1,
            .action_p2 = 2,
        },
    };
    SDKBuildPropertyReplyMsg(am);
    return 0;
}
