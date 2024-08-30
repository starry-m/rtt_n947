#include <finsh.h>
#include <netdb.h>
#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文件 */
#define BUFSZ 1024

// #define CONNECT_IP "192.168.1.111"
// #define CONNECT_PORT "20240"
#define DBG_TAG "app.tcp"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>
#include <uMCN.h>
#include "board_value.h"
static McnNode_t color_temp_nod;
MCN_DECLARE(color_temp);
static McnNode_t touch_nod;
MCN_DECLARE(touch_topic);

static const char send_data[] = "This is TCP Client from RT-Thread.\r\n"; /* 发送用到的数据 */
static char send_status_data[100];
static uint8_t send_length;
static rt_thread_t tid_tcp = RT_NULL;
static rt_thread_t tid_data_sync = RT_NULL;

static  touch_topic_t t_data;
static  apds_temp_topic_t data;


/* 用于接收消息的信号量 */
static struct rt_semaphore connected_sem;

/*
rgb=000

*/
static uint8_t update_led_status;
static char temp_buffer[10];
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
        return 2;
    }
    return 0;
}

static void tcpclient_handle_thread(void *parameter)
{
    char *recv_data; /* 用于接收的指针，后面会做一次动态分配以请求可用内存 */
    socklen_t sin_size;
    int sock, bytes_received;
    struct sockaddr_in server_addr, client_addr;
    rt_bool_t stop = RT_FALSE; /* 停止标志 */
    int ret;
    int connected;
    recv_data = rt_malloc(BUFSZ + 1); /* 分配接收用的数据缓冲 */
    if (recv_data == RT_NULL)
    {
        rt_kprintf("No memory\n");
        return;
    }

    /* 一个socket在使用前，需要预先创建出来，指定SOCK_STREAM为TCP的socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* 创建失败的错误处理 */
        rt_kprintf("Socket error\n");

        /* 释放已分配的接收缓冲 */
        rt_free(recv_data);
        return;
    }
    /* 初始化服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000); /* 服务端工作的端口 */
    server_addr.sin_addr.s_addr = INADDR_ANY;
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 绑定socket到服务端地址 */
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        /* 绑定失败 */
        rt_kprintf("Unable to bind\n");

        /* 释放已分配的接收缓冲 */
        rt_free(recv_data);
        return;
    }

    /* 在socket上进行监听 */
    if (listen(sock, 5) == -1)
    {
        rt_kprintf("Listen error\n");

        /* release recv buffer */
        rt_free(recv_data);
        return;
    }

    rt_kprintf("\nTCPServer Waiting for client on port 5000...\n");
    while (stop != RT_TRUE)
    {
        sin_size = sizeof(struct sockaddr_in);

        /* 接受一个客户端连接socket的请求，这个函数调用是阻塞式的 */
        connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);
        /* 返回的是连接成功的socket */
        if (connected < 0)
        {
            rt_kprintf("accept connection failed! errno = %d\n", errno);
            continue;
        }

        /* 接受返回的client_addr指向了客户端的地址信息 */
        rt_kprintf("I got a connection from (%s , %d)\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
//        rt_sem_release(&connected_sem);

        
        /* 客户端连接的处理 */
        while (1)
        {
            

            /* 从connected socket中接收数据，接收buffer是1024大小，但并不一定能够收到1024大小的数据 */
            bytes_received = recv(connected, recv_data, BUFSZ, 0);
            if (bytes_received < 0)
            {
                /* 接收失败，关闭这个connected socket */
                closesocket(connected);
                break;
            }
            else if (bytes_received == 0)
            {
                /* 打印recv函数返回值为0的警告信息 */
                rt_kprintf("\nReceived warning,recv function return 0.\r\n");
                closesocket(connected);
                
                break;
            }

            /* 有接收到数据，把末端清零 */
            recv_data[bytes_received] = '\0';
            if (strcmp(recv_data, "q") == 0 || strcmp(recv_data, "Q") == 0)
            {
                /* 如果是首字母是q或Q，关闭这个连接 */
                closesocket(connected);
                break;
            }
            else if (strcmp(recv_data, "exit") == 0)
            {
                /* 如果接收的是exit，则关闭整个服务端 */
                closesocket(connected);
                stop = RT_TRUE;
                break;
            }
            else
            {
                /* 在控制终端显示收到的数据 */
                rt_kprintf("RECEIVED DATA = %s \n", recv_data);
            }
            if(1==recv_data_jude(recv_data))
            {
								LOG_I(send_status_data);
                /* 发送数据到connected socket */
                ret = send(connected, send_status_data, strlen(send_status_data), 0);
                if (ret < 0)
                {
                    /* 发送失败，关闭这个连接 */
                    closesocket(connected);
                    rt_kprintf("\nsend error,close the socket.\r\n");
                    break;
                }
                else if (ret == 0)
                {
                    /* 打印send函数返回值为0的警告信息 */
                    rt_kprintf("\n Send warning,send function return 0.\r\n");
                }
            }
           
        }
    }

    /* 退出服务 */
    closesocket(sock);

    /* 释放接收缓冲 */
    rt_free(recv_data);

    return;
}
static void data_sync_thread(void *parameter)
{
    static rt_err_t result;
    int ret;
    /* 永久方式等待信号量，获取到信号量，则执行number自加的操作 */
//    result = rt_sem_take(&connected_sem, RT_WAITING_FOREVER);
//    if (result != RT_EOK)
//    {
//        rt_kprintf("thread2 take a dynamic semaphore, failed.\n");
//        return;
//    }
//    else
//    {
//        rt_kprintf("start tcp tx thread\r\n");
//    }
    rt_uint32_t send_tick = 0;
    rt_uint8_t send_temp[20];

    while (1)
    {
            /* synchronous wait until topic received */
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
				rt_sprintf(send_status_data,"r=%d,g=%d,b=%d,c=%d,temp=%d.%d,touch=%d,key=%d \n\0",\
        data.sensor_Color_r,data.sensor_Color_g,data.sensor_Color_b,data.sensor_Color_c,(uint16_t)(data.temperature),(uint16_t)(data.temperature*100)/100%100,t_data.pressed,1);
      
    rt_thread_mdelay(1000);
    // rt_sprintf(send_temp, "第%d次发送\r\n", send_tick++);
    // //        rt_kprintf(" 发送数据到connected socket\r\n");
  
    }
}

int thread_tcp_event_start(void)
{
    color_temp_nod = mcn_subscribe(MCN_HUB(color_temp), RT_NULL, RT_NULL);
    touch_nod = mcn_subscribe(MCN_HUB(touch_topic), RT_NULL, RT_NULL);

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
// MSH_CMD_EXPORT(thread_tcp_event_start, a tcp client);