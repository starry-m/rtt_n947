#include "Adafruit_APDS9960.h"

#include <rtthread.h>
#include <uMCN.h>
#define LOG_TAG              "app.apds&temp"
#include <rtdbg.h>
#include "board_value.h"
MCN_DEFINE(color_temp, sizeof(apds_temp_topic_t));

apds_temp_topic_t apds_temp_data;



Adafruit_APDS9960 apds;
// TwoWire sensor_wire;
static void apds9960_read_test()
{
    uint32_t tick = 100;
    // sensor_wire.begin("swi2c0");
    // if (!apds.begin(20, APDS9960_AGAIN_4X, APDS9960_ADDRESS, &sensor_wire))
    if (!apds.begin())
    {
        LOG_I("failed to initialize device! Please check your wiring.\n");	
        return;
    }
    else
        LOG_I("Device initialized!");

    // gesture mode will be entered once proximity mode senses something close
    // apds.enableProximity(true);
    // apds.enableGesture(true);
    // uint8_t gesture = 0;
    // enable color sensign mode
       apds.enableColor(true);
    LOG_I("mode set is enabled");
    uint16_t r, g, b, c;
    while (tick--)
    {
        // gesture = apds.readGesture();
        // if (gesture == APDS9960_DOWN)
        //     LOG_I("v");
        // if (gesture == APDS9960_UP)
        //     LOG_I("^");
        // if (gesture == APDS9960_LEFT)
        //     LOG_I("<");
        // if (gesture == APDS9960_RIGHT)
        //     LOG_I(">");
        // rt_thread_mdelay(10);
        // wait for color data to be ready
        while (!apds.colorDataReady())
        {
            rt_thread_mdelay(5);
        }
        
        // get the data and print the different channels
        apds.getColorData(&r, &g, &b, &c);
        LOG_I("Color data: red=%d green=%d blue=%d clear=%d\n", r, g, b, c);
        rt_thread_mdelay(500);
    }
}

MSH_CMD_EXPORT(apds9960_read_test, apds read);

static void thread_apds_entry(void *parameter)
{
    rt_uint32_t e;
    rt_uint8_t ret = 0;
    rt_uint8_t buffer[2];
	// rt_event_send(&touch_event, EVENT_FLAG5);
    if (!apds.begin())
    {
        LOG_I("failed to initialize device! Please check your wiring. ");	
        return;
    }
    else
        LOG_I("apds9960 Device initialized! ");

    // gesture mode will be entered once proximity mode senses something close
    // apds.enableProximity(true);
    // apds.enableGesture(true);
    // uint8_t gesture = 0;
    // enable color sensign mode
    apds.enableColor(true);
    LOG_I("apds9960mode set is enabled  ");
    uint16_t r, g, b, c;
    while (1)
    {
        while (!apds.colorDataReady())
        {
            rt_thread_mdelay(5);
        }
        // get the data and print the different channels
        apds.getColorData(&r, &g, &b, &c);
//        LOG_I("Color data: red=%d green=%d blue=%d clear=%d\n", r, g, b, c);
        rt_thread_mdelay(500);
        ret=apds.read_P3T1755(buffer);
//        LOG_I("P3T1755->temperature=%d.%d ", buffer[0], buffer[1]);
        rt_thread_mdelay(500); 
        apds_temp_data.sensor_Color_r=r;
        apds_temp_data.sensor_Color_g=g;
        apds_temp_data.sensor_Color_b=b;
        apds_temp_data.sensor_Color_c=c;
        apds_temp_data.temperature=((buffer[0]<<8)+buffer[1])/256.0f;
        mcn_publish(MCN_HUB(color_temp), &apds_temp_data);
    }
}

static int  apds_data_topic_echo(void *param)
{
    static  apds_temp_topic_t data;
    if (mcn_copy_from_hub((McnHub *)param, &data) == RT_EOK)
    {
        LOG_I("sensor_Color_r=%d, sensor_Color_g=%d,sensor_Color_b=%d,sensor_Color_c=%d,temperature=%d",\
        data.sensor_Color_r,data.sensor_Color_g,data.sensor_Color_b,data.sensor_Color_c,(uint16_t)(data.temperature/256.0));
        return 0;
    }
    return -1;
}


static rt_thread_t tid_a = RT_NULL;
int thread_apds9960_start(void)
{
    mcn_advertise(MCN_HUB( color_temp),  apds_data_topic_echo);



    tid_a = rt_thread_create("th apds", thread_apds_entry, RT_NULL, 1024, 11, 10);
    /* 如果获得线程控制块，启动这个线程 */
    if (tid_a != RT_NULL)
        rt_thread_startup(tid_a);

    return 0;
}
INIT_APP_EXPORT(thread_apds9960_start);