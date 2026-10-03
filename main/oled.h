#pragma once
#include <stdint.h>
#include <stdbool.h>


#define X_START_POINT 2 
#define X_END_POINT 125
#define CENTRAL_OF_OLED 60
#define START_MENU_NAME_Y 5
#define OLED_LENGTH_WITH_FRAME 120
#define FONT_WIDTH 6


#define OLED_PARAM_CONTRAST_25       0x40
#define OLED_PARAM_CONTRAST_50       0x80
#define OLED_PARAM_CONTRAST_75       0xC0
#define OLED_PARAM_CONTRAST_100      0xFF

#define OLED_CMD_DISPLAY_OFF         0xAE
#define OLED_CMD_DISPLAY_ON          0xAF
#define OLED_CMD_ENABLE_CHARGE_PUMP  0x8D
#define OLED_CMD_SET_CONTRAST        0x81
#define OLED_CMD_SET_MUX_RATIO       0xA8
#define OLED_CMD_SET_DISPLAY_OFFSET  0xD3
#define OLED_CMD_SET_START_LINE      0x40
#define OLED_CMD_SET_SEGMENT_REMAP   0xA1
#define OLED_CMD_SET_COM_SCAN_DIR    0xC8
#define OLED_CMD_SET_COM_PINS        0xDA
#define OLED_CMD_MEMORY_ADDR_MODE    0x20
#define OLED_CMD_SET_OSC_FREQ        0xD5
#define OLED_CMD_DISABLE_ENTIRE_ON   0xA4
#define OLED_CMD_SET_NORMAL_DISPLAY  0xA6
#define OLED_PARAM_MUX_RATIO_64      0x3F
#define OLED_PARAM_OFFSET_ZERO       0x00
#define OLED_PARAM_COM_PINS_ALT      0x12
#define OLED_PARAM_OSC_FREQ_DEFAULT  0x80
#define OLED_PARAM_CHARGE_PUMP_ON    0x14
#define OLED_PARAM_ADDR_MODE_HORIZ   0x00


void memset_oled_buffer(void);
void write_pixel_in_oled_buffer(uint8_t x, uint8_t y, bool position);
void write_text_pixel(const char *text , uint8_t x_start , uint8_t y_start);
void write_h_line_in_oled_buffer(uint8_t x_start , uint8_t x_end , uint8_t y );
void write_v_line_in_oled_buffer(uint8_t x , uint8_t y_start , uint8_t y_end );
void write_invert_pixel(uint8_t x, uint8_t y);
void write_invert_area(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end);
void write_invert_square(uint8_t square_index); 
void write_frames(void);
void oled_begin(void);
void oled_set_default_settings(void);
void oled_send_cmd(uint8_t cmd);
