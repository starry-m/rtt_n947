//
// Created by shchl on 2024/2/28.
//
#include "st7735s.h"
//#include "drv_common.h"
#include "drv_spi.h"
#include <rtthread.h>
#define DBG_ENABLE
#define DBG_TAG "ST7735_DEMO"
#define DBG_LVL DBG_LOG

#include <rtdbg.h>


struct rt_spi_device *st7735s_device = RT_NULL;

static void spi_cris_enter(void) { rt_spi_take_bus(st7735s_device); }

static void spi_cris_exit(void) { rt_spi_release_bus(st7735s_device); }

static void spi_cs_deselect(void) { rt_spi_release(st7735s_device); }

static void spi_cs_select(void) { rt_spi_take(st7735s_device); }

static void dc_dat_select(void) { rt_pin_write(ST7735S_DC_PIN, PIN_HIGH); }

static void dc_reg_select(void) { rt_pin_write(ST7735S_DC_PIN, PIN_LOW); }

static void spi_write_byte(uint8_t data) { rt_spi_send(st7735s_device, &data, 1); }

rt_err_t st7735s_device_init(const char *spi_dev_name) {
    //将spi设备挂载到总线
   rt_hw_spi_device_attach(ST7735S_BUS_NAME, spi_dev_name,ST7735S_CS_PIN);
    /* 查找 spi 设备获取设备句柄 */
    st7735s_device = (struct rt_spi_device *) rt_device_find(spi_dev_name);
    if (st7735s_device == NULL) {
        LOG_W("未找到该设备");
        return -RT_ERROR;
    } else {
        LOG_W("找到该设备");
        //配置spi参数
        struct rt_spi_configuration cfg;
        {
            cfg.data_width = 8;//数据宽度为八位
            cfg.mode = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
            cfg.max_hz = 80 * 1000 * 1000;//最大波特率,因为在cubemx配置分频后，spi2的波特率是4.5M，所以这里最大就写5了
        }
        rt_spi_configure(st7735s_device, &cfg);
    }
    return RT_EOK;
}


static void st7735s_callback_register() {
    /* register critical section callback function */
    reg_st7735s_cris_cbfunc(spi_cris_enter, spi_cris_exit);
    /* register SPI device CS select callback function */
    reg_st7735s_cs_cbfunc(spi_cs_select, spi_cs_deselect);

    /* register DC select callback function */
    reg_st7735s_dc_cbfunc(dc_dat_select, dc_reg_select);
    /* register SPI write data callback function */
    reg_st7735s_spi_cbfunc(spi_write_byte);
}
//********************************************************************************/
//函数：void LCD_SoftRest(void)
//函数功能：给屏幕发命令，执行软复位命令
//LCD开启显示
//*******************************************************************/
void LCD_SoftRest(void) {
    // rt_pin_write(ST7735S_RESET_PIN, PIN_LOW);
    // rt_thread_mdelay(100);
    // rt_pin_write(ST7735S_RESET_PIN, PIN_HIGH);
    // rt_thread_mdelay(1000);
}


int st7735s_init(void) {
    rt_err_t result = RT_EOK;
    /*初始化对应的引脚*/
    rt_pin_mode(ST7735S_DC_PIN, PIN_MODE_OUTPUT);
    // rt_pin_mode(ST7735S_RESET_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(ST7735S_CS_PIN, PIN_MODE_OUTPUT);
    // rt_pin_mode(ST7735S_BLK_PIN, PIN_MODE_OUTPUT);

    result = st7735s_device_init(ST7735S_SPI_DEVICE);
    if (result != RT_EOK) goto __exit;

    /* 回调函数注册 */
    st7735s_callback_register();

    __exit:
    if (result == RT_EOK) {
        LOG_I("st7735s_init initialize success.");
    } else {
        LOG_I("st7735s_init initialize err:%d.", result);
    }
     return RT_EOK;
}
#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       1024
#define THREAD_TIMESLICE        5
static rt_thread_t tid1 = RT_NULL;
/* 线程1的入口函数 */
static void thread1_entry(void *parameter)
{
    rt_uint32_t count = 0;
    while (1)
    {
        /* 线程1采用低优先级运行，一直打印计数值 */
//        rt_kprintf("thread1 count: %d\n", count ++);
        rt_thread_mdelay(500);
        LCD_Clear(RED);
        rt_thread_mdelay(500);
        LCD_Clear(GREEN);
        rt_thread_mdelay(500);
        LCD_Clear(BLUE);
    }
}


int st7735s_test(void)
{
    /* 创建线程1，名称是thread1，入口是thread1_entry*/
    tid1 = rt_thread_create("thread1",
                            thread1_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);
    /* 如果获得线程控制块，启动这个线程 */
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    return RT_EOK;
}

INIT_APP_EXPORT(st7735s_test);
// MSH_CMD_EXPORT(st7735s_init, st7735s_  init);

