/* WiFi station Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <stdio.h>

#include "WiFi.h"
#include "storage.h"
#include "web_server.h"


static const char *TAG = "main";


void app_main(void)
{
    NVS_init();
    wifi_init_sta();

    const char* base_path = "/data";

    SPIFFS_mount_partition(base_path);

    vTaskDelay(5000 / portTICK_PERIOD_MS);

    web_serwer_init();
}
