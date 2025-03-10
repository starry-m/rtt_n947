# mcxn947 实践

## 任务要求

![任务要求](./pic/活动任务要求.png)

## 开箱

从 rtt github 仓库拉取的 bsp 例程有问题，需要使用 scons --target=mdk5 重新生成下

使用 scons 命令创建独立的项目目录

```
scons --dist --target=mdk --project-name=rtt_n947 --project-path=d:/1my_program_study/funpack3-4/rtt_n947
```

命令行结果

````
scons: Reading SConscript files ...
Keil5 project is generating...                                                                                                              ```
Keil Version: 0.0
Keil-MDK project has generated successfully!
make distribution....
=> frdm-mcxn947
=> start dist handle
=> copy bsp drivers
=> copy bsp CMSIS
=> copy bsp library
=> components
=> include
=> libcpu
=> src
=> tools
Update configuration files...
b'scons: Reading SConscript files ...\r\nKeil5 project is generating...\r\nKeil Version: 0.0\r\nKeil-MDK project has generated successfully!
\r\n'
dist project successfully!
````

打开 eth,只要在 chip driver 中使能，并在 rtconfig.h 中修改 RT_LWIP_TCPTHREAD_STACKSIZE 大小为 2048 即可。

rtt 的 tcp 客户端示例需要添加头文件`#include <stdlib.h>`才不会报错

## 引脚分配

| CAM 引脚 | CAM 功能  | eth 引脚 | eth 功能   |
| -------- | --------- | -------- | ---------- |
| P1_4     | CAM_D0    | P1_4     | ENET_TXCLK |
| P1_5     | CAM_D1    | P1_5     | ENET_TXEN  |
| P1_6     | CAM_D2    | P1_6     | ENET_TXD0  |
| P1_7     | CAM_D3    | P1_7     | ENET_TXD1  |
| P3_4     | CAM_D4    | P1_13    | ENET_RXDV  |
| P3_5     | CAM_D5    | P1_14    | ENET_RXD0  |
| P1_10    | CAM_D6    | P1_15    | ENET_RXD1  |
| P1_11    | CAM_D7    | P1_21    | ENET_MDIO  |
| P1_18    | CAM_PDW   | P1_20    | ENET_MDC   |
| P1_19    | CAM_RST   | P5_8     | ENET_RST   |
| P0_5     | CAM_PCLK  |          |            |
| P2_2     | CAM_CLKIN |          |            |
| P0_11    | CAM_HS    |          |            |
| P0_4     | CAM_VS    |          |            |
| P3_3     | CAM_SCL   |          |            |
| P3_2     | CAM_SDA   |          |            |

| 其他引脚 | 功能       | Mikro Bus 引脚 | 功能     |
| -------- | ---------- | -------------- | -------- |
| P0_10    | LED_RED    | AN             | ADC1_A0  |
| P0_27    | LED_GREEN  | P1_3           | RESET    |
| P1_2     | LED_BLUE   | P3_23          | SPI_CS   |
| P0_23    | WK_UP 按键 | P3_21          | SPI_CLK  |
| P1_3     | TOUCH_PAD  | P3_22          | SPI_MISO |
| P1_10    | CAN_TX     | P3_20          | SPI_MOSI |
| P1_11    | CAN_RX     | P3_19          | PWM      |
| P1_16    | I3C_SCL    | P5_7           | INT      |
| P1_17    | I3C_SDA    | P1_16          | RX       |
| P1_11    | I3C_PUR    | P1_17          | TX       |
|          |            | P1_1           | I2C_SCL  |
|          |            | P1_0           | I2C_SDA  |

| PMOD 引脚 | 功能    | PMOD 引脚 | 功能     |
| --------- | ------- | --------- | -------- |
| P3_3      | I2C_SCL | P0_17     | SPI_CLK  |
| P3_2      | I2C_SDA | P0_18     | SPI_MISO |
| P5_6      | RESET   | P0_16     | SPI_MOSI |
| P0_20     | INT     | P0_19     | SPI_CS   |

## 说明

