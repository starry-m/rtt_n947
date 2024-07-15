//
// Created by shchl on 2024/2/28.
//

#include "st7735s.h"
#include <rtdevice.h>
#include <rtthread.h>

#define CRITICAL_ENTER()  if(ST7735S_DEV.CRIS._enter)ST7735S_DEV.CRIS._enter()
#define CRITICAL_EXIT()   if(ST7735S_DEV.CRIS._enter)ST7735S_DEV.CRIS._exit()
#define DC_DATA_SELECT()  if(ST7735S_DEV.DC._select_data) ST7735S_DEV.DC._select_data()
#define DC_REG_SELECT()   if(ST7735S_DEV.DC._select_reg) ST7735S_DEV.DC._select_reg()
#define CS_SELECT()       if(ST7735S_DEV.CS._select) ST7735S_DEV.CS._select()
#define CS_DESELECT()     if(ST7735S_DEV.CS._deselect) ST7735S_DEV.CS._deselect()

void ST7735S_write_dat_u8(uint8_t dat) {
    CRITICAL_ENTER();
    DC_DATA_SELECT(); /*恢复到写数据模式*/
    CS_SELECT();
    if (ST7735S_DEV.SPI._write_byte) {
        ST7735S_DEV.SPI._write_byte(dat);
    }
    CS_DESELECT();
    CRITICAL_EXIT();
}

void ST7735S_write_dat_u16(uint16_t dat) {
    ST7735S_write_dat_u8(dat >> 8);
    ST7735S_write_dat_u8(dat);
}

void ST7735S_write_reg_val(uint16_t reg, uint16_t dat) {
    ST7735S_write_reg(reg);
    ST7735S_write_dat_u8(dat);
}

void ST7735S_write_reg(uint8_t dat) {
    CRITICAL_ENTER();
    DC_REG_SELECT();/*恢复到写寄存器模式*/
    CS_SELECT();
    if (ST7735S_DEV.SPI._write_byte) {
        ST7735S_DEV.SPI._write_byte(dat);
    }
    CS_DESELECT();
    CRITICAL_EXIT();
}

/*******************************************************************************/
//函数：void LCD_Init(void)
//函数功能：初始化lcd
/*******************************************************************************/

