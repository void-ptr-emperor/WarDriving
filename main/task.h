#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "menu.h"
#include "esp_wifi.h"
#include "freertos/semphr.h"    
#include "freertos/queue.h"  


#define MAX_WIFI_NETWORKS 20

typedef struct {
    char    ssid[33];
    int8_t  rssi;
    uint8_t bssid[6];
    char authmode[16];
    char    lat[16];
    char    lon[16];
    char    alt[16];
    char    fix[16];
} record_t;

extern volatile bool tasks_status_flag;



extern QueueHandle_t record_queue;
extern SemaphoreHandle_t ui_mutex;


extern TaskHandle_t btn_up_task_handle;
extern TaskHandle_t btn_ok_task_handle;
extern TaskHandle_t btn_down_task_handle;
extern TaskHandle_t btn_back_task_handle;

extern TaskHandle_t wifi_task_handle;
extern TaskHandle_t gps_task_handle;
extern TaskHandle_t sd_task_handle;

extern TaskHandle_t ui_task_handle;



extern char networks_str[16];
extern char best_wifi_str[16];
extern char best_rssi_str[16];
extern char auth_name_str[16];


extern char gps_lat_str[16];    
extern char gps_lon_str[16]; 
extern char gps_alt_str[16];
extern char gps_fix_str[16];   


extern char card_status_str[16];
extern char card_records_str[16];
extern char card_size_str[16];
extern char card_free_str[16];


void btn_up_task(void *param);
void btn_ok_task(void *param);
void btn_down_task(void *param);
void btn_back_task(void *param);

void wifi_task(void *param);
void gps_task(void *param);
void sd_task(void *param);

void ui_task(void *param);
