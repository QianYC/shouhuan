#include "network.h"

esp_http_client_handle_t client=NULL;

esp_err_t wifi_init()
{
    //enable NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    //Set interface
    tcpip_adapter_init();

    //ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    //esp_netif_create_default_wifi_sta();

    //enable wifi
    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_config));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = NET_SSID,
            .password = NET_PWD,
            //.pmf_cfg = {
            //    .capable = true,
            //    .required = false
            //},
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    //enable connection, if fail, return error code
    return esp_wifi_connect();
}

static esp_err_t http_event_handle(esp_http_client_event_t *e)
{
    return ESP_OK;
}

esp_err_t http_upload(int heartRate, int bloodOxy, float temp)
{
    char post[POST_LEN];
    if (client == NULL)
    {
        esp_http_client_config_t config = {
            .url = HTTP_URL,
            .event_handler = http_event_handle,
        };
        client = esp_http_client_init(&config);
    }
    sprintf(post,"\"Temp\" : %f, \"SPO\" : %d, \"HR\" : %d",temp,bloodOxy,heartRate);
    ESP_ERROR_CHECK(esp_http_client_set_method(client,HTTP_METHOD_POST));
    ESP_ERROR_CHECK(esp_http_client_set_header(client, "Content-Type", "application/json"));
    ESP_ERROR_CHECK(esp_http_client_set_post_field(client,post,POST_LEN));
    return esp_http_client_perform(client);
}
