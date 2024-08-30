/*
 * Copyright 2022 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board.h"
#include "fsl_inputmux.h"
#include "fsl_lptmr.h"
#include "fsl_tsi_v6.h"
#include "pin_mux.h"

#include <rtdevice.h>
#include <rtthread.h>

#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOARD_TSI_ELECTRODE_1 3U
/* Available PAD names on board */
#define PAD_TSI_ELECTRODE_1_NAME "E1"

/* IRQ related redefinitions for specific SOC */
#define TSI0_IRQHandler TSI_END_OF_SCAN_IRQHandler
#define TSI0_IRQn TSI_END_OF_SCAN_IRQn

/* Define the delta value to indicate a touch event */
#define TOUCH_DELTA_VALUE 100U

/* TSI indication led of electrode 1 */
#define LED1_INIT() LED_BLUE_INIT(LOGIC_LED_OFF)
#define LED1_TOGGLE() LED_BLUE_TOGGLE()
#define LED2_INIT() LED_RED_INIT(LOGIC_LED_OFF)

/* Get source clock for LPTMR driver */
#define LPTMR_SOURCE_CLOCK (16000)
/* Define LPTMR microseconds count value */
#define LPTMR_USEC_COUNT (100000)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
tsi_calibration_data_t buffer;
static volatile bool s_tsiInProgress = true;
/* Array of TSI peripheral base address. */
#if defined(TSI0)
#define APP_TSI TSI0
#elif defined(TSI)
#define APP_TSI TSI
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
void touch_main(void)
{
    volatile uint32_t i = 0;
    tsi_selfCap_config_t tsiConfig_selfCap;
    // lptmr_config_t lptmrConfig;
    // memset((void *)&lptmrConfig, 0, sizeof(lptmrConfig));

    /* Initialize standard SDK demo application pins */
    /* Enables the clock for INPUTMUX: Enables clock */
    CLOCK_EnableClock(kCLOCK_InputMux0);

    /* attach FRO 12M to FLEXCOMM4 (debug console) */
    // CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
    // CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* Enables the clk_16k[1] */
    CLOCK_SetupClk16KClocking(kCLOCK_Clk16KToVsys);
    /* attach FRO HF to SCT */
    CLOCK_SetClkDiv(kCLOCK_DivTsiClk, 1u);
    CLOCK_AttachClk(kCLK_IN_to_TSI);

    // BOARD_InitPins();
    // BOARD_InitBootClocks();
    // BOARD_InitDebugConsole();
    /* Init tsi Leds in Demo app */
    // LED1_INIT();
    // LED2_INIT();

    /* Configure LPTMR */
    // LPTMR_GetDefaultConfig(&lptmrConfig);
    /* TSI default hardware configuration for self-cap mode */
    TSI_GetSelfCapModeDefaultConfig(&tsiConfig_selfCap);

    /* Initialize the LPTMR */
    // LPTMR_Init(LPTMR0, &lptmrConfig);
    /* Initialize the TSI */
    TSI_InitSelfCapMode(APP_TSI, &tsiConfig_selfCap);
    /* Enable noise cancellation function */
    TSI_EnableNoiseCancellation(APP_TSI, true);

    /* Set timer period */
    // LPTMR_SetTimerPeriod(LPTMR0, USEC_TO_COUNT(LPTMR_USEC_COUNT, LPTMR_SOURCE_CLOCK));

    // NVIC_EnableIRQ(TSI0_IRQn);
    TSI_EnableModule(APP_TSI, true); /* Enable module */

    rt_kprintf("\r\nTSI_V6 Self-Cap mode Example Start!\r\n");
    /*********  CALIBRATION PROCESS ************/
    memset((void *)&buffer, 0, sizeof(buffer));
    TSI_SelfCapCalibrate(APP_TSI, &buffer);
    /* Print calibrated counter values */
    for (i = 0U; i < FSL_FEATURE_TSI_CHANNEL_COUNT; i++)
    {
        rt_kprintf("Calibrated counters for channel %d is: %d \r\n", i, buffer.calibratedData[i]);
    }

    /********** SOFTWARE TRIGGER SCAN USING POLLING METHOD ********/
    rt_kprintf("\r\nNOW, comes to the software trigger scan using polling method!\r\n");
    TSI_EnableHardwareTriggerScan(APP_TSI, false); /* Enable software trigger scan */
    TSI_DisableInterrupts(APP_TSI, kTSI_EndOfScanInterruptEnable);
    TSI_ClearStatusFlags(APP_TSI, kTSI_EndOfScanFlag);
    TSI_SetSelfCapMeasuredChannel(APP_TSI, BOARD_TSI_ELECTRODE_1);
    TSI_StartSoftwareTrigger(APP_TSI);

    // while (!(TSI_GetStatusFlags(APP_TSI) & kTSI_EndOfScanFlag))
    // {
    // }
    // rt_kprintf("Channel %d Normal mode counter is: %d \r\n", BOARD_TSI_ELECTRODE_1, TSI_GetCounter(APP_TSI));

    // TSI_ClearStatusFlags(APP_TSI, kTSI_EndOfScanFlag | kTSI_OutOfRangeFlag);

// #if (defined(PAD_TSI_ELECTRODE_2_ENABLED) && PAD_TSI_ELECTRODE_2_ENABLED)
//     TSI_ClearStatusFlags(APP_TSI, kTSI_EndOfScanFlag);
//     TSI_SetSelfCapMeasuredChannel(APP_TSI, BOARD_TSI_ELECTRODE_2);
//     TSI_StartSoftwareTrigger(APP_TSI);
//     while (!(TSI_GetStatusFlags(APP_TSI) & kTSI_EndOfScanFlag))
//     {
//     }
//     rt_kprintf("Channel %d Normal mode counter is: %d \r\n", BOARD_TSI_ELECTRODE_2, TSI_GetCounter(APP_TSI));
// #endif


//     /********** SOFTWARE TRIGGER SCAN USING INTERRUPT METHOD ********/
//     rt_kprintf("\r\nNOW, comes to the software trigger scan using interrupt method!\r\n");
//     TSI_EnableInterrupts(APP_TSI, kTSI_GlobalInterruptEnable);
//     TSI_EnableInterrupts(APP_TSI, kTSI_EndOfScanInterruptEnable);
//     TSI_ClearStatusFlags(APP_TSI, kTSI_EndOfScanFlag);
//     TSI_SetSelfCapMeasuredChannel(APP_TSI, BOARD_TSI_ELECTRODE_1);

//     while (s_tsiInProgress)
//     {
//         TSI_StartSoftwareTrigger(APP_TSI);
//     }
//     s_tsiInProgress = true;
//     rt_kprintf("Channel %d Normal mode counter is: %d \r\n", BOARD_TSI_ELECTRODE_1, TSI_GetCounter(APP_TSI));
// #if (defined(PAD_TSI_ELECTRODE_2_ENABLED) && PAD_TSI_ELECTRODE_2_ENABLED)
//     TSI_SetSelfCapMeasuredChannel(APP_TSI, BOARD_TSI_ELECTRODE_2);
//     TSI_StartSoftwareTrigger(APP_TSI);
//     while (s_tsiInProgress)
//     {
//         TSI_StartSoftwareTrigger(APP_TSI);
//     }
//     rt_kprintf("Channel %d Normal mode counter is: %d \r\n", BOARD_TSI_ELECTRODE_2, TSI_GetCounter(APP_TSI));
// #endif
    /********** HARDWARE TRIGGER SCAN ********/
    // rt_kprintf("\r\nNOW, comes to the hardware trigger scan method!\r\n");
    // rt_kprintf("After running, touch pad %s each time, you will see LED toggles.\r\n", PAD_TSI_ELECTRODE_1_NAME);
    // TSI_EnableModule(APP_TSI, false);
    // TSI_EnableHardwareTriggerScan(APP_TSI, true);
    // TSI_EnableInterrupts(APP_TSI, kTSI_EndOfScanInterruptEnable);
    // TSI_ClearStatusFlags(APP_TSI, kTSI_EndOfScanFlag);

    // TSI_SetSelfCapMeasuredChannel(APP_TSI,
    //                               BOARD_TSI_ELECTRODE_1); /* Select BOARD_TSI_ELECTRODE_1 as detecting electrode. */
    // TSI_EnableModule(APP_TSI, true);
    // INPUTMUX_AttachSignal(INPUTMUX0, 0U, kINPUTMUX_Lptmr0ToTsiTrigger);
    // LPTMR_StartTimer(LPTMR0); /* Start LPTMR triggering */

    // while (1)
    // {
    //     rt_thread_mdelay(600);
    // }
}
static rt_thread_t tid_touch = RT_NULL;

