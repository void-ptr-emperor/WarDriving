#include  "isr.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_attr.h"
#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "task.h"



static uint64_t last_time_up = 0;
static uint64_t last_time_ok = 0;
static uint64_t last_time_down = 0;
static uint64_t last_time_back = 0;



void IRAM_ATTR btn_up_isr(void *arg)
{
    uint64_t now = esp_timer_get_time();

    if(now - last_time_up > 200 * 1000){
        last_time_up = now;

        BaseType_t woken = pdFALSE;
        vTaskNotifyGiveFromISR(btn_up_task_handle , &woken);
        if(woken){
            portYIELD_FROM_ISR();
        }
    }
}

void IRAM_ATTR btn_ok_isr(void *arg)
{
    uint64_t now = esp_timer_get_time();

    if(now - last_time_ok > 200 * 1000){
        last_time_ok = now;

        BaseType_t woken = pdFALSE;
        vTaskNotifyGiveFromISR(btn_ok_task_handle , &woken);
        if(woken){
            portYIELD_FROM_ISR();
        }
    }
}

void IRAM_ATTR btn_down_isr(void *arg)
{
    uint64_t now = esp_timer_get_time();

    if(now - last_time_down > 200 *1000){
        last_time_down = now;

        BaseType_t woken = pdFALSE;
        vTaskNotifyGiveFromISR(btn_down_task_handle , &woken);
        if(woken){
            portYIELD_FROM_ISR();
        }
    }
}

void IRAM_ATTR btn_back_isr(void *arg)
{
    uint64_t now = esp_timer_get_time();
    if(now - last_time_back > 200 * 1000){
        last_time_back = now;

        BaseType_t woken = pdFALSE;
        vTaskNotifyGiveFromISR(btn_back_task_handle , &woken);
        if(woken){
            portYIELD_FROM_ISR();
        }
    }
}




void all_isr_initialize(void)
{

    gpio_config_t isr_gpio_config = {

        .pin_bit_mask = (1ULL << ISR_DOWN_PIN) | 
                        (1ULL << ISR_OK_PIN) | 
                        (1ULL << ISR_UP_PIN) | 
                        (1ULL << ISR_BACK_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE

    };


    gpio_config(&isr_gpio_config);

    
    gpio_install_isr_service(0);

    gpio_isr_handler_add(ISR_DOWN_PIN , btn_down_isr , NULL);
    gpio_isr_handler_add(ISR_OK_PIN , btn_ok_isr , NULL);
    gpio_isr_handler_add(ISR_UP_PIN , btn_up_isr , NULL);
    gpio_isr_handler_add(ISR_BACK_PIN , btn_back_isr , NULL);



}
