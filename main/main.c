#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_mac.h"
#include "spi.h"
#include "oled.h"
#include "sd.h"
#include "task.h"
#include "isr.h"
#include "menu.h"
#include "freertos/queue.h"
#include "web.h"



void app_main(void)
{
 
    record_queue = xQueueCreate(20 , sizeof(record_t));
    ui_mutex = xSemaphoreCreateMutex();
    
    spi_bus_config();
    spi_oled_config();
    oled_set_default_settings();
    
    show_menu();


    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_config);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    uart_gps_init();
    gps_init();

    sd_init();
    web_init();

    all_settings_init();

    xTaskCreate(ui_task , "UI_TASK" , 4096 , NULL , 2 , &ui_task_handle);

    xTaskCreate(btn_up_task ,"UP_TASK" , 4096 , NULL , 2 , &btn_up_task_handle);
    xTaskCreate(btn_ok_task ,"OK_TASK" , 4096 , NULL , 2 , &btn_ok_task_handle);
    xTaskCreate(btn_down_task ,"DOWN_TASK" , 4096 , NULL , 2 , &btn_down_task_handle);
    xTaskCreate(btn_back_task ,"BACK_TASK" , 4096 , NULL , 2 , &btn_back_task_handle);

    xTaskCreate(wifi_task , "WIFI_TASK" , 4096 , NULL , 1  , &wifi_task_handle);
    xTaskCreate(gps_task , "GPS_TASK" , 4096 , NULL , 1 , &gps_task_handle);
    xTaskCreate(sd_task , "SD_TASK" , 4096 , NULL , 1  , &sd_task_handle);




    all_isr_initialize();

}

















