#include "uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include <stdio.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_mac.h"
#include "menu.h"
#include "task.h"
#include "oled.h"
#include <stdbool.h>
#include "sd.h"
#include "esp_vfs_fat.h"
#include "freertos/queue.h"
#include <string.h>
#include "freertos/semphr.h"    

volatile bool tasks_status_flag = true;


QueueHandle_t record_queue;
SemaphoreHandle_t ui_mutex;


static uint16_t wifi_count = 0;
static wifi_ap_record_t ap_records[MAX_WIFI_NETWORKS];


static uint32_t card_records_count = 0;

TaskHandle_t btn_up_task_handle = NULL;
TaskHandle_t btn_ok_task_handle = NULL;
TaskHandle_t btn_down_task_handle = NULL;
TaskHandle_t btn_back_task_handle = NULL;

TaskHandle_t wifi_task_handle = NULL;
TaskHandle_t gps_task_handle = NULL;
TaskHandle_t sd_task_handle  = NULL;

TaskHandle_t ui_task_handle;

char networks_str[16];
char best_wifi_str[16];
char best_rssi_str[16];
char auth_name_str[16];


char gps_lat_str[16];    
char gps_lon_str[16]; 
char gps_alt_str[16];
char gps_fix_str[16];   


char card_status_str[16];
char card_records_str[16];
char card_size_str[16];
char card_free_str[16];



void btn_up_task(void *param)
{
    while(1){

        ulTaskNotifyTake(pdTRUE , portMAX_DELAY);

        xSemaphoreTake(ui_mutex , portMAX_DELAY);

        if(selected_index == 0){
            selected_index = ptr_menu->size;
        }
        else {
            selected_index--;
        }

        xSemaphoreGive(ui_mutex);

        xTaskNotifyGive(ui_task_handle);

    }

}

void btn_ok_task(void *param)
{
    while(1)
    {
        ulTaskNotifyTake(pdTRUE , portMAX_DELAY);

        xSemaphoreTake(ui_mutex , portMAX_DELAY);

        if(ptr_menu[selected_index].is_settings)
        {
            func_arg_t select_settings;
            select_settings.int8_value = -1;
            ptr_menu[selected_index].func(select_settings);
            
        }
        
        else if(ptr_menu[selected_index].pointer != NULL)
        {
            if(menu_history_index < MAX_MENU_DEPTH)
            {
                menu_history_array[menu_history_index] = ptr_menu;
                menu_history_index++;
                ptr_menu = ptr_menu[selected_index].pointer;
                selected_index = 0;
            }
        }

        xSemaphoreGive(ui_mutex);

        xTaskNotifyGive(ui_task_handle);
    }

}

void btn_down_task(void *param)
{
    while(1)
    {
        ulTaskNotifyTake(pdTRUE , portMAX_DELAY);

        xSemaphoreTake(ui_mutex , portMAX_DELAY);

            if(selected_index < ptr_menu->size){
                selected_index++;
            }
            else {
                selected_index =0;
            }

        xSemaphoreGive(ui_mutex);

        xTaskNotifyGive(ui_task_handle);
    }

}

void btn_back_task(void *param)
{
    while(1)
    {
        ulTaskNotifyTake(pdTRUE , portMAX_DELAY);
        
        xSemaphoreTake(ui_mutex , portMAX_DELAY);

            if(menu_history_index > 0){
                menu_history_index--;
                ptr_menu = menu_history_array[menu_history_index];
                selected_index = 0 ;
            }

        xSemaphoreGive(ui_mutex);

        xTaskNotifyGive(ui_task_handle);

    }

}