#define EVENT_FLAG3 (1 << 3)
#define EVENT_FLAG5 (1 << 5)
/* 事件控制块 */
// static struct rt_event touch_event;
void TSI0_IRQHandler(void)
{
#if BOARD_TSI_ELECTRODE_1 > 15
    /* errata ERR051410: When reading TSI_COMFIG[TSICH] bitfield, the upper most bit will always be 0. */
    if ((TSI_GetSelfCapMeasuredChannel(APP_TSI) | 0x10U) == BOARD_TSI_ELECTRODE_1)
#else
    if (TSI_GetSelfCapMeasuredChannel(APP_TSI) == BOARD_TSI_ELECTRODE_1)
#endif
    {
        if (TSI_GetCounter(APP_TSI) > (uint16_t)(buffer.calibratedData[BOARD_TSI_ELECTRODE_1] + TOUCH_DELTA_VALUE))
        {
            //            LED1_TOGGLE(); /* Toggle the touch event indicating LED */
            // rt_event_send(&touch_event, EVENT_FLAG3);

            s_tsiInProgress = false;
        }
    }

    /* Clear endOfScan flag */
    TSI_ClearStatusFlags(APP_TSI, kTSI_EndOfScanFlag);
    SDK_ISR_EXIT_BARRIER;
}
MSH_CMD_EXPORT(touch_main, touch main);

