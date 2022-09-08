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
        .id = 1,
        .code = 1,
        .timestamp = 1,
        .status = "OK",
    };
    SDKBuildReplyMsg(m);

    exit(0);
}
