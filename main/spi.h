#pragma once
#include "driver/spi_master.h"
#include <stdint.h>

#define SPI_MOSI_PIN            23 
#define SPI_CLK_PIN             18
#define SPI_MISO_PIN            19

#define SPI_OLED_DC_PIN         2
#define SPI_OLED_RES_PIN        4
#define SPI_OLED_CS_PIN         5


extern spi_device_handle_t oled_handle;


void dc_pin(spi_transaction_t *t);
void spi_bus_config(void);
void spi_oled_config(void);
void spi_transaction_oled(uint8_t *pointer, int dc_position , size_t size);