int LCD_Init(void) {

    //初始化驱动 I/O接口
    st7735s_init();

    LCD_SoftRest();        //软复位

    ST7735S_write_reg(0x11);      //Sleep exit
    rt_thread_mdelay(120);      // delay 120 ms
	
    //ST7735R Frame Rate
    ST7735S_write_reg(0xB1);
    ST7735S_write_dat_u8(0x01);
    ST7735S_write_dat_u8(0x2C);
    ST7735S_write_dat_u8(0x2D);
    ST7735S_write_reg(0xB2);
    ST7735S_write_dat_u8(0x01);
    ST7735S_write_dat_u8(0x2C);
    ST7735S_write_dat_u8(0x2D);
    ST7735S_write_reg(0xB3);
    ST7735S_write_dat_u8(0x01);
    ST7735S_write_dat_u8(0x2C);
    ST7735S_write_dat_u8(0x2D);
    ST7735S_write_dat_u8(0x01);
    ST7735S_write_dat_u8(0x2C);
    ST7735S_write_dat_u8(0x2D);
    ST7735S_write_reg(0xB4); //Column inversion
    ST7735S_write_dat_u8(0x07);
    //ST7735R Power Sequence
    ST7735S_write_reg(0xC0);
    ST7735S_write_dat_u8(0xA2);
    ST7735S_write_dat_u8(0x02);
    ST7735S_write_dat_u8(0x84);
    ST7735S_write_reg(0xC1);
    ST7735S_write_dat_u8(0xC5);
    ST7735S_write_reg(0xC2);
    ST7735S_write_dat_u8(0x0A);
    ST7735S_write_dat_u8(0x00);
    ST7735S_write_reg(0xC3);
    ST7735S_write_dat_u8(0x8A);
    ST7735S_write_dat_u8(0x2A);
    ST7735S_write_reg(0xC4);
    ST7735S_write_dat_u8(0x8A);
    ST7735S_write_dat_u8(0xEE);
    ST7735S_write_reg(0xC5); //VCOM
    ST7735S_write_dat_u8(0x0E);
    ST7735S_write_reg(0x36); //MX, MY, RGB mode
    ST7735S_write_dat_u8(0xC0);
    //ST7735R Gamma Sequence
    ST7735S_write_reg(0xe0);
    ST7735S_write_dat_u8(0x0f);
    ST7735S_write_dat_u8(0x1a);
    ST7735S_write_dat_u8(0x0f);
    ST7735S_write_dat_u8(0x18);
    ST7735S_write_dat_u8(0x2f);
    ST7735S_write_dat_u8(0x28);
    ST7735S_write_dat_u8(0x20);
    ST7735S_write_dat_u8(0x22);
    ST7735S_write_dat_u8(0x1f);
    ST7735S_write_dat_u8(0x1b);
    ST7735S_write_dat_u8(0x23);
    ST7735S_write_dat_u8(0x37);
    ST7735S_write_dat_u8(0x00);
    ST7735S_write_dat_u8(0x07);
    ST7735S_write_dat_u8(0x02);
    ST7735S_write_dat_u8(0x10);
    ST7735S_write_reg(0xe1);
    ST7735S_write_dat_u8(0x0f);
    ST7735S_write_dat_u8(0x1b);
    ST7735S_write_dat_u8(0x0f);
    ST7735S_write_dat_u8(0x17);
    ST7735S_write_dat_u8(0x33);
    ST7735S_write_dat_u8(0x2c);
    ST7735S_write_dat_u8(0x29);
    ST7735S_write_dat_u8(0x2e);
    ST7735S_write_dat_u8(0x30);
    ST7735S_write_dat_u8(0x30);
    ST7735S_write_dat_u8(0x39);
    ST7735S_write_dat_u8(0x3f);
    ST7735S_write_dat_u8(0x00);
    ST7735S_write_dat_u8(0x07);
    ST7735S_write_dat_u8(0x03);
    ST7735S_write_dat_u8(0x10);
    ST7735S_write_reg(0x2a);
    ST7735S_write_dat_u8(0x00);
    ST7735S_write_dat_u8(0x00);
    ST7735S_write_dat_u8(0x00);
    ST7735S_write_dat_u8(0x7f);
    ST7735S_write_reg(0x2b);
    ST7735S_write_dat_u8(0x00);
    ST7735S_write_dat_u8(0x00);
    ST7735S_write_dat_u8(0x00);
    ST7735S_write_dat_u8(0x9f);
    ST7735S_write_reg(0xF0); //Enable test command
    ST7735S_write_dat_u8(0x01);
    ST7735S_write_reg(0xF6); //Disable ram power save mode
    ST7735S_write_dat_u8(0x00);
    ST7735S_write_reg(0x3A); //65k mode
    ST7735S_write_dat_u8(0x05);
    ST7735S_write_reg(0x29);//Display on
    LCD_Display_Dir(ST7735S_DEV.CNF.screen_mode);    //选择--屏幕显示方式
    LCD_Clear(ST7735S_DEV.CNF.back_color);
     return RT_EOK;
}

INIT_COMPONENT_EXPORT(LCD_Init);



//********************************************************************************/
//函数：void LCD_DisplayOn(void)
//函数功能：
//LCD开启显示
//*******************************************************************/
void LCD_DisplayOn(void) {
    ST7735S_write_reg(0X29);    //开启显示
}

//*******************************************************************/
//函数：void LCD_DisplayOff(void)
//函数功能：
//LCD关闭显示
//*******************************************************************/

void LCD_DisplayOff(void) {
    ST7735S_write_reg(0X28);    //关闭显示

}

/**************************************************************************/
//函数：void LCD_Display_Dir(u8 dir)
//函数功能：设置LCD的显示方向及像素参数

//输入参数：

//设置LCD显示方向

