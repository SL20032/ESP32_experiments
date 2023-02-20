#include "WiFi.h"

static const char *TAG = "wifi station";

//WiFi connection control varibles
static int WiFi_connection_status = WIFI_DISABLE;
static int WiFi_try_to_connect_retry_counter = 0;

//handlers for WiFi event system
static esp_event_handler_instance_t instance_any_id;
static esp_event_handler_instance_t instance_got_ip;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        WiFi_connection_status = WIFI_TRY_TO_MAKE_CONNECTION;
    } 
    
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (WiFi_try_to_connect_retry_counter < WIFI_TRY_TO_CONNECT_RETRY) {
            esp_wifi_connect();
            WiFi_try_to_connect_retry_counter++;
            ESP_LOGI(TAG, "module try to make connection fail -> retrying");
        } else {
            WiFi_connection_status = WIFI_GIVE_UP_ON_CONNECTION;
        }
    } 
    
    //chenge event to WIFI event for connection
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "module connected sucessfully -> got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        WiFi_try_to_connect_retry_counter = 0;
        WiFi_connection_status = WIFI_SUCESSFUL_CONNECTION;
    }

    //need this?
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_STOP)
    {
        WiFi_connection_status = WIFI_DISABLE;
        ESP_LOGI(TAG, "module disenable");
    }
}

void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
	     .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");
}







