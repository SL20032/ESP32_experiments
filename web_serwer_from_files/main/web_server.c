#include "web_server.h"

static const char *TAG = "web_server";

static const bool chec_is_uri_match(const char * uri, const char * segment_to_be_matched)
{
    const size_t uri_len = strlen(uri);
    const size_t matched_segment_len = strlen(segment_to_be_matched);
    size_t pointer = 0;

    for (size_t i = uri_len - 1; i >= 0; i--)
    {
        if (uri[i] == '/')
        {
            pointer = i;
            break;
        }
    }

    for (size_t i = 0; i < matched_segment_len; i++)
    {
        if (uri[i + pointer] != segment_to_be_matched[i]) return false;        
    }
    
    if (pointer + matched_segment_len == uri_len) return true;
    else if (uri[pointer + matched_segment_len] == '?') return true;

    return false;
}

static const char * get_last_uri_path_segment(const char * uri)
{
    const size_t uri_len = strlen(uri);

    for (size_t i = uri_len - 1; i >= 0; i--)
    {
        if (uri[i] == '/') return &uri[i];
    }
    return NULL; 
}

static esp_err_t serve_css_handle(httpd_req_t *req)
{
    const SPIFFS_file_handle_t file_css = SPIFFS_styles_css_file_handler();

    httpd_resp_set_type(req, "text/css");
    httpd_resp_send_chunk(req,file_css.file_pointer,file_css.file_len);
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static esp_err_t serve_js_handle(httpd_req_t *req)
{
    const SPIFFS_file_handle_t file_js = SPIFFS_js_code_file_handler();

    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send_chunk(req,file_js.file_pointer,file_js.file_len);
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static esp_err_t embed_hile_header_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Found URL query => %s", req->uri);
    const char * last_segment_start = get_last_uri_path_segment(req->uri);

    if (strcmp(last_segment_start, "/mystyles.css") == 0)
    {
        serve_css_handle(req);
        return ESP_OK;
    }
    else if(strcmp(last_segment_start, "/code.js") == 0)
    {
        serve_js_handle(req);
        return ESP_OK;
    }

    return ESP_FAIL;    
}

httpd_uri_t embed_file = {
    .uri       = "/embed_files*",  // Match all URIs of type /path/to/file
    .method    = HTTP_GET,
    .handler   = embed_hile_header_handler,
    .user_ctx  = NULL,    // Pass server data as context
};

static esp_err_t main_site_handle(httpd_req_t *req)
{
    const SPIFFS_file_handle_t file_html = SPIFFS_index_html_file_handler();

    httpd_resp_set_type(req, "text/html"); 
    httpd_resp_send_chunk(req,file_html.file_pointer,file_html.file_len);
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

httpd_uri_t main_site = {
    .uri       = "/home",  // Match all URIs of type /path/to/file
    .method    = HTTP_GET,
    .handler   = main_site_handle,
    .user_ctx  = NULL,    // Pass server data as context
};

static esp_err_t data_handle(httpd_req_t *req)
{    
    httpd_resp_sendstr_chunk(req,"test0=000&test1=123&test2=5674&test3=teraccotta");
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

httpd_uri_t test = {
    .uri       = "/data_update_GET",  // Match all URIs of type /path/to/file
    .method    = HTTP_GET,
    .handler   = data_handle,
    .user_ctx  = NULL,    // Pass server data as context
};


esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.uri_match_fn = httpd_uri_match_wildcard;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &embed_file);
        httpd_register_uri_handler(server, &main_site);
        httpd_register_uri_handler(server, &test);
        
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

void web_serwer_init(void)
{
    static httpd_handle_t server = NULL;
    
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    start_webserver();
}