//dir:   0,竖屏  正
//         1,竖屏  反
//         2,横屏  左
//         3,横屏  右
//*************************************************************************/
void LCD_Display_Dir(ST7735S_DIR_MODE dir) {
    ST7735S_DEV.CNF.screen_mode = dir;
    uint16_t width, high;
    switch (ST7735S_DEV.CNF.screen_mode) {

        case VER_0_MODE: { //竖屏  正
            width = ST7735S_SCREEN_WIDTH;
            high = ST7735S_SCREEN_HIGH;
            ST7735S_DEV.CNF.CMD.wramcmd = 0X2C;
            ST7735S_DEV.CNF.CMD.setxcmd = 0X2A;
            ST7735S_DEV.CNF.CMD.setycmd = 0X2B;
            ST7735S_write_reg_val(0x36, 0xC0);//选择扫描方向
        }
            break;
        case VER_1_MODE: {
            width = ST7735S_SCREEN_WIDTH;
            high = ST7735S_SCREEN_HIGH;
            ST7735S_DEV.CNF.CMD.wramcmd = 0X2C;
            ST7735S_DEV.CNF.CMD.setxcmd = 0X2A;
            ST7735S_DEV.CNF.CMD.setycmd = 0X2B;
            ST7735S_write_reg_val(0x36, 0x40);//选择扫描方向
        }
            break;
        case HOR_0_MODE: {
            width = ST7735S_SCREEN_HIGH;
            high = ST7735S_SCREEN_WIDTH;
            ST7735S_DEV.CNF.CMD.wramcmd = 0X2C;
            ST7735S_DEV.CNF.CMD.setxcmd = 0X2A;
            ST7735S_DEV.CNF.CMD.setycmd = 0X2B;
            ST7735S_write_reg_val(0x36, 0xA0);//选择扫描方向
        }
            break;
        case HOR_1_MODE: {
            width = ST7735S_SCREEN_HIGH;
            high = ST7735S_SCREEN_WIDTH;
            ST7735S_DEV.CNF.CMD.wramcmd = 0X2C;
            ST7735S_DEV.CNF.CMD.setxcmd = 0X2A;
            ST7735S_DEV.CNF.CMD.setycmd = 0X2B;
            ST7735S_write_reg_val(0x36, 0x60);//选择扫描方向

        }
            break;
    }
    ST7735S_DEV.CNF.width = width;
    ST7735S_DEV.CNF.height = high;
    //以下设置，为窗口参数设置，设置了全屏的显示范围
    LCD_Set_Window(0, 0, width, high);//设置全屏窗口


}




/**************************************************************************/
//函数功能：设置LCD的显示窗口
//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height.
//*************************************************************************/
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height) {
    width = sx + width - 1;
    height = sy + height - 1;
    if (ST7735S_DEV.CNF.screen_mode == VER_0_MODE || ST7735S_DEV.CNF.screen_mode == VER_1_MODE) {

        ST7735S_write_reg(ST7735S_DEV.CNF.CMD.setxcmd);
        ST7735S_write_dat_u16(sx + 1);      //设置 X方向起点
        ST7735S_write_dat_u16(width + 1);   //设置 X方向终点

        ST7735S_write_reg(ST7735S_DEV.CNF.CMD.setycmd);
        ST7735S_write_dat_u16(sy + 2);      //设置 Y方向起点
        ST7735S_write_dat_u16(height + 2);  //设置 Y方向终点

    } else {
        ST7735S_write_reg(ST7735S_DEV.CNF.CMD.setxcmd);
        ST7735S_write_dat_u16(sx + 1);      //设置 X方向起点
        ST7735S_write_dat_u16(width + 1);   //设置 X方向终点

        ST7735S_write_reg(ST7735S_DEV.CNF.CMD.setycmd);
        ST7735S_write_dat_u16(sy + 2);      //设置 Y方向起点
        ST7735S_write_dat_u16(height + 2);  //设置 Y方向终点

    }
}
/*******************************************************************************/
//函数：void LCD_Clear(u16 color)
//函数功能：全屏清屏填充函数
//输入参数：
//color:要清屏的填充色
/*******************************************************************************/

void LCD_Clear(uint16_t color) {
    uint32_t index = 0;
    uint32_t totalpoint;
    LCD_Set_Window(0, 0, ST7735S_DEV.CNF.width, ST7735S_DEV.CNF.height);//设置全屏窗口
    totalpoint = ST7735S_DEV.CNF.width * ST7735S_DEV.CNF.height;            //得到总点数
    LCD_SetCursor(0, 0);    //设置光标位置
    LCD_WriteRAM_Prepare();            //开始写入GRAM
    for (index = 0; index < totalpoint; index++) {
        ST7735S_write_dat_u16(color);
    }
}

//*******************************************************************/
//函数功能：设置光标位置
//输入参数：
//Xpos:横坐标
//Ypos:纵坐标
//*******************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos) {
    if (ST7735S_DEV.CNF.screen_mode == VER_0_MODE || ST7735S_DEV.CNF.screen_mode == VER_1_MODE) {

        ST7735S_write_reg(ST7735S_DEV.CNF.CMD.setxcmd);
        ST7735S_write_dat_u16(Xpos+1);
        ST7735S_write_reg(ST7735S_DEV.CNF.CMD.setycmd);
        ST7735S_write_dat_u16(Ypos+2);

    } else {
        ST7735S_write_reg(ST7735S_DEV.CNF.CMD.setxcmd);
        ST7735S_write_dat_u16(Xpos+1);
        ST7735S_write_reg(ST7735S_DEV.CNF.CMD.setycmd);
        ST7735S_write_dat_u16(Ypos+2);
    }

}
//*******************************************************************/
//函数：void LCD_WriteRAM_Prepare(void)
//函数功能：开始写GRAM  命令
//*******************************************************************/

