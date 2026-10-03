#include "esp_http_server.h"
#include "esp_wifi.h"
#include <stdio.h>
#include <string.h>
#include "web.h"


static esp_err_t csv_get(httpd_req_t *req)
{
    FILE *f = fopen("/main/data.csv", "r");
    if (f == NULL) {
        httpd_resp_send(req, "no file", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }

    httpd_resp_set_type(req, "text/csv");
    httpd_resp_set_hdr(req, "Content-Disposition", "attachment; filename=data.csv");

    char buf[512];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        httpd_resp_send_chunk(req, buf, n);
    }

    fclose(f);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

void web_init(void)
{
    esp_netif_create_default_wifi_ap();

    wifi_config_t ap = {
        .ap = {
            .ssid = "3net277",
            .ssid_len = 9,
            .password = "10101010",
            .max_connection = 2,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_APSTA);
    esp_wifi_set_config(WIFI_IF_AP, &ap);

    httpd_handle_t server = NULL;
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    httpd_start(&server, &cfg);

    httpd_uri_t uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = csv_get,
    };
    httpd_register_uri_handler(server, &uri);
}