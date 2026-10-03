#pragma once
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define UART_TX 17
#define UART_RX 16


extern uint8_t gps_data[128];


void uart_gps_init();
void gps_init();
uint8_t read_gps();