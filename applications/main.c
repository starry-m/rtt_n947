/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-10-24     Magicoe      first version
 * 2020-01-10     Kevin/Karl   Add PS demo
 * 2020-09-21     supperthomas fix the main.c
 *
 */

#include "drv_pin.h"
#include <rtdevice.h>
#include <rtthread.h>
#include <uMCN.h>
#include "board_value.h"
MCN_DEFINE(key_topic, sizeof(key_topic_t));
key_topic_t key_data;
static McnNode_t led_nod;
MCN_DECLARE(led_topic);

#define LEDR_PIN ((0 * 32) + 10)
#define LEDG_PIN ((0 * 32) + 27)
#define LEDB_PIN ((1 * 32) + 2)
#define BUTTON_PIN ((0 * 32) + 23)

static void sw_pin_cb(void *args);

static int  key_data_topic_echo(void *param)
{
    static  key_topic_t data;
    if (mcn_copy_from_hub((McnHub *)param, &data) == RT_EOK)
    {
         return 0;
    }
    return -1;
}
int main(void)
{
#if defined(__CC_ARM)
    rt_kprintf("using armcc, version: %d\n", __ARMCC_VERSION);
#elif defined(__clang__)
    rt_kprintf("using armclang, version: %d\n", __ARMCC_VERSION);
#elif defined(__ICCARM__)
    rt_kprintf("using iccarm, version: %d\n", __VER__);
#elif defined(__GNUC__)
    rt_kprintf("using gcc, version: %d.%d\n", __GNUC__, __GNUC_MINOR__);
#endif

    rt_pin_mode(LEDR_PIN, PIN_MODE_OUTPUT); /* Set GPIO as Output */
    rt_pin_mode(LEDG_PIN, PIN_MODE_OUTPUT); /* Set GPIO as Output */
    rt_pin_mode(LEDB_PIN, PIN_MODE_OUTPUT); /* Set GPIO as Output */
    rt_pin_write(LEDR_PIN, PIN_HIGH);
    rt_pin_write(LEDG_PIN, PIN_HIGH);
    rt_pin_write(LEDB_PIN, PIN_HIGH);
    rt_pin_mode(BUTTON_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(BUTTON_PIN, PIN_IRQ_MODE_FALLING, sw_pin_cb, RT_NULL);
    rt_pin_irq_enable(BUTTON_PIN, 1);

    rt_kprintf("MCXN947 HelloWorld\r\n");

#ifdef RT_USING_SDIO
    rt_thread_mdelay(2000);
    if (dfs_mount("sd", "/", "elm", 0, NULL) == 0)
    {
        rt_kprintf("sd mounted to /\n");
    }
    else
    {
        rt_kprintf("sd mount to / failed\n");
    }
#endif

    //    i2c_bus=rt_i2c_bus_device_find("i2c1");
    //     if (i2c_bus == RT_NULL)
    //    {
    //        rt_kprintf("can't find %s device!\n", "i2c1");
    //    }
    //    else
    //    {
    //
    //
    //    }
      mcn_advertise(MCN_HUB( key_topic),  key_data_topic_echo);
      led_nod = mcn_subscribe(MCN_HUB(led_topic), RT_NULL, RT_NULL);
      static  led_topic_t tled_data;
      rt_uint32_t count = 0;

    char *str;
    uint8_t last_key=0;
    uint8_t led_r=1,led_g=1,led_b=1;
    while (1)
    {
        key_data.pressed=!rt_pin_read(BUTTON_PIN);
        if(last_key !=key_data.pressed)
        {
            last_key =key_data.pressed;
            mcn_publish(MCN_HUB(key_topic), &key_data);	
        }
        if (mcn_poll(led_nod)){
        mcn_copy(MCN_HUB(led_topic), led_nod, &tled_data);
					rt_kprintf("[main]:led status=%d\r\n", tled_data.led_status);  
                    led_r= tled_data.led_status&0x4;
                    led_g= tled_data.led_status&0x2;
                    led_b= tled_data.led_status&0x1;
                    rt_pin_write(LEDR_PIN, led_r);
                    rt_pin_write(LEDG_PIN, led_g);
                    rt_pin_write(LEDB_PIN, led_b);
        }

//        rt_pin_write(LEDB_PIN, PIN_HIGH); /* Set GPIO output 1 */
        rt_thread_mdelay(500);            /* Delay 500mS */
        // count++;
        // //			rt_kprintf("main runnig\r\n");
//         rt_pin_write(LEDB_PIN, PIN_LOW); /* Set GPIO output 0 */
//         rt_thread_mdelay(600);           /* Delay 500mS */

//        if (rt_mb_recv(&mb_temp, (rt_uint32_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
//        {
//            rt_kprintf("thread main: get a mail from mailbox, the content:%s\n", str);
//            /* 延时100ms */
//            rt_thread_mdelay(100);
//        }
    }
}

static void sw_pin_cb(void *args)
{
    rt_kprintf("sw pressed\r\n");
}

// end file
