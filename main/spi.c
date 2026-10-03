#include "spi.h"
#include "driver/gpio.h"
#include <string.h>
#include "esp_err.h"


spi_device_handle_t oled_handle;



void dc_pin(spi_transaction_t *t)
{
    
     gpio_set_level(SPI_OLED_DC_PIN , (int)(intptr_t)t->user);
    
}

void spi_bus_config()
{

    gpio_set_direction(SPI_OLED_DC_PIN, GPIO_MODE_OUTPUT);


    spi_bus_config_t bus_config = {

        .mosi_io_num = SPI_MOSI_PIN,
        .miso_io_num = SPI_MISO_PIN,
        .sclk_io_num = SPI_CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 0
    };

    spi_bus_initialize(SPI2_HOST , &bus_config , SPI_DMA_CH_AUTO);

}

void spi_oled_config()
{
 
    spi_device_interface_config_t device_config = {

        .clock_speed_hz = 10 * 1000 *1000,
        .mode = 0,
        .spics_io_num = SPI_OLED_CS_PIN,
        .queue_size = 7,
        .pre_cb = dc_pin
    }; 

    spi_bus_add_device(SPI2_HOST , &device_config , &oled_handle);

}

void spi_transaction_oled(uint8_t *pointer, int dc_position , size_t size)
{

    spi_transaction_t transaction;

    memset(&transaction , 0 , sizeof(transaction));

    transaction.length = size * 8;
    transaction.tx_buffer = pointer;
    transaction.user = (void*)(intptr_t)dc_position;

    spi_device_polling_transmit(oled_handle , &transaction);

}

