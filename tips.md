```
1.用app.overlay则默认使用,用其他文件名则要在build配置中添加
2.添加文件要重新保存cmakelists,此时FILE(GLOB app_sources src/*.c)会添加新文件
```

```
设备树
1.compatible字段用来binding驱动,yaml文件会要求此节点必备的属性
2.status字段为okay则表示此设备驱动启用,用disable则禁用掉
3. zephyr,usb-device = &usbd;  DT_CHOSEN(zephyr_usb_device) 在C代码中,逗号和-都被转成下划线了
```

```
调度器基于线程的优先级将线程分为两类：

协作式线程（cooperative thread）：优先级为负数的线程。这样的线程一旦成为了当前线程，它将一直执行下去，直到它采取的某种动作导致自己变为非就绪线程。
抢占式线程（preemptible thread）：优先级为非负的线程。这样的线程成为了当前线程后，它可以在任何时刻被协作式线程或者优先级更高（或相等）的抢占式线程替代。抢占式线程被替代后，它依然是就绪的。
线程的初始优先级值可以在线程启动后动态地增加或减小。因此，通过改变线程的优先级，抢占式线程可以变为协作式线程，或者相反。

协作式线程可以自身间或性地放弃 CPU，让其它线程得以执行。线程放弃 CPU 的方法有两种：

调用 k_yield() 将线程放到调度器维护的按照优先级排列的就绪线程链表中，然后调用调度器。在该线程被再次调度前，所有优先级高于或等于该线程的就绪线程都将得以执行。如果不存在优先级更高或相等的线程，调度器将不会进行上下文切换，立即再次调度该线程。
调用 k_sleep() 让该线程在一段指定时间内变为非就绪线程。所有 优先级的就绪线程都可能得以执行；不过，不能保证优先级低于该睡眠线程的其它线程都能在睡眠线程再次变为就绪线程前执行完。
```

```
RAM布局 
CONFIG_IS_BOOTLOADER开启后,CONFIG_BOOTLOADER_SRAM_SIZE配置boot的ram尺寸,放在ram最后面,后续zephyr版本废弃掉,要用设备树配置,具体怎么配置还没查到
但CONFIG_IS_BOOTLOADER似乎没什么影响?

chosen {
		zephyr,sram = &sram0;
	};
选择用到的配置
```

```
FLASH布局
nrf52840.dtsi中定义了
flash0: flash@0
限制flash从地址0开始

nrf52840_qiaa.dtsi中定义了
&flash0 {
	reg = <0x00000000 DT_SIZE_K(1024)>;
};
给出了falsh起始地址和大小,起始地址与flash@0要相同

板子配置partitions做分区
chosen {
		zephyr,flash = &flash0;
		zephyr,code-partition = &boot_partition;
	};
选择用到的配置
```
