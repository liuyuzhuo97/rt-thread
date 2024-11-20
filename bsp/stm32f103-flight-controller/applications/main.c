/*
* Copyright (c) 2006-2021, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author    	   Notes
* 2024-11-19     yuzhuo_liu       init
*/

#include <board.h>
#include <rtthread.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#endif /* RT_USING_NANO */

#include "fc_led.h"
// #include "fc_mpu6050.h"
// #include "fc_motor.h"
// #include "fc_watchdog.h"
// #include "fc_pose.h"
// #include "fc_pid.h"


void fc_start(void);

int main(void) {
	rt_kprintf("enter main\n");
	int ret;
	ret = fc_led_init();
	if (!ret) {
		rt_kprintf("!!!flight controller led init failed!!!\n");
		return ret;
	}

	fc_led_set_mode(0, LED_ON);

	// ret = fc_mpu6050_init();
	// if (!ret) {
	// 	rt_kprintf("!!!flight controller mpu6050 init failed!!!\n");
	// 	return ret;
	// }	
	
	// ret = fc_motor_init();
	// if (!ret) {
	// 	rt_kprintf("!!!flight controller motor init failed!!!\n");
	// 	return ret;
	// }

	// ret = fc_watchdog_init();
	// if (!ret) {
	// 	rt_kprintf("!!!flight controller watchdog init failed!!!\n");
	// 	return ret;
	// }

	// ret = fc_pose_init();
	// if (!ret) {
	// 	rt_kprintf("!!!flight controller pose init failed!!!\n");
	// 	return ret;
	// }

	// ret = fc_pid_init();
	// if (!ret) {
	// 	rt_kprintf("!!!flight controller pid init failed!!!\n");
	// 	return ret;
	// }

	// fc_start();

	while (1);

	return 0;
}
