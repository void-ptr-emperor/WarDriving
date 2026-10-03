#include "oled.h"
#include "menu.h"
#include <stddef.h>
#include "task.h"
#include <stdbool.h>
#include <string.h>
#include "esp_vfs_fat.h"
#include "esp_pm.h"



static uint8_t real_frequency_now = 80;
static uint8_t real_brightness_now = OLED_PARAM_CONTRAST_25;

uint8_t selected_index = 0;


uint8_t menu_history_index = 0;
menu_item_t *menu_history_array[MAX_MENU_DEPTH] = {0};


const char *auth_names[] = {
    "OPEN",            // 0
    "WEP",             // 1
    "WPA",             // 2
    "WPA2",            // 3
    "WPA/WPA2",        // 4
    "ENTERPRISE",      // 5
    "WPA3",            // 6
    "WPA2/WPA3",       // 7
    "WAPI",            // 8
    "OWE",             // 9
    "WPA3 ENT 192",    // 10
    "WPA3 EXT",        // 11
    "WPA3 EXT MIX",    // 12
    "DPP",             // 13
    "WPA3 ENT",        // 14
    "WPA2/WPA3 ENT",   // 15
};

uint32_t frequency_values[] = {

    80,
    160,
    240,

};

uint8_t brightness_values[] = {

    OLED_PARAM_CONTRAST_25,
    OLED_PARAM_CONTRAST_50,
    OLED_PARAM_CONTRAST_75,
    OLED_PARAM_CONTRAST_100,
    
};




char mhz_80_ststus_str[16];
char mhz_160_ststus_str[16];
char mhz_240_ststus_str[16];
char sleep_status_str[16] = "OFF";

char brightness_low_status_str[16];
char brightness_mid_status_str[16];
char brightness_high_status_str[16];
char brightness_max_status_str[16];

char tasks_status_str[16] = "ON";

menu_item_t wifi_live_struct[] = {
    {"WIFI LIVE MENU" , "NETWORKS:" ,  NULL , networks_str  ,  NULL ,  3 ,  false},
    {"WIFI LIVE MENU" , "BEST:" ,      NULL , best_wifi_str ,  NULL ,  3 ,  false},
    {"WIFI LIVE MENU" , "RSSI:" ,      NULL , best_rssi_str ,  NULL ,  3 ,  false},
    {"WIFI LIVE MENU" , "AUTH NAME:" , NULL , auth_name_str , NULL , 3 ,  false},
};

menu_item_t gps_live_struct[] = {
    {"GPS LIVE MENU" , "LAT:" , NULL , gps_lat_str , NULL , 3 , false},
    {"GPS LIVE MENU" , "LON:" , NULL , gps_lon_str , NULL , 3 , false},
    {"GPS LIVE MENU" , "ALT:" , NULL , gps_alt_str , NULL , 3 , false},
    {"GPS LIVE MENU" , "FIX:" , NULL , gps_fix_str , NULL , 3 , false},

};

menu_item_t sd_status_struct[] = {
    {"SD STATUS MENU" , "CARD:" ,   NULL , card_status_str ,   NULL ,  3 , false},
    {"SD STATUS MENU" , "RECORDS:", NULL , card_records_str ,  NULL,   3 , false},
    {"SD STATUS MENU" , "SIZE" ,    NULL , card_size_str ,     NULL ,  3 , false},
    {"SD STATUS MENU" , "FREE:" ,   NULL , card_free_str ,     NULL ,  3 , false},
};




menu_item_t esp32_settings_select[] = {
    {"FREQUENCY MENU" , "80 MHz:" ,     NULL , mhz_80_ststus_str ,  select_frequency ,      3 , true},
    {"FREQUENCY MENU" , "160 MHz:" ,    NULL , mhz_160_ststus_str , select_frequency ,      3 , true},
    {"FREQUENCY MENU" , "240 MHz:" ,    NULL , mhz_240_ststus_str , select_frequency ,      3 , true},
    {"FREQUENCY MENU" , "SLEEP MODE:" , NULL , sleep_status_str ,   select_esp_sleep_mode , 3 , true},

};