在 menuconfig 中添加在线包之后，还需要手动`pkgs --update`
并且，在 git 中，不会自动包含，拉取下来后还需要再更新下。

# mcxn947 报告

# 基于 MXCN947 和 TCP 的远程感知与控制系统

# 一、任务要求

- 板卡通过以太网与电脑通信
- 电脑获取板卡上的温度、触摸和按键信息，以及 ADPS9960 传感器的信息
- 可以控制板卡上的 RGB LED 灯

# 二、设计思路

FRDM-MCXN947 是 NXP 推出的一款集成下载器、以太网和多功能模块接口的开发板。配合其丰富的 SDK，我们可以快速且方便的完成大部分简单应用开发，如本次任务中我们需要使用到板卡的以太网、I2C、GPIO、TSI 这些外设作为任务的支撑。其中，在嵌入式设备中我们的以太网通信一般是借助 LwIP 协议栈向上提供的 TCP、UDP 等基础通信，开发我们自己的功能。本次任务即使用的 socket 的 tcp 服务端，电脑作为 TCP 客户端，向板卡请求状态信息和设置状态。温度传感器和 ADPS9960 传感器都可使用 I2C 通信，直接配置和获取数据。触摸按键通过 TSI 专用外设获取。

# 三、实现过程

## 1、硬件电路分析

![](./pic/硬件连接.png)
使用 FRDM-N947 开发板+APDS9960 传感器模块，就可以做出很多好玩的效果。

## 2、实现原理分析

### 以太网 LwIP 协议栈

