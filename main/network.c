#include "network.h"
#include "string.h"

esp_http_client_handle_t client = NULL;
char id[ID_LENGTH];
uint8_t haveId;
//uint8_t rcvBuffer[HTTP_BUFFER_SIZE];

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

static int check_wifi(){
    wifi_ap_record_t ap;
    if(ESP_OK != esp_wifi_sta_get_ap_info(&ap)){
        return 1;
    }
    return 0;
}

static esp_err_t http_event_handle(esp_http_client_event_t *e)
{
    switch (e->event_id)
    {
    case HTTP_EVENT_ERROR:
        //printf("Event error\n");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        //printf("Connected to Server\n");
        break;
    case HTTP_EVENT_HEADER_SENT:
        break;
    case HTTP_EVENT_ON_HEADER:
        break;
    case HTTP_EVENT_ON_FINISH:
        //printf("Finish\n");
        break;
    case HTTP_EVENT_DISCONNECTED:
        //printf("Disconnected to Server\n");
        break;
    case HTTP_EVENT_ON_DATA:
        if (!esp_http_client_is_chunked_response(e->client) && e->data_len == ID_LENGTH)
        {
            memcpy(id, e->data, ID_LENGTH);
            haveId=1;
            printf("Get ID: %s\n", id);
        }
        break;
    }
    return ESP_OK;
}

static int mayInitClient(char *url){
    if (NULL == client)
    {
        esp_http_client_config_t config = {
            .url = url,
            .event_handler = http_event_handle,
        };
        client = esp_http_client_init(&config);
    }
    return  NULL == client;
}

esp_err_t http_getId()
{
    //Check wifi connection
    if(check_wifi()){
        if(ESP_OK != esp_wifi_connect()){
            printf("Unable to connect wifi\n");
            return ESP_OK;
        }
    }

    //Check http connection
    if(mayInitClient(ID_URL)){
        printf("Connection Failed\n");
        return ESP_OK;
    }
    

    ESP_ERROR_CHECK(esp_http_client_set_url(client, ID_URL));
    ESP_ERROR_CHECK(esp_http_client_set_method(client, HTTP_METHOD_GET));
    ESP_ERROR_CHECK(esp_http_client_perform(client));
    return ESP_OK;
}

esp_err_t http_upload(int heartRate, int bloodOxy, float temp)
{
    char post[POST_LEN];
    //Check wifi connection
    if(check_wifi()){
        if(ESP_OK != esp_wifi_connect()){
            printf("Unable to connect wifi\n");
            return ESP_OK;
        }
    }

    //Check http connection
    if(mayInitClient(UPLOAD_URL)){
        printf("Connection Failed\n");
        return ESP_OK;
    }
    

    if (haveId == 1)
    {
        sprintf(post,"{\"id\":\"%s\",\"Temp\":\"%f\",\"SPO\":\"%d\",\"HR\":\"%d\"}", id, temp, bloodOxy, heartRate);
        ESP_ERROR_CHECK(esp_http_client_set_url(client, UPLOAD_URL));
        ESP_ERROR_CHECK(esp_http_client_set_method(client, HTTP_METHOD_POST));
        ESP_ERROR_CHECK(esp_http_client_set_header(client, "Content-Type", "application/json"));
        ESP_ERROR_CHECK(esp_http_client_set_post_field(client, post, strlen(post)));
        return esp_http_client_perform(client);
    }
    else
    {
        printf("Can't upload due to lack of ID\n");
        return ESP_OK;
    }
}