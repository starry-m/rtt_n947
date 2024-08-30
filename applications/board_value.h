#ifndef __BOARD_VALUE_H__
#define __BOARD_VALUE_H__

#include <rtthread.h>

typedef struct {
    uint16_t sensor_Color_r;
    uint16_t sensor_Color_g; 
    uint16_t sensor_Color_b;
    uint16_t sensor_Color_c;
    float temp;
    uint8_t touched;
    uint8_t key_pressed;
}app_board_value_send_t;

typedef struct {
    uint8_t led_r;
    uint8_t led_g;
    uint8_t led_b;
}app_board_value_recv_t;

#endif

