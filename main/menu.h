#pragma once
#include <stdbool.h>


#define MAX_MENU_DEPTH 2




typedef union {
    bool bool_value;
    int8_t int8_value;
    uint8_t uint8_value;
    int16_t int16_value;
    uint16_t uint16_value;
    int32_t int32_value;
    uint32_t uint32_value;
    float float_value;
    void *pointer;
} func_arg_t;


typedef struct menu_item_t {
    const char *menu_name;// 0
    const char *name;// 1
    struct menu_item_t *pointer;// 2
    char *dyn_data;// 3
    void (*func)(func_arg_t);// 4
    const uint8_t size;// 5
    bool is_settings;// 6
}menu_item_t;

extern const char *auth_names[];

extern menu_item_t *ptr_menu;


extern uint8_t selected_index;


extern uint8_t menu_history_index ;
extern menu_item_t *menu_history_array[MAX_MENU_DEPTH];


const char *get_auth_name(uint8_t mode_index);



void show_menu(void);




void select_frequency(func_arg_t arg);
void select_brightness(func_arg_t arg);

void select_esp_sleep_mode(func_arg_t arg);

void select_tasks_status(func_arg_t arg);



void all_settings_init();



