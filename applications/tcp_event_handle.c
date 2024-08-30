#include <finsh.h>
#include <netdb.h>
#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> /* 使用BSD socket，需要包含socket.h头文件 */
#define BUFSZ 1024

#define CONNECT_IP "192.168.1.111"
#define CONNECT_PORT "20240"
#define DBG_TAG "app.tcp"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static const char send_data[] = "This is TCP Client from RT-Thread.\r\n"; /* 发送用到的数据 */

int connected;
/* 用于接收消息的信号量 */
static struct rt_semaphore connected_sem;

static void tcpclient_rx_thread(void *parameter)
{
    char *recv_data; /* 用于接收的指针，后面会做一次动态分配以请求可用内存 */
    socklen_t sin_size;
    int sock, bytes_received;
    struct sockaddr_in server_addr, client_addr;
    rt_bool_t stop = RT_FALSE; /* 停止标志 */
    int ret;

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
        rt_sem_release(&connected_sem);

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
                //                closesocket(connected);
                //                break;
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
        }
    }

    /* 退出服务 */
    closesocket(sock);

    /* 释放接收缓冲 */
    rt_free(recv_data);

    return;
}
static void tcpclient_tx_thread(void *parameter)
{
    static rt_err_t result;
    int ret;
    /* 永久方式等待信号量，获取到信号量，则执行number自加的操作 */
    result = rt_sem_take(&connected_sem, RT_WAITING_FOREVER);
    if (result != RT_EOK)
    {
        rt_kprintf("thread2 take a dynamic semaphore, failed.\n");
        return;
    }
    else
    {
        rt_kprintf("start tcp tx thread\r\n");
    }
    rt_uint32_t send_tick = 0;
    rt_uint8_t send_temp[20];
    while (1)
    {
        rt_thread_mdelay(1000);
        rt_sprintf(send_temp, "第%d次发送\r\n", send_tick++);
        //        rt_kprintf(" 发送数据到connected socket\r\n");
        /* 发送数据到connected socket */
        ret = send(connected, send_temp, strlen(send_temp), 0);
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
static rt_thread_t tid_tcp = RT_NULL;
static rt_thread_t tid_tcp_tx = RT_NULL;
int thread_tcp_event_start(void)
{
    /* 初始化信号量 */
    rt_sem_init(&connected_sem, "con_sem", 0, RT_IPC_FLAG_FIFO);

    tid_tcp = rt_thread_create("th tcp r", tcpclient_rx_thread, RT_NULL, 2048, 24, 100);
    /* 如果获得线程控制块，启动这个线程 */
    if (tid_tcp != RT_NULL)
        rt_thread_startup(tid_tcp);

    tid_tcp_tx = rt_thread_create("th tcp t", tcpclient_tx_thread, RT_NULL, 2048, 23, 100);
    /* 如果获得线程控制块，启动这个线程 */
    if (tid_tcp_tx != RT_NULL)
        rt_thread_startup(tid_tcp_tx);

    return 0;
}

//INIT_APP_EXPORT(thread_tcp_event_start);
// MSH_CMD_EXPORT(thread_tcp_event_start, a tcp client);