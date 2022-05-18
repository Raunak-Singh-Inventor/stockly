#include "core2forAWS.h"
#include "freertos/task.h" 

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

#include "includes/wifi.h"

#include "includes/amzn/amzn_functions.hpp"
#include "includes/amzn/amzn.h"

/* CA Root certificate */
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t aws_root_ca_pem_end[] asm("_binary_aws_root_ca_pem_end");

/* Default MQTT HOST URL is pulled from the aws_iot_config.h */
char HostAddress[255] = AWS_IOT_MQTT_HOST;

/* Default MQTT port is pulled from the aws_iot_config.h */
uint32_t port = AWS_IOT_MQTT_PORT;

char* tag = "STOCKLY";

void disconnect_callback_handler(AWS_IoT_Client *pClient, void *data) {
    ESP_LOGW(tag, "MQTT Disconnect");
    IoT_Error_t rc = FAILURE;

    if(pClient == NULL) {
        return;
    }

    if(aws_iot_is_autoreconnect_enabled(pClient)) {
        ESP_LOGI(tag, "Auto Reconnect is enabled, Reconnecting attempt will start now");
    } else {
        ESP_LOGW(tag, "Auto Reconnect not enabled. Starting manual reconnect...");
        rc = aws_iot_mqtt_attempt_reconnect(pClient);
        if(NETWORK_RECONNECTED == rc) {
            ESP_LOGW(tag, "Manual Reconnect Successful");
        } else {
            ESP_LOGW(tag, "Manual Reconnect Failed - %d", rc);
        }
    }
}

void get_data(void *param) {
    IoT_Error_t rc = FAILURE;

    AWS_IoT_Client client;
    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

    ESP_LOGI(tag, "AWS IoT SDK Version %d.%d.%d-%s", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

    mqttInitParams.enableAutoReconnect = false; // We enable this later below
    mqttInitParams.pHostURL = HostAddress;
    mqttInitParams.port = port;    
    mqttInitParams.pRootCALocation = (const char *)aws_root_ca_pem_start;
    mqttInitParams.pDeviceCertLocation = "#";
    mqttInitParams.pDevicePrivateKeyLocation = "#0";
    
#define CLIENT_ID_LEN (ATCA_SERIAL_NUM_SIZE * 2)

    char *client_id = malloc(CLIENT_ID_LEN + 1);
    ATCA_STATUS ret = Atecc608_GetSerialString(client_id);
    if (ret != ATCA_SUCCESS)
    {
        printf("Failed to get device serial from secure element. Error: %i", ret);
        abort();
    }

    char* subscribe_topic = "stockly/AMZN";

    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 5000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = disconnect_callback_handler;
    mqttInitParams.disconnectHandlerData = NULL;

    rc = aws_iot_mqtt_init(&client, &mqttInitParams);
    if(SUCCESS != rc) {
        ESP_LOGE(tag, "aws_iot_mqtt_init returned error : %d ", rc);
        abort();
    }

    /* Wait for WiFI to show as connected */
    initialise_wifi();
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, 0, 1, 300);    

    connectParams.keepAliveIntervalInSec = 10;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;

    connectParams.pClientID = client_id;
    connectParams.clientIDLen = CLIENT_ID_LEN;
    connectParams.isWillMsgPresent = false;
    ESP_LOGI(tag, "Connecting to AWS IoT Core at %s:%d", mqttInitParams.pHostURL, mqttInitParams.port);
    do {
        rc = aws_iot_mqtt_connect(&client, &connectParams);
        if(SUCCESS != rc) {
            ESP_LOGE(tag, "Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    } while(SUCCESS != rc);
    ESP_LOGI(tag, "Successfully connected to AWS IoT Core!");

    while(true) {
        vTaskDelay(1000);
    }
}

void app_main() {
    Core2ForAWS_Init();

    Core2ForAWS_Display_SetBrightness(100);

    amzn_setup();

    lv_init();

    LV_FONT_DECLARE(jetbrains_mono_bold_70px)
    static lv_style_t st;
    lv_style_init(&st);
    lv_style_set_text_font(&st, LV_STATE_DEFAULT, &jetbrains_mono_bold_70px);

    lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_recolor(label1, true);
    lv_label_set_text(label1, "#FF0000 Stockly");
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, -115, -80);
    lv_obj_add_style(label1, LV_LABEL_PART_MAIN , &st);
    vTaskDelay(pdMS_TO_TICKS(1000));
    lv_obj_del(label1);

    xTaskCreatePinnedToCore(&get_data, "get_data", 4096 * 2, NULL, 5, NULL, 1);
}