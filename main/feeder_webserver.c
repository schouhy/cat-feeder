/* Simple HTTP Server Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"
#include "esp_http_server.h"

#include "feeder_event_source.h"
#include "feeder_state.h"
#include "feeder_timer.h"

static const char *TAG = "WEBSERVER";

/* An HTTP GET handler */
static esp_err_t config_get_handler(httpd_req_t *req)
{
    char strftime_buf[17];
    size_t buf_size = get_lid_open_time_str((char *)strftime_buf);
    if(buf_size != 0) {
        httpd_resp_sendstr_chunk(req, "Configurado para abrirse: ");
        httpd_resp_sendstr_chunk(req, strftime_buf);
        httpd_resp_sendstr_chunk(req, "<br>");

        char opening_time_buf[4];
        snprintf(opening_time_buf, 3, "%i", LID_OPENING_TIME_IN_MINUTES);
        httpd_resp_sendstr_chunk(req, "Tiempo de apertura: ");
        httpd_resp_sendstr_chunk(req, opening_time_buf);
        httpd_resp_sendstr_chunk(req, " minutos.");
        httpd_resp_sendstr_chunk(req, NULL);
    }
    else {

        time_t now;
        char strftime_buf[17];
        struct tm timeinfo;

        time(&now);

        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%dT%H:%M", &timeinfo);

        httpd_resp_sendstr_chunk(req, "<form action=\"/set\" method=\"post\">");
        httpd_resp_sendstr_chunk(req, "<label for=\"datetime\">Fecha y hora:</label><br>");
        httpd_resp_sendstr_chunk(req, "<input type=\"datetime-local\" id=\"datetime\" name=\"datetime\" value=\"");
        httpd_resp_sendstr_chunk(req, strftime_buf);
        httpd_resp_sendstr_chunk(req, "\"><br>");
        httpd_resp_sendstr_chunk(req, " <input type=\"submit\" value=\"Programar\">");
        httpd_resp_sendstr_chunk(req, "</form>");
        httpd_resp_sendstr_chunk(req, NULL);

    }
    return ESP_OK;
}

static const httpd_uri_t config_uri = {
    .uri       = "/config",
    .method    = HTTP_GET,
    .handler   = config_get_handler,
    .user_ctx  = NULL
};

static esp_err_t set_post_handler(httpd_req_t *req)
{
    SystemState current_state = get_current_state();

    if(current_state < WAITING_FOR_SCHEDULE) {
        httpd_resp_sendstr_chunk(req, "Iniciando sistema.");
        httpd_resp_sendstr_chunk(req, NULL);
        return ESP_OK;
    }
    else if(current_state > WAITING_FOR_SCHEDULE) {
        httpd_resp_sendstr_chunk(req, "Para volver a configurar resetear el aparato.");
        httpd_resp_sendstr_chunk(req, NULL);
        return ESP_OK;
    }

    char buf[27];
    int ret = httpd_req_recv(req, buf, 27);
    if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        return ESP_FAIL;
    ESP_LOGI(TAG, "%s", buf);

    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    strptime(buf, "datetime=%Y-%m-%dT%H%%3A%M", &tm);

    set_lid_open_time_t(mktime(&tm));
    ESP_ERROR_CHECK(esp_event_post(WEBSERVER_EVENTS, WEBSERVER_EVENT_TIME_CONFIGURED, NULL, 0, portMAX_DELAY));

    httpd_resp_sendstr_chunk(req, "Programado con &eacute;xito");
    httpd_resp_sendstr_chunk(req, NULL);

    return ESP_OK;
}

static const httpd_uri_t set_uri = {
    .uri       = "/set",
    .method    = HTTP_POST,
    .handler   = set_post_handler,
    .user_ctx  = NULL
};

httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &config_uri);
        httpd_register_uri_handler(server, &set_uri);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

// static void stop_webserver(httpd_handle_t server)
// {
//     // Stop the httpd server
//     httpd_stop(server);
// }
// 
// static void disconnect_handler(void* arg, esp_event_base_t event_base,
//                                int32_t event_id, void* event_data)
// {
//     httpd_handle_t* server = (httpd_handle_t*) arg;
//     if (*server) {
//         ESP_LOGI(TAG, "Stopping webserver");
//         stop_webserver(*server);
//         *server = NULL;
//     }
// }
// 
// static void connect_handler(void* arg, esp_event_base_t event_base,
//                             int32_t event_id, void* event_data)
// {
//     httpd_handle_t* server = (httpd_handle_t*) arg;
//     if (*server == NULL) {
//         ESP_LOGI(TAG, "Starting webserver");
//         *server = start_webserver();
//     }
// }