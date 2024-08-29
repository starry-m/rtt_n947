/***************************************************************************
  This is a library for the APDS9960 digital proximity, ambient light, RGB, and gesture sensor

  This sketch puts the sensor in gesture mode and decodes gestures.
  To use this, first put your hand close to the sensor to enable gesture mode.
  Then move your hand about 6" from the sensor in the up -> down, down -> up,
  left -> right, or right -> left direction.

  Designed specifically to work with the Adafruit APDS9960 breakout
  ----> http://www.adafruit.com/products/3595

  These sensors use I2C to communicate. The device's I2C address is 0x39

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include "Adafruit_APDS9960.h"

#include <rtthread.h>

Adafruit_APDS9960 apds;
// TwoWire sensor_wire;
static void apds9960_read_test()
{
    uint8_t tick = 100;
    // sensor_wire.begin("swi2c0");
    // if (!apds.begin(20, APDS9960_AGAIN_4X, APDS9960_ADDRESS, &sensor_wire))
    if (!apds.begin())
    {
        rt_kprintf("failed to initialize device! Please check your wiring.\n");
        return;
    }
    else
        rt_kprintf("Device initialized!");

    // gesture mode will be entered once proximity mode senses something close
    // apds.enableProximity(true);
    // apds.enableGesture(true);
    // uint8_t gesture=0;
      //enable color sensign mode
  apds.enableColor(true);
  rt_kprintf("mode set is enabled");
  uint16_t r, g, b, c;
    while (tick--)
    {	
         
        //wait for color data to be ready
        while(!apds.colorDataReady()){
            rt_thread_mdelay(5);
        }
        //get the data and print the different channels
        apds.getColorData(&r, &g, &b, &c);
        rt_kprintf("Color data: red=%d green=%d blue=%d clear=%d\n", r, g, b, c);
        rt_thread_mdelay(500);
    }
}

MSH_CMD_EXPORT(apds9960_read_test, apds read);
