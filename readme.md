# mcxn947 实践

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