void LCD_WriteRAM_Prepare(void) {
    ST7735S_write_reg(ST7735S_DEV.CNF.CMD.wramcmd);
}
//*******************************************************************/
//函数：void LCD_DrawPoint(u16 x,u16 y)
//函数功能：画点
//输入参数：
//x,y:坐标
//POINT_COLOR:此点的颜色
//*******************************************************************/

void LCD_DrawPoint(uint16_t x, uint16_t y) {
    LCD_SetCursor(x, y);        //设置光标位置
    LCD_WriteRAM_Prepare();    //开始写入GRAM
    ST7735S_write_dat_u16(ST7735S_DEV.CNF.point_color);
}

//*******************************************************************/
//函数：LCD_WriteRAM(u16 RGB_Code)
//函数功能：写入点阵颜色值
//输入参数:
//RGB_Code:颜色值
//*******************************************************************/

void LCD_WriteRAM(uint16_t RGB_Code) {
    ST7735S_write_dat_u16(RGB_Code);//写十六位GRAM
}
/*******************************************************************************/
//函数：void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)

//函数功能：区域填充函数
//输入参数：
//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
/*******************************************************************************/

void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color) {

    uint32_t i;
    uint16_t xlen = 0;

    //设置窗口

    LCD_Set_Window(sx, sy, ex - sx, ey - sy);//设置窗口

    ST7735S_write_reg(ST7735S_DEV.CNF.CMD.wramcmd);

    xlen = (ex - sx) * (ey - sy);//计算出总共需要写入的点数

    LCD_WriteRAM_Prepare();                //开始写入GRAM

    for (i = 0; i <= xlen; i++) {
        ST7735S_write_dat_u16(color);    //显示颜色
    }

    //以下设置，为窗口参数设置，设置了全屏的显示范围
    LCD_Set_Window(0, 0, ST7735S_DEV.CNF.width, ST7735S_DEV.CNF.height);//设置全屏窗口


}
//*******************************************************************/
//函数：void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color)
//函数功能：快速画点
//输入参数：
//x,y:坐标
//color:颜色
//*******************************************************************/

void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color) {

    LCD_SetCursor(x, y);        //设置光标位置
    ST7735S_write_reg(ST7735S_DEV.CNF.CMD.wramcmd);
    ST7735S_write_dat_u16(color); //写入16位颜色

}
/*******************************************************************************/
//函数：void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
//函数功能：画矩形
//输入参数：
//(x1,y1),(x2,y2):矩形的对角坐标
//Color;线条颜色
/*******************************************************************************/

void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color) {
    LCD_DrawLine(x1, y1, x2, y1, Color);
    LCD_DrawLine(x1, y1, x1, y2, Color);
    LCD_DrawLine(x1, y2, x2, y2, Color);
    LCD_DrawLine(x2, y1, x2, y2, Color);
}
/*******************************************************************************/
//函数功能：画线
//输入参数：
//x1,y1:起点坐标
//x2,y2:终点坐标
//Color;线条颜色
/*******************************************************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t Color) {
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0)incx = 1; //设置单步方向
    else if (delta_x == 0)incx = 0;//垂直线
    else {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0;//水平线
    else {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴
    else distance = delta_y;
    for (t = 0; t <= distance + 1; t++)//画线输出
    {
        LCD_Fast_DrawPoint(uRow, uCol, Color);//画点 --使用输入颜色参数
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance) {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance) {
            yerr -= distance;
            uCol += incy;
        }
    }
}
/*******************************************************************************/
//函数：void LCD_Draw_Circle(u16 x0,u16 y0,u8 r)
//函数功能：在指定位置画一个指定大小的圆
//输入参数：
//(x,y):中心点
//r    :半径
//Color;线条颜色
/*******************************************************************************/

void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t Color) {
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);             //判断下个点位置的标志
    while (a <= b) {
        LCD_Fast_DrawPoint(x0 + a, y0 - b, Color);             //5
        LCD_Fast_DrawPoint(x0 + b, y0 - a, Color);             //0
        LCD_Fast_DrawPoint(x0 + b, y0 + a, Color);             //4
        LCD_Fast_DrawPoint(x0 + a, y0 + b, Color);             //6
        LCD_Fast_DrawPoint(x0 - a, y0 + b, Color);             //1
        LCD_Fast_DrawPoint(x0 - b, y0 + a, Color);
        LCD_Fast_DrawPoint(x0 - a, y0 - b, Color);             //2
        LCD_Fast_DrawPoint(x0 - b, y0 - a, Color);             //7
        a++;
        //使用Bresenham算法画圆
        if (di < 0)di += 4 * a + 6;
        else {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}
