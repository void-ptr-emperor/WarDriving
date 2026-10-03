#include "uart.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>


uint8_t gps_data[128];


const char *disable_gll = "$PUBX,40,GLL,0,0,0,0,0,0*5C\r\n";
const char *disable_gsa = "$PUBX,40,GSA,0,0,0,0,0,0*4E\r\n";
const char *disable_gsv = "$PUBX,40,GSV,0,0,0,0,0,0*59\r\n";
const char *disable_rmc = "$PUBX,40,RMC,0,0,0,0,0,0*47\r\n";
const char *disable_vtg = "$PUBX,40,VTG,0,0,0,0,0,0*5E\r\n";


void uart_gps_init()
{
    
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
       .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_param_config(UART_NUM_2 ,&uart_config);
    uart_set_pin(UART_NUM_2 , UART_TX , UART_RX , UART_PIN_NO_CHANGE , UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_2 , 1024 , 0 , 0 , NULL , 0);

}

uint8_t read_gps()
{
  
    uint8_t byte = 0;
    uint8_t index = 0;

    while(byte != '\n' && index < sizeof(gps_data) - 1){
        

        int data = uart_read_bytes(UART_NUM_2 , &byte , 1 , pdMS_TO_TICKS(100));

            if(data <= 0){
                return 0;
            }
            
        gps_data[index] = byte;
        index++;

    }

    gps_data[index] = '\0';

    return index;
}

void gps_init()
{
    uart_write_bytes(UART_NUM_2, disable_gll, strlen(disable_gll));
    vTaskDelay(pdMS_TO_TICKS(100));
    uart_write_bytes(UART_NUM_2, disable_gsa, strlen(disable_gsa));
    vTaskDelay(pdMS_TO_TICKS(100));
    uart_write_bytes(UART_NUM_2, disable_gsv, strlen(disable_gsv));
    vTaskDelay(pdMS_TO_TICKS(100));
    uart_write_bytes(UART_NUM_2, disable_rmc, strlen(disable_rmc));
    vTaskDelay(pdMS_TO_TICKS(100));
    uart_write_bytes(UART_NUM_2, disable_vtg, strlen(disable_vtg));
    vTaskDelay(pdMS_TO_TICKS(100));
}