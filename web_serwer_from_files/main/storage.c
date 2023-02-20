#include "storage.h"

static const char *TAG = "STORAGE";

esp_err_t SPIFFS_mount_partition(const char* base_path)
{
    ESP_LOGI(TAG, "SPIFFS: Init");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = base_path,
        .partition_label = NULL,
        .max_files = 5,   // This sets the maximum number of files that can be open at the same time
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "SPIFFS: Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "SPIFFS: Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "SPIFFS: Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS: Failed to get partition information (%s)", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "SPIFFS: Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

const SPIFFS_file_handle_t SPIFFS_js_code_file_handler(void)
{
    extern const unsigned char js_start[] asm("_binary_code_js_start");
    extern const unsigned char js_end[]   asm("_binary_code_js_end");
    const size_t js_size = (js_end - js_start);

    SPIFFS_file_handle_t return_struct = {
        .file_len = js_size,
        .file_pointer = js_start,
    };

    return return_struct;
}

const SPIFFS_file_handle_t SPIFFS_index_html_file_handler(void)
{
    /* Get handle to embedded file upload script */
    extern const unsigned char html_start[] asm("_binary_index_html_start");
    extern const unsigned char html_end[]   asm("_binary_index_html_end");
    const size_t html_size = (html_end - html_start);

    SPIFFS_file_handle_t return_struct = {
        .file_len = html_size,
        .file_pointer = html_start,
    };

    return return_struct;
}

const SPIFFS_file_handle_t SPIFFS_styles_css_file_handler(void)
{
    extern const unsigned char css_start[] asm("_binary_mystyles_css_start");
    extern const unsigned char css_end[]   asm("_binary_mystyles_css_end");
    const size_t css_size = (css_end - css_start);

    SPIFFS_file_handle_t return_struct = {
        .file_len = css_size,
        .file_pointer = css_start,
    };

    return return_struct;
}



void NVS_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}