menu_item_t oled_settings_select[] = {
    {"BRIGHTNESS MENU" , "LOW:" ,  NULL , brightness_low_status_str ,  select_brightness ,  3 ,  true},
    {"BRIGHTNESS MENU" , "MID:" ,  NULL , brightness_mid_status_str ,  select_brightness ,  3 ,  true},
    {"BRIGHTNESS MENU" , "HIGH:" , NULL , brightness_high_status_str , select_brightness ,  3 , true},
    {"BRIGHTNESS MENU" , "MAX:" ,  NULL , brightness_max_status_str ,  select_brightness ,  3 , true}

};

menu_item_t tasks_status_menu[] = {
    {"SET TASKS STATUS" , "START:" , NULL , tasks_status_str, select_tasks_status , 1 , true},
};



menu_item_t other_menu[] = {
    {"OTHER MENU" , "ESP32 SETTINGS", esp32_settings_select , NULL , NULL ,  2 , false},
    {"OTHER MENU" , "OLED SETTINGS",  oled_settings_select ,  NULL , NULL ,  2 , false},
    {"OTHER MENU" , "MODULE STATUS",  tasks_status_menu ,     NULL , NULL ,  2 , false},
    
};



menu_item_t main_menu[] = {

    {"MAIN MENU" , "WIFI LIVE" ,  wifi_live_struct ,  NULL ,  NULL , 3 , false},
    {"MAIN MENU" , "GPS LIVE" ,   gps_live_struct ,   NULL ,  NULL , 3 , false},
    {"MAIN MENU" , "SD STATUS" ,  sd_status_struct ,  NULL ,  NULL , 3 , false},
    {"MAIN MENU" , "OTHER MENU" , other_menu ,        NULL ,  NULL , 3 , false},
};


menu_item_t *ptr_menu =  main_menu;


const char *get_auth_name(uint8_t mode_index)
{
    if(mode_index >= sizeof(auth_names) / sizeof(auth_names[0])){
        return "UNKNOWN";
    }

    return auth_names[mode_index];
}

void show_menu(void)
{

    uint8_t size = ptr_menu->size +1;


    memset_oled_buffer();

    write_text_pixel(ptr_menu[0].menu_name , (((OLED_LENGTH_WITH_FRAME / 2) - (strlen(ptr_menu[0].menu_name) * FONT_WIDTH ) / 2) + 1) , START_MENU_NAME_Y);

    for(int i = 0 ; i < size ; i++)
    {
        write_text_pixel(ptr_menu[i].name , X_START_POINT , 19 + (12 * i));

        if(ptr_menu[i].dyn_data != NULL)
        {
            write_text_pixel(
                ptr_menu[i].dyn_data,
                X_END_POINT - strlen(ptr_menu[i].dyn_data) * FONT_WIDTH,
                19 + (12 * i));
        }


    }

        write_frames();
        write_invert_square (selected_index);
        oled_begin();

}





void select_frequency(func_arg_t arg)
{

    real_frequency_now = (arg.int8_value == -1) ? frequency_values[selected_index] : arg.int8_value;

    esp_pm_config_t pm_config = {

        .max_freq_mhz = real_frequency_now,
        .min_freq_mhz = real_frequency_now,
        .light_sleep_enable = false,

    };

    esp_pm_configure(&pm_config);

    snprintf(mhz_80_ststus_str , sizeof(mhz_80_ststus_str) , "%s" , real_frequency_now == 80 ? "ON" : "OFF");
    snprintf(mhz_160_ststus_str , sizeof(mhz_160_ststus_str) , "%s" ,  real_frequency_now == 160 ? "ON" : "OFF");
    snprintf(mhz_240_ststus_str , sizeof(mhz_240_ststus_str) , "%s" , real_frequency_now == 240 ? "ON" : "OFF");
 
}

