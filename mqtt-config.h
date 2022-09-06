//--------------------------------------------------------------------------------------------------
// SDK默认配置模板
//--------------------------------------------------------------------------------------------------
// 调试开关
#define DEBUG ${DEBUG}
// MQTT 配置
#define MQTT_HOST "${MQTT_HOST}"
#define MQTT_PORT ${MQTT_PORT}
#define MQTT_U "${MQTT_U}"
#define MQTT_C "${MQTT_C}"
#define MQTT_P "${MQTT_P}"
#define IOTHUB_PRODUCT_ID "${IOTHUB_PRODUCT_ID}"
// 属性上报
char propertyUp[] = "$thing/up/property/${IOTHUB_PRODUCT_ID}/${MQTT_C}";
// 属性下发
char propertyDown[] = "$thing/down/property/${IOTHUB_PRODUCT_ID}/${MQTT_C}";
// 属性下发
char actionDown[] = "$thing/down/action/${IOTHUB_PRODUCT_ID}/${MQTT_C}";
// 动作回复
char actionReply[] = "$thing/action/reply/${IOTHUB_PRODUCT_ID}/${MQTT_C}";

//
// NTP 配置:
// https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino
//
const char *ntpServer0 = "0.cn.pool.ntp.org";
const char *ntpServer1 = "1.cn.pool.ntp.org";
const char *ntpServer2 = "2.cn.pool.ntp.org";
const long gmtOffset_sec = 7 * 60 * 60;
const int daylightOffset_sec = 3600;
struct tm timeinfo;