static void thread_touch_entry(void *parameter)
{
    rt_uint32_t e;

    touch_main();
	// rt_event_send(&touch_event, EVENT_FLAG5);
    while (1)
    {
        while (!(TSI_GetStatusFlags(APP_TSI) & kTSI_EndOfScanFlag))
        {
					 rt_thread_mdelay(10);
        }
        // rt_kprintf("Channel %d Normal mode counter is: %d \r\n", BOARD_TSI_ELECTRODE_1, TSI_GetCounter(APP_TSI));
        if (TSI_GetCounter(APP_TSI) > (uint16_t)(buffer.calibratedData[BOARD_TSI_ELECTRODE_1] + TOUCH_DELTA_VALUE))
        {
            //            LED1_TOGGLE(); /* Toggle the touch event indicating LED */
            // rt_event_send(&touch_event, EVENT_FLAG3);
            rt_kprintf("touch pressed\r\n");
            // s_tsiInProgress = false;
        }
//				else
//					rt_kprintf("no touch pressed\r\n");
        TSI_ClearStatusFlags(APP_TSI, kTSI_EndOfScanFlag | kTSI_OutOfRangeFlag);
				TSI_StartSoftwareTrigger(APP_TSI);
        rt_thread_mdelay(500);
        /* 第一次接收事件，事件3或事件5任意一个可以触发线程1，接收完后清除事件标志 */
        // if (rt_event_recv(&touch_event, (EVENT_FLAG3 | EVENT_FLAG5), RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 500, &e) ==
        //     RT_EOK)
        // {
        //     rt_kprintf("thread touch: OR recv event 0x%x\n", e);
		// 			if(EVENT_FLAG3==e)
        //     rt_kprintf("touch pressed\r\n");
        // }
        
    }
}

int thread_touch_start(void)
{
//    rt_err_t result;

//    /* 初始化事件对象 */
//    result = rt_event_init(&touch_event, "tou event", RT_IPC_FLAG_FIFO);
//    if (result != RT_EOK)
//    {
//        rt_kprintf("init touch event failed.\n");
//        return -1;
//    }
    tid_touch = rt_thread_create("th touch", thread_touch_entry, RT_NULL, 1024, 9, 10);
    /* 如果获得线程控制块，启动这个线程 */
    if (tid_touch != RT_NULL)
        rt_thread_startup(tid_touch);

    return 0;
}
INIT_APP_EXPORT(thread_touch_start);
