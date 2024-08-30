#include "SoftwareI2C.h"

#include <rtdevice.h>
#include <rtthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SoftwareI2C i2c_temp;
static float board_temp;
static void temp_read_test()
{
    i2c_temp.begin(48, 49);
    rt_uint8_t ret = 0;
    rt_uint8_t buffer[2];

    i2c_temp.requestFrom(0x48, 2);
    while (i2c_temp.available())
    {
        buffer[ret] = i2c_temp.read();
        ret++;
    }
    //		 rt_kprintf("read_buff[0]=%d,[1]=%d\r\n",buffer[0],buffer[1]);
    rt_kprintf("P3T1755->temperature=%d.%d\r\n", buffer[0], buffer[1]);
}

MSH_CMD_EXPORT(temp_read_test, temp read);

static rt_thread_t tid_temp = RT_NULL;
/* 邮箱控制块 */
struct rt_mailbox mb_temp;
/* 用于放邮件的内存池 */
static char mb_pool[128];
static char mb_str[30];
static void thread_temp_entry(void *parameter)
{
    rt_uint32_t e;

//    temp_read_test();
    rt_uint8_t ret = 0;
    rt_uint8_t buffer[2];
    uint16_t temp;
	rt_thread_mdelay(1000);
    i2c_temp.begin(48, 49);
//    rt_uint8_t ret = 0;
//    rt_uint8_t buffer[2];

    i2c_temp.requestFrom(0x48, 2);
    while (i2c_temp.available())
    {
        buffer[ret] = i2c_temp.read();
        ret++;
    }
    //		 rt_kprintf("read_buff[0]=%d,[1]=%d\r\n",buffer[0],buffer[1]);
    rt_kprintf("P3T1755->temperature=%d.%d\r\n", buffer[0], buffer[1]);
    // rt_event_send(&touch_event, EVENT_FLAG5);
    while (1)
    {
        rt_thread_mdelay(1000);
        rt_kprintf("temp thread runnig\r\n");
        // i2c_temp.begin(48, 49);
        i2c_temp.requestFrom(0x48, 2);
        while (i2c_temp.available())
        {
            buffer[ret] = i2c_temp.read();
            ret++;
        }
				rt_kprintf("P3T1755->temperature=%d.%d\r\n", buffer[0], buffer[1]);
//        temp = buffer[0] << 8 + buffer[1];
//        board_temp = temp / 256.0f;
        rt_snprintf(mb_str, temp, "board temp=%d.%d\r\n", buffer[0], buffer[1]);
//        rt_snprintf(mb_str, temp, "board temp:%.3f", board_temp);
        rt_kprintf(mb_str);
        mb_str[temp] = '\0';
        rt_kprintf(mb_str);
        /* 发送mb_str1地址到邮箱中 */
//        rt_mb_send(&mb_temp, (rt_uint32_t)&mb_str);

        
    }
}

int thread_temp_start(void)
{
    tid_temp = rt_thread_create("th temp", thread_temp_entry, RT_NULL, 1024, 10, 5);
    /* 如果获得线程控制块，启动这个线程 */
    if (tid_temp != RT_NULL)
        rt_thread_startup(tid_temp);

    return 0;
}
//INIT_APP_EXPORT(thread_temp_start);
