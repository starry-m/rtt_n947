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
TwoWire sensor_wire;
static void apds9960_read_test()
{
    uint8_t tick = 100;
    sensor_wire.begin("i2c2");
    if (!apds.begin(20, APDS9960_AGAIN_4X, APDS9960_ADDRESS, &sensor_wire))
    {
        rt_kprintf("failed to initialize device! Please check your wiring.\n");
        return;
    }
    else
        rt_kprintf("Device initialized!");

    // gesture mode will be entered once proximity mode senses something close
    apds.enableProximity(true);
    apds.enableGesture(true);

    while (tick--)
    {	
        uint8_t gesture = apds.readGesture();
        if (gesture == APDS9960_DOWN)
            rt_kprintf("v");
        if (gesture == APDS9960_UP)
            rt_kprintf("^");
        if (gesture == APDS9960_LEFT)
            rt_kprintf("<");
        if (gesture == APDS9960_RIGHT)
            rt_kprintf(">");
    }
}

MSH_CMD_EXPORT(apds9960_read_test, apds read);
