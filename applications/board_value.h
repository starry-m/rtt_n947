#ifndef __BOARD_VALUE_H__
#define __BOARD_VALUE_H__

#include "stdint.h"
#include "stdio.h"

typedef struct {
    uint16_t sensor_Color_r;
    uint16_t sensor_Color_g; 
    uint16_t sensor_Color_b;
    uint16_t sensor_Color_c;
    float temperature;
    uint8_t touched;
    uint8_t key_pressed;
}app_board_value_send_t;

typedef struct {
    uint8_t led_status;
}led_topic_t;

typedef struct {
    uint16_t sensor_Color_r;
    uint16_t sensor_Color_g; 
    uint16_t sensor_Color_b;
    uint16_t sensor_Color_c;
    float temperature;
} apds_temp_topic_t;

typedef struct {
    uint8_t pressed;
} touch_topic_t;
typedef struct {
    uint8_t pressed;
} key_topic_t;
#endif

