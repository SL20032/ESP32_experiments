#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define WIFI_SSID                   "WIRE8BIS_G"                //"Orange_Swiatlowod_DB80_Ext"
#define WIFI_PASS                   "Kojano5286Q@TS!"           //"NN4UY7DDF59X"
#define WIFI_TRY_TO_CONNECT_RETRY   5

//Macros for WiFi connection status info
#define WIFI_SUCESSFUL_CONNECTION       0x10
#define WIFI_GIVE_UP_ON_CONNECTION      0x20
#define WIFI_TRY_TO_MAKE_CONNECTION     0x40
#define WIFI_DISABLE                    0x0

void wifi_init_sta(void);
