#include "stdio.h"
#include "iothubschema.h"
#include "cJSON.h"
void build_iothub_property_msg(iothub_property_msg msg)
{
    // 构建外层JSON
    cJSON *json = cJSON_CreateObject();
    cJSON *method = cJSON_CreateString(msg.method);
    cJSON *id = cJSON_CreateNumber(msg.id);
    cJSON *timestamp = cJSON_CreateNumber(msg.timestamp);
    cJSON_AddItemToObject(json, "method", method);
    cJSON_AddItemToObject(json, "id", id);
    cJSON_AddItemToObject(json, "timestamp", timestamp);
    // 构建data
    cJSON *data = cJSON_CreateObject();
    {
        cJSON *a = cJSON_CreateNumber(msg.p.a);
        cJSON *b = cJSON_CreateNumber(msg.p.b);
        cJSON_AddItemToObject(data, "a", a);
        cJSON_AddItemToObject(data, "b", b);
    }
    cJSON_AddItemToObject(json, "data", data);
    char *string = cJSON_PrintUnformatted(json);
    printf("Json:%s\n", string);
}