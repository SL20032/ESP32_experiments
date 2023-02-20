#ifndef STORAGE_H  
#define STORAGE_H

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "nvs_flash.h"

typedef struct SPIFFS_file_handle
{
    size_t file_len;
    char * file_pointer;
} SPIFFS_file_handle_t;

void NVS_init(void);

esp_err_t SPIFFS_mount_partition(const char* base_path);
const SPIFFS_file_handle_t SPIFFS_js_code_file_handler(void);
const SPIFFS_file_handle_t SPIFFS_index_html_file_handler(void);
const SPIFFS_file_handle_t SPIFFS_styles_css_file_handler(void);

#endif