void wifi_task(void *param)
{   
    while(1)
    {

        if(tasks_status_flag)
        {

        esp_wifi_scan_start(NULL , true);

        uint16_t wifi_found = 0;

        esp_wifi_scan_get_ap_num(&wifi_found);

        wifi_count = MAX_WIFI_NETWORKS;

        esp_wifi_scan_get_ap_records(&wifi_count , ap_records);

        esp_wifi_clear_ap_list(); 

        xSemaphoreTake(ui_mutex , portMAX_DELAY);

        if(wifi_count == 0){
            snprintf(networks_str , sizeof(networks_str) , "NOT FOUND");
            snprintf(best_wifi_str , sizeof(best_wifi_str) , "ERROR");
            snprintf(best_rssi_str , sizeof(best_rssi_str) , "ERROR");
            snprintf(auth_name_str , sizeof(auth_name_str) , "ERROR");

        }

        else {


            int best_wifi_index = 0;
            int8_t best_rssi_value = ap_records[0].rssi;

            for(int i = 1 ; i < wifi_count ; i++)
            {
                if(ap_records[i].rssi > best_rssi_value)
                {
                   best_rssi_value = ap_records[i].rssi;
                   best_wifi_index = i;
                }
            }

            snprintf(networks_str , sizeof(networks_str) , "%d" , wifi_count);
            snprintf(best_wifi_str , sizeof(best_wifi_str) , "%.15s" , (char*)ap_records[best_wifi_index].ssid);
            snprintf(best_rssi_str , sizeof(best_rssi_str) , "%d" , ap_records[best_wifi_index].rssi);
            snprintf(auth_name_str , sizeof(auth_name_str) , "%s" , get_auth_name(ap_records[best_wifi_index].authmode));

            }

            char lon[16], lat[16], alt[16], fix[16];
            memcpy(lat, gps_lat_str, 16);
            memcpy(lon, gps_lon_str, 16);
            memcpy(alt, gps_alt_str, 16);
            memcpy(fix, gps_fix_str, 16);

            xSemaphoreGive(ui_mutex);

            for(int i = 0; i < wifi_count; i++)
            {
                record_t rec = {0};

                snprintf(rec.ssid , sizeof(rec.ssid) , "%s" , (char*)ap_records[i].ssid);
                rec.rssi = ap_records[i].rssi;
                memcpy(rec.bssid , ap_records[i].bssid , 6);
                snprintf(rec.authmode , sizeof(rec.authmode) , "%s" , get_auth_name(ap_records[i].authmode));
                memcpy(rec.lat , lat , sizeof(lat));
                memcpy(rec.lon , lon , sizeof(lon));
                memcpy(rec.alt , alt , sizeof(alt));
                memcpy(rec.fix , fix , sizeof(fix));

                xQueueSend(record_queue , &rec , 0);

            }

    xTaskNotifyGive(ui_task_handle);

    }

    vTaskDelay(pdMS_TO_TICKS(1000 * 5));

    }

}

