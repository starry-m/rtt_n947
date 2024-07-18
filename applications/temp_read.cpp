#include "SoftwareI2C.h"


#include <rtthread.h>
#include <rtdevice.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SoftwareI2C i2c_temp;

static void temp_read_test()
{
    i2c_temp.begin(48,49);
    rt_uint8_t ret = 0; 
		rt_uint8_t buffer[2];
		
    i2c_temp.requestFrom(0x48, 2);
    while (i2c_temp.available()) {
        buffer[ret] = i2c_temp.read();
        ret++;
    }
//		 rt_kprintf("read_buff[0]=%d,[1]=%d\r\n",buffer[0],buffer[1]);
		 rt_kprintf("P3T1755->temperature=%d.%d\r\n",buffer[0],buffer[1]);	
}


MSH_CMD_EXPORT(temp_read_test, temp  read);

