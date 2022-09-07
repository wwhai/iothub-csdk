MQTT_SERVER=`jq .server iothub-config.json | sed 's/\"//g'`
MQTT_C=`jq .deviceid iothub-config.json | sed 's/\"//g'`
MQTT_U=`jq .username iothub-config.json | sed 's/\"//g'`
MQTT_P=`jq .password iothub-config.json | sed 's/\"//g'`
IOTHUB_PRODUCT_ID=`jq .productid iothub-config.json | sed 's/\"//g'`
# Check json params
check(){
    if [ $1 = "null" ]; then
        echo "!!"
        echo "!! Missing params! Check your iothub-config.json"
        echo "!!"
        exit 1
    fi
}
check $MQTT_SERVER
check $MQTT_U
check $MQTT_P
check $MQTT_P
check $IOTHUB_PRODUCT_ID
#---------------------------------------------------------------------------------------------------
# 生成MQTT配置
#---------------------------------------------------------------------------------------------------
cat >./iothubconfig.h <<EOF
#ifndef MQTTCONFIG_H
#define MQTTCONFIG_H

//--------------------------------------------------------------------------------------------------
// SDK默认配置模板
//--------------------------------------------------------------------------------------------------
#define MQTT_HOST "tcp://${MQTT_SERVER}:1883"
#define MQTT_C "${MQTT_C}"
#define MQTT_U "${MQTT_U}"
#define MQTT_P "${MQTT_P}"
#define IOTHUB_PRODUCT_ID "${IOTHUB_PRODUCT_ID}"

#endif
EOF
#---------------------------------------------------------------------------------------------------
# 生成iothubTopic
#---------------------------------------------------------------------------------------------------
cat >./iothubconfig.c <<EOF
#include "iothubconfig.h"
// 属性上报
const char *PROPERTY_UP = "\$thing/up/property/${IOTHUB_PRODUCT_ID}/${MQTT_C}";
// 属性下发
const char *PROPERTY_DOWN = "\$thing/down/property/${IOTHUB_PRODUCT_ID}/${MQTT_C}";
// 属性回复
const char *PROPERTY_REPLY = "\$thing/property/reply/${IOTHUB_PRODUCT_ID}/${MQTT_C}";
// 属性下发
const char *ACTION_DOWN = "\$thing/down/action/${IOTHUB_PRODUCT_ID}/${MQTT_C}";
// 动作回复
const char *ACTION_REPLY = "\$thing/action/reply/${IOTHUB_PRODUCT_ID}/${MQTT_C}";
EOF