void gps_task(void *param)
{

    while(1)
    {
        if(tasks_status_flag == true)
        {
            uint8_t len = read_gps();

            if(len > 0)
            {
                            char *field[16] = {0};
            int n = 0;
            char *p = (char*)gps_data;

            field[n++] = p;
            while (*p != '\0' && n < 16) {
                if (*p == ',') {
                    *p = '\0';
                    field[n++] = p + 1;
                }
                p++;
            }

            char lat[16] = {0};
            char lon[16] = {0};
            char alt[16] = {0};
            char fix[16] = {0};

            if (n >= 10) {
                snprintf(lat, sizeof(lat), "%.15s", field[2]);
                snprintf(lon, sizeof(lon), "%.15s", field[4]);
                snprintf(alt, sizeof(alt), "%.15s", field[9]);
                snprintf(fix, sizeof(fix), "%.15s", field[6]);
            }
                xSemaphoreTake(ui_mutex , portMAX_DELAY);

                if(fix[0] == '0')
                {
                    snprintf(gps_lat_str , sizeof(gps_lat_str) , "NOT FOUND");
                    snprintf(gps_lon_str , sizeof(gps_lon_str) , "NOT FOUND");
                    snprintf(gps_alt_str , sizeof(gps_alt_str) , "NOT FOUND");
                    snprintf(gps_fix_str , sizeof(gps_fix_str) , "ERROR");
                }
                else
                {
                    snprintf(gps_lat_str , sizeof(gps_lat_str) , "%s" , lat);
                    snprintf(gps_lon_str , sizeof(gps_lon_str) , "%s" , lon);
                    snprintf(gps_alt_str , sizeof(gps_alt_str) , "%s" , alt);
                    snprintf(gps_fix_str , sizeof(gps_fix_str) , "%s" , fix);
                }

                xSemaphoreGive(ui_mutex);

                xTaskNotifyGive(ui_task_handle);
            }
        }
        else{
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}


    
void sd_task(void *param)
{
    while(1)
    {
        if(tasks_status_flag)
        {

        record_t rec;

        if(xQueueReceive(record_queue , &rec , pdMS_TO_TICKS(1000 * 5)) == pdTRUE)
        {

            FILE *ptr_file = fopen("/main/data.csv" , "a");

            if(ptr_file != NULL)
            {

                int result = fprintf(ptr_file , "%s,%d,%02x:%02x:%02x:%02x:%02x:%02x,%s,%s,%s,%s,%s\n",
                    rec.ssid,
                    rec.rssi,
                    rec.bssid[0],
                    rec.bssid[1],
                    rec.bssid[2],
                    rec.bssid[3],
                    rec.bssid[4],
                    rec.bssid[5],
                    rec.authmode,
                    rec.lat,
                    rec.lon,
                    rec.alt,
                    rec.fix);

                    if(result > 0){
                        card_records_count++;
                    }

                fclose(ptr_file);
                
            }

        }


    static bool first_call = true;
    static uint64_t last_time_card = 0;
    uint64_t now_time_card = esp_timer_get_time();

    char size_buff[16];
    char free_buff[16];
    bool update_flag = false;

        if(is_mounted == ESP_OK && (first_call || now_time_card - last_time_card > 60000000))
        {   
            update_flag = true;

            first_call = false;
            last_time_card = now_time_card;

            uint64_t card_total_bytes = 0;
            uint64_t card_free_bytes = 0;

        
            esp_err_t fat_info_err = esp_vfs_fat_info("/main" , &card_total_bytes , &card_free_bytes);

            if(fat_info_err == ESP_OK && card_total_bytes > 0)
            {
                uint32_t card_total_gb = card_total_bytes / (1024 * 1024 * 1024);
                uint32_t card_free_percent = (card_free_bytes * 100) / card_total_bytes;


                snprintf(size_buff , sizeof(size_buff) ,  "%luGB" , card_total_gb);
                snprintf(free_buff , sizeof(free_buff) ,  "%lu%%" , card_free_percent);

            }
            else
            {
                snprintf(size_buff , sizeof(size_buff) , "ERROR");
                snprintf(free_buff , sizeof(free_buff) , "ERROR");
            }

        }

    xSemaphoreTake(ui_mutex , portMAX_DELAY);

    if(is_mounted == ESP_OK)
    {
        snprintf(card_status_str , sizeof(card_status_str) ,  "%s" , "OK");
        snprintf(card_records_str , sizeof(card_records_str) ,  "%lu" , card_records_count);

        if (update_flag) {
            memcpy(card_size_str, size_buff, 16);
            memcpy(card_free_str, free_buff, 16);
        }
    }

    else{
        snprintf(card_status_str , sizeof(card_status_str) , "FAILED");
        snprintf(card_records_str , sizeof(card_records_str) ,"FAILED");
        snprintf(card_size_str , sizeof(card_size_str) , "FAILED");
        snprintf(card_free_str , sizeof(card_free_str) , "FAILED");
    }

    xSemaphoreGive(ui_mutex);

    xTaskNotifyGive(ui_task_handle);

    }

    else{
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    }
}




void ui_task(void *param)
{
    while(1)
    {    
        ulTaskNotifyTake(pdTRUE , pdMS_TO_TICKS(1000));

        xSemaphoreTake(ui_mutex , portMAX_DELAY);
        show_menu();
        xSemaphoreGive(ui_mutex);
    

    }

}