LwIP 全称 Light weight Interner protocol,是一个轻量化且开源的 TCP/IP 协议栈。他的 ROM 和 RAM 开销极低，极其合适在资源受限的嵌入式设备中使用。
![](./pic/LWIP-1.png)
如上是网卡接收数据流程，摘自[野火 LwIP 应用开发实战指南](https://doc.embedfire.com/net/lwip/zh/latest/doc/chapter9/chapter9.html)。借助 RTOS 的多线程和 IPC 通信，能够很好的隔绝底层以太网接口、内核和用户程序。意味着它可以在任何支持以太网的单片机和任何 RTOS 中跑起来。
再到上面就是各种协议的实现，如 ARP、IP、TCP、UDP 等。

### TSI 触摸

学习 MCXNx4xRM(参考手册)和 UG10111 手册（MCX Nx4x TSI User Guide）。以自电容感应举例
![](./pic/tsi_1.png)
如上图，即使人手指不按下，触摸引脚连接的铜皮电机会与地之间存在电容。当人手指按下时，触摸引脚会与人手指之间等效成一个电容接地，相当于增大这个引脚的对地电容值。
![](./pic/tsi_2.png)
这里通过给触摸引脚进行充电，再放电。让 Vci 的电压变大，当手指按下，电容更大，积累的电荷自然更多，会让 Vci 的值变大的更快，从而更快打断 TSICNT 计数（大概）。实现检测到触摸按下的效果。

### APDS9960 颜色检测

![](/pic/apds.png)
颜色检测是 4 个专用 UV and IR 滤光片分别检测一个通道的值，手势检测和接触检测都是使用的自带的红外发射管，发射，反射接收检测做判断。

## 3、软件流程分析

- 软件框图
  ![](./pic/软件框图.png)
  因为 rtt 有适配这个板子且想真正的用 rtt 做点东西，所以我选择使用主线 rtt 的 frdm-n947。

### 1、环境搭建

- 1.拉取仓库`git clone https://github.com/RT-Thread/rt-thread.git`

- 2.进入`rt-thread/bsp/nxp/mcx/mcxn/frdm-mcxn947/`目录，使用 scons 命令创建独立的项目目录

```
scons --dist --target=mdk --project-name=rtt_n947 --project-path=d:/1my_program_study/funpack3-4/rtt_n947
```

命令行运行结果

````
scons: Reading SConscript files ...
Keil5 project is generating...                                                                                                              ```
Keil Version: 0.0
Keil-MDK project has generated successfully!
make distribution....
=> frdm-mcxn947
=> start dist handle
=> copy bsp drivers
=> copy bsp CMSIS
=> copy bsp library
=> components
=> include
=> libcpu
=> src
=> tools
Update configuration files...
b'scons: Reading SConscript files ...\r\nKeil5 project is generating...\r\nKeil Version: 0.0\r\nKeil-MDK project has generated successfully!
\r\n'
dist project successfully!
````

### 2、打开以太网

打开 eth,只要在 menuconfig 的 chip driver 中使能，并修改 RT_LWIP_TCPTHREAD_STACKSIZE 大小为 2048 即可。然后会自动注册以太网设备，给板卡插上网线连接至路由器，在 shell 中可以使用 ifconfig 查看 ip 等信息，使用 ping 命令测试联网有无问题。

### 3、读取温度传感器 P3T1755 与 手势颜色传感器 apds9960

在 github 检索 apds9960，发现有 arduino 相应的库`https://github.com/adafruit/Adafruit_APDS9960.git`，正好 rtt 这边有适配 rt-duino，可以对接 arduino。因此可以在 menuconfig 中打开 arduino，再在工程中加入 adps9960 的源文件和头文件，直接使用相应示例读取。Adafruit_APDS9960 还依赖 Adafruit_I2CDevice，但是在使用时一直读不到设备，最后魔改了下读写驱动。

```cpp
uint8_t Adafruit_APDS9960::read(uint8_t reg, uint8_t *buf, uint8_t num) {
  rt_uint8_t ret = 0;
  // buf[0] = reg;
  // i2c_dev->write_then_read(buf, 1, buf, num);
  i2c_adps.beginTransmission(APDS9960_ADDRESS);
  i2c_adps.write(reg);
  i2c_adps.endTransmission();
  i2c_adps.requestFrom(APDS9960_ADDRESS, num);
  while (i2c_adps.available()) {
      buf[ret] = i2c_adps.read();
      ret++;
  }
  // i2c_adps.endTransmission();

  return num;
}
void Adafruit_APDS9960::write(uint8_t reg, uint8_t *buf, uint8_t num) {
  // uint8_t prefix[1] = {reg};
  // i2c_dev->write(buf, num, true, prefix, 1);
  i2c_adps.beginTransmission(APDS9960_ADDRESS);
  i2c_adps.write(reg);
  i2c_adps.write(buf, num);
  i2c_adps.endTransmission();
}
```

温度传感器可以使用 i2c 读取，但是没接上拉电阻，存在一些问题，因此我选择温度传感器与 apds 共用引脚，并偷懒直接把温度读取塞到 Adafruit_APDS9960 中。

```cpp
uint8_t Adafruit_APDS9960::read_P3T1755(uint8_t *buf) {
  rt_uint8_t ret = 0;
  // buf[0] = reg;
  // i2c_dev->write_then_read(buf, 1, buf, num);
  i2c_adps.beginTransmission(0x48);
  i2c_adps.write(0);
  i2c_adps.endTransmission();
  i2c_adps.requestFrom(0x48, 2);
  while (i2c_adps.available()) {
      buf[ret] = i2c_adps.read();
      ret++;
  }
  // i2c_adps.endTransmission();

  return ret;
}
```

### 4、触摸检测

rtt 没有 touch 框架，因此直接使用的 nxp sdk 中的 tsi_v6 示例。修改`Libraries/MCXN947/SConscript`,增加` src += ['MCXN947/drivers/fsl_tsi_v6.c']
src += ['MCXN947/drivers/fsl_inputmux.c']`，再重新 scon 生成工程即可。
为了减少中断模式对 rtos 的影响，使用了示例中的软件触发阻塞方式,再单开一个线程做触摸检测。

```c
static void thread_touch_entry(void *parameter)
{
    rt_uint32_t e;
    uint8_t last_touch=0;
    touch_main();
	// rt_event_send(&touch_event, EVENT_FLAG5);
    while (1)
    {
        while (!(TSI_GetStatusFlags(APP_TSI) & kTSI_EndOfScanFlag))
        {
            rt_thread_mdelay(10);
        }
        if (TSI_GetCounter(APP_TSI) > (uint16_t)(buffer.calibratedData[BOARD_TSI_ELECTRODE_1] + TOUCH_DELTA_VALUE))
        {
            rt_kprintf("touch pressed\r\n");
            touch_data.pressed=1;
            // s_tsiInProgress = false;
        }
        else
        {
        //    rt_kprintf("no touch pressed\r\n");
            touch_data.pressed=0;
        }
        if(last_touch !=touch_data.pressed)
        {
            last_touch =touch_data.pressed;
            mcn_publish(MCN_HUB(touch_topic), &touch_data);
        }

        TSI_ClearStatusFlags(APP_TSI, kTSI_EndOfScanFlag | kTSI_OutOfRangeFlag);
				TSI_StartSoftwareTrigger(APP_TSI);
        rt_thread_mdelay(500);
    }
}
```

### 5、TCP 通信

这里选择的与电脑通信方式为板子上开一个 tcp 服务端，等待电脑作为客户端连接和指令操作。目前是基于 RTT 示例的 TCP_SERVER_DEMO 修改，是对一个单一客户端的操作。
并且 Lwip 默认的 TCP 连接后,他的 recv 函数是阻塞接收的（虽然也可以设置超时退出时间），而我的 send 函数又与他在同一个线程。所以我选择在当前文件中使用另一个线程做数据交换，本线程只做 tcp 的 send 和 recv。

```c
static uint8_t recv_data_jude(char *str)
{
    char *ptr_splice;
    if(NULL!=rt_strstr(str,"get status"))
    {
        return 1;
    }
    else if(NULL!=rt_strstr(str,"set status"))
    {
        ptr_splice=strchr(str,':');
        rt_strcpy(temp_buffer,ptr_splice+1);
        LOG_I("get from pc:%s",temp_buffer);
        update_led_status=atoi(temp_buffer);
        LOG_I("update_led_status=%d",update_led_status);
        led_data.led_status=update_led_status;
        mcn_publish(MCN_HUB(led_topic), &led_data);
        return 2;
    }
    return 0;
}
```

上面是对上位机发送消息的命令进行判断。

```c
int thread_tcp_event_start(void)
{
    color_temp_nod = mcn_subscribe(MCN_HUB(color_temp), RT_NULL, RT_NULL);
    touch_nod = mcn_subscribe(MCN_HUB(touch_topic), RT_NULL, RT_NULL);
    key_nod = mcn_subscribe(MCN_HUB(key_topic), RT_NULL, RT_NULL);

    mcn_advertise(MCN_HUB( led_topic),  led_topic_echo);
    /* 初始化信号量 */
    rt_sem_init(&connected_sem, "con_sem", 0, RT_IPC_FLAG_FIFO);

    tid_tcp = rt_thread_create("th tcp", tcpclient_handle_thread, RT_NULL, 2048, 14, 10);
    /* 如果获得线程控制块，启动这个线程 */
    if (tid_tcp != RT_NULL)
        rt_thread_startup(tid_tcp);
    tid_data_sync = rt_thread_create("th sync", data_sync_thread, RT_NULL, 2048, 16, 10);
    /* 如果获得线程控制块，启动这个线程 */
    if (tid_data_sync != RT_NULL)
        rt_thread_startup(tid_data_sync);

    return 0;
}

INIT_APP_EXPORT(thread_tcp_event_start);
```

### 6、屏幕状态信息显示

板卡上有 arduino uno 接口，而我有一块这样接口的 LCD 屏。所以，巧了。
屏幕使用的这个[Arduino 1.8'' TFT Shield](https://oshwhub.com/micespring/adafruit-1-8-tft-shield-v1-clone)
驱动从这里借鉴的[STM32 开发(屏幕驱动 ST7735S-SPI 方式)用 RT-Thread 驱动测试](https://blog.csdn.net/weixin_44742767/article/details/136350637)。
其中有一个引脚与 RGB GREEN 灯冲突，因此对 SJ6 跳线至 P2_5 。

## 4、组件+++

这里我为了多使用 rtos 的特性，因此针对触摸读取、I2C 设备数据读取和 TCP 管理都是单独开的线程。现在需要进行线程间的通信，互相传递数据。这里使用的是我之前看到其他项目后，收藏的一个开源组件[uMCN](https://github.com/JcZou/uMCN.git)，他的介绍如下：

```
uMCN (Micro Multi-Communication Node) 提供了一种基于发布/订阅模式的安全跨线程/进程的通信方式。在系统中，uMCN 被广泛应用于任务和模块间的数据通信。
```

这个组件是基于 rtt 实现的，可以在 Menuconfig 中直接使能加入进来。
还有[i2c-tools](https://github.com/wuhanstudio/rt-i2c-tools)可以很方便的在 shell 中对 I2C 设备进行测试。我的 I2C 驱动使用的就是这里的`SoftwareI2C`.
如下是使用 uMCN 组件完成的线程通信

```c
static void data_sync_thread(void *parameter)
{
    while (1)
    {
    if (mcn_poll(color_temp_nod)) {
        /* copy topic data */
        mcn_copy(MCN_HUB(color_temp), color_temp_nod, &data);
         // rt_kprintf("get sync topic, tick=%ld\n", data.tick);
         //LOG_I("sensor_Color_r=%d, sensor_Color_g=%d,sensor_Color_b=%d,sensor_Color_c=%d,temperature=%d.%d",\
        // data.sensor_Color_r,data.sensor_Color_g,data.sensor_Color_b,data.sensor_Color_c,(uint16_t)(data.temperature),(uint16_t)(data.temperature*100)/100%100);
    }
    if (mcn_poll(touch_nod)){
        mcn_copy(MCN_HUB(touch_topic), touch_nod, &t_data);
        LOG_I("touch status=%d", t_data.pressed);
    }
    if (mcn_poll(key_nod)){
        mcn_copy(MCN_HUB(key_topic), key_nod, &k_data);
        LOG_I("key pressed status=%d", k_data.pressed);
    }
    rt_sprintf(send_status_data,"r=%d,g=%d,b=%d,c=%d,temp=%d.%d,touch=%d,key=%d \n\0",\
        data.sensor_Color_r,data.sensor_Color_g,data.sensor_Color_b,data.sensor_Color_c,(uint16_t)(data.temperature),(uint16_t)(data.temperature*100)/100%100,t_data.pressed,k_data.pressed);
    rt_thread_mdelay(1000);

    }
}

```

## 5、上位机界面

![](./pic/上位机界面.png)
使用 QT 完成简单的界面显示和交互

# 四、效果展示与遇到的问题

## 效果展示

- 获取传感器数据
  ![](./pic/数据读取.png)
- 获取触摸与按键状态
  ![](./pic/触摸检测.png)
- 控制 LED 状态
  ![](./pic/控制LED.png)
  ![](./pic/LED控制2.png)

## 遇到的问题

- 温度传感器使用 cmd 命令读取没问题，但是放在线程中一直读取时只能读取一次数据，然后再也无法读到，仔细查看原理图发现默认上拉电阻没接，最后选择将 apds 传感器的 scl、sda 直接接在温度传感器引脚上，这样就加上了上拉电阻，数据也能正常读取了。
- 触摸检测本来直接使用示例，硬件触发加中断。会导致 tsi 中断一直触发，会影响 rtos 的实时性，所以改成了线程中轮询读取。
- TCP 通信本来想做收发各一个线程，但是当客户端断开连接后会导致 rtt shell 中报错，尝试过挂起发送线程，任然无效，问题定位可能不对。因为赶时间，所以选择换回示例方法，后面再做研究。

# 五、感想与未来计划

- 这是我第二次使用 NXP 的单片机，第一次是寒假练的 RT1021，当时就感觉 NXP 单片机的外设很多，玩法也多。这次使用 N947 后，感觉更明显，各种各样的外设和特性，看的我眼花缭乱，可以说一大半的功能，我都还没体验到。只能说资源 IP 多，做单片机就是豪横，哈哈，啥都能做。
- 下一步必须体验下他的双核跟 NPU。