void select_brightness(func_arg_t arg)
{
    real_brightness_now = (arg.int8_value == -1) ? brightness_values[selected_index] : arg.int8_value;

    oled_send_cmd(OLED_CMD_SET_CONTRAST);
    oled_send_cmd(real_brightness_now);

    snprintf(brightness_low_status_str , sizeof(brightness_low_status_str) , "%s" , real_brightness_now == OLED_PARAM_CONTRAST_25 ? "ON" : "OFF");
    snprintf(brightness_mid_status_str , sizeof(brightness_mid_status_str) , "%s" , real_brightness_now == OLED_PARAM_CONTRAST_50 ? "ON" : "OFF");
    snprintf(brightness_high_status_str , sizeof(brightness_high_status_str) , "%s" , real_brightness_now == OLED_PARAM_CONTRAST_75 ? "ON" : "OFF");
    snprintf(brightness_max_status_str , sizeof(brightness_max_status_str) , "%s" , real_brightness_now == OLED_PARAM_CONTRAST_100 ? "ON" : "OFF");

}


void select_esp_sleep_mode(func_arg_t arg)
{
    static bool sleep_flag = false;

    sleep_flag = !sleep_flag;

    esp_pm_config_t sleep_pm_config = {
        .max_freq_mhz = real_frequency_now,
        .min_freq_mhz = real_frequency_now,
        .light_sleep_enable = sleep_flag,
    };

    esp_pm_configure(&sleep_pm_config);

    snprintf(sleep_status_str , sizeof(sleep_status_str) , "%s" ,  sleep_flag ? "ON" : "OFF");

}

void select_tasks_status(func_arg_t arg)
{
    tasks_status_flag = !tasks_status_flag;

    if(!tasks_status_flag)
    {
        snprintf(networks_str , sizeof(networks_str)   , "SUSPENDED");
        snprintf(best_wifi_str , sizeof(best_wifi_str) , "SUSPENDED");
        snprintf(best_rssi_str , sizeof(best_rssi_str) , "SUSPENDED");
        snprintf(auth_name_str , sizeof(auth_name_str) , "SUSPENDED");

        snprintf(gps_lat_str , sizeof(gps_lat_str) , "SUSPENDED");
        snprintf(gps_lon_str , sizeof(gps_lon_str) , "SUSPENDED");
        snprintf(gps_alt_str , sizeof(gps_alt_str) , "SUSPENDED");
        snprintf(gps_fix_str , sizeof(gps_fix_str) , "SUSPENDED");

        snprintf(card_status_str , sizeof(card_status_str)   ,"SUSPENDED");
        snprintf(card_records_str , sizeof(card_records_str) ,"SUSPENDED");
        snprintf(card_size_str , sizeof(card_size_str)       ,"SUSPENDED");
        snprintf(card_free_str , sizeof(card_free_str)       ,"SUSPENDED");

    }
    else
    {
        snprintf(networks_str , sizeof(networks_str)   , "STARTING");
        snprintf(best_wifi_str , sizeof(best_wifi_str) , "STARTING");
        snprintf(best_rssi_str , sizeof(best_rssi_str) , "STARTING");
        snprintf(auth_name_str , sizeof(auth_name_str) , "STARTING");

        snprintf(gps_lat_str , sizeof(gps_lat_str) , "STARTING");
        snprintf(gps_lon_str , sizeof(gps_lon_str) , "STARTING");
        snprintf(gps_alt_str , sizeof(gps_alt_str) , "STARTING");
        snprintf(gps_fix_str , sizeof(gps_fix_str) , "STARTING");

        snprintf(card_status_str , sizeof(card_status_str)   ,"STARTING");
        snprintf(card_records_str , sizeof(card_records_str) ,"STARTING");
        snprintf(card_size_str , sizeof(card_size_str)       ,"STARTING");
        snprintf(card_free_str , sizeof(card_free_str)       ,"STARTING");   
    }


    snprintf(tasks_status_str , sizeof(tasks_status_str) , "%s" ,  tasks_status_flag ? "ON" : "OFF");

}



void all_settings_init()
{

    func_arg_t settings_args;

    settings_args.int8_value = frequency_values[0];
    select_frequency(settings_args);

    settings_args.int8_value = brightness_values[0];
    select_brightness(settings_args);

}