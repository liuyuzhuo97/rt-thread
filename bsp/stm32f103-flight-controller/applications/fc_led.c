// #include <zephyr/device.h>
// #include <zephyr/devicetree.h>
// #include <errno.h>
// #include <zephyr/drivers/pwm.h>
// #include <zephyr/kernel.h>
// #include <zephyr/logging/log.h>
// #include <zephyr/shell/shell.h>

// #include "fc_led.h"

// LOG_MODULE_REGISTER(led, 0);

// #define  get_pwm_device()      (DEVICE_DT_GET(DT_ALIAS(pwm_led)))

// /* CYCLE OF PWM WAVE */
// #define MAX_PWM_CYCLE_HZ       (30)
// /* LEVEL OF LED BRIGHTLESS */
// #define BRIGHTNESS_LEVEL 	   50
// /* TIME OF LED FROM LIGHT TO DARK TO LIGHT*/
// #define LED_CHANGE_INTERVAL_MS  1000

// #define MAX_PWM_CYCLE_MS       (MSEC_PER_SEC / MAX_PWM_CYCLE_HZ)
// #define CYCLE_CHANGE_ONCE      MAX_PWM_CYCLE_MS/BRIGHTNESS_LEVEL
// #define FADE_DELAY_MS	       (LED_CHANGE_INTERVAL_MS / BRIGHTNESS_LEVEL)
// #define FADE_DELAY	           K_MSEC(FADE_DELAY_MS)
// #define BLINK_DELAY	           K_MSEC(FADE_DELAY_MS * FADE_DELAY_MS)

// static struct k_thread s_led_task;
// K_KERNEL_STACK_MEMBER(led_task_stack, 512);

// static led_mode_def led_current_mode = LED_BREATHE;
// static uint64_t period_cycles;
// static const struct device *led_pwm = get_pwm_device();
// static uint32_t brightless_table[BRIGHTNESS_LEVEL + 1];
// static struct k_sem led_mode_change_sem;

// static int led_on(void)
// {
//     LOG_DBG("LDE MODE ALWAYS ON");

//     return pwm_set_cycles(led_pwm, 1, period_cycles, \
//                     0, 0);
// }

// static int led_breathe(void)
// {
//     LOG_DBG("LDE MODE BREATHE");
//     static uint8_t led_breathe_current_level = 0;
//     static bool led_breathe_current_dir = false;
//     uint8_t blightness_index;
//     int err = 0;

//     if (led_breathe_current_level > BRIGHTNESS_LEVEL) {
//         led_breathe_current_level = 0;
//         led_breathe_current_dir = !led_breathe_current_dir;
//     }

//     if (led_breathe_current_dir) {
//         blightness_index = led_breathe_current_level;
//     } else {
//         blightness_index = BRIGHTNESS_LEVEL - led_breathe_current_level;
//     }

//     led_breathe_current_level++;
    
//     err = pwm_set_cycles(led_pwm, 1, period_cycles, \
//             brightless_table[blightness_index], 0);
//     if (err < 0) {
//         return err;
//     }

//     k_sem_take(&led_mode_change_sem, FADE_DELAY);

//     return err;
// }

// static int led_blink(void)
// {
//     LOG_DBG("LDE MODE BLINK");
//     int err = 0;
//     static bool led_blink_current_dir = false;
//     uint8_t blightness_index;

//     led_blink_current_dir = !led_blink_current_dir;
//     if (led_blink_current_dir) {
//         blightness_index = 0;
//     } else {
//         blightness_index = BRIGHTNESS_LEVEL;
//     }

//     err = pwm_set_cycles(led_pwm, 1, period_cycles, \
//             brightless_table[blightness_index], 0);
//     if (err < 0) {
//         return err;
//     }

//     k_sem_take(&led_mode_change_sem, BLINK_DELAY);

//     return err;
// }

// static int led_off(void)
// {
//     LOG_DBG("LDE MODE OFF");

//     return pwm_set_cycles(led_pwm, 1, period_cycles, \
//                     period_cycles, 0);
// }


// static int led_display(led_mode_def mode)
// {
// 	int err = 0;
//     switch (led_current_mode) {
//     case LED_ON:
//         err = led_on();
//         if (err < 0) {
//             LOG_ERR("led_current_mode=%d err=%d line%d\n", led_current_mode, err, __LINE__);
//             return err;
//         }

//         k_sem_take(&led_mode_change_sem, K_FOREVER);

//         break;
//     case LED_BREATHE:
//         err = led_breathe();
//         if (err < 0) {
//             LOG_ERR("led_current_mode=%d err=%d line%d\n", led_current_mode, err, __LINE__);
//             return err;
//         }
//         break;
//     case LED_BLINK:
//         err = led_blink();
//         if (err < 0) {
//             LOG_ERR("led_current_mode=%d err=%d line%d\n", led_current_mode, err, __LINE__);
//             return err;
//         }
//         break;
//     case LED_OFF:
//     default:
//         err = led_off();
//         if (err < 0) {
//             LOG_ERR("led_current_mode=%d err=%d line%d\n", led_current_mode, err, __LINE__);
//             return err;
//         }

//         k_sem_take(&led_mode_change_sem, K_FOREVER);

//         break;
//     }

//     return err;
// }

// static void led_task(void* arg)
// {
// 	int err;
// 	uint64_t ticks_per_sec;

// 	if (!device_is_ready(led_pwm)) {
// 		LOG_ERR("LED PWM is not ready\n");
//         return;
// 	}

//     err = pwm_get_cycles_per_sec(led_pwm, 1, &ticks_per_sec);
//     if (err < 0) {
// 		return;
// 	}

//     period_cycles = ticks_per_sec / MAX_PWM_CYCLE_MS;

//     /* initialize blightness array for lookup */
// 	for (uint8_t level = 0; level <= BRIGHTNESS_LEVEL; level++) {
//         brightless_table[level] = period_cycles * level / BRIGHTNESS_LEVEL;    
//         LOG_ERR("brightless_table[%d]=%d\n", \
//                  level, brightless_table[level]);
//     }

//     while (true) {
//         led_display(led_current_mode);
//     }
// }

// void fc_led_set_mode(led_mode_def mode)
// {
//     led_current_mode = mode;
//     k_sem_give(&led_mode_change_sem);
// }

// void fc_led_init(void)
// {
// 	k_sem_init(&led_mode_change_sem, 0, UINT_MAX);
//     k_tid_t tid = k_thread_create(&s_led_task, led_task_stack,
//             512, (k_thread_entry_t)led_task, NULL, NULL, NULL,
//             3, K_INHERIT_PERMS, K_NO_WAIT);

// 	if (!tid) {
// 		LOG_ERR("ERROR led thread create fail!\n");
// 	}

// }

// static int led_mode_handler(const struct shell *sh, size_t argc, char **argv,
// 		    void *data)
// {
// 	int val = (intptr_t)data;
//     fc_led_set_mode(val);
//     shell_print(sh, "led mode: %s value: %d", argv[0], val);
// 	return 0;
// }

// SHELL_SUBCMD_DICT_SET_CREATE(mode, led_mode_handler,
//         (on, LED_ON, "led on"),
//         (breathe, LED_BREATHE, "led breathe"),
//         (blink, LED_BLINK, "led blink"),
//         (off, LED_OFF, "led off")
// );

// SHELL_CMD_REGISTER(led, &mode,
// 		"switch led mode: on, breathe, blink, off",
// 		NULL);





#include <drv_gpio.h>
#include <rtthread.h>
#ifndef RT_USING_NANO
#include <rtdevice.h>
#endif /* RT_USING_NANO */
#include<stdlib.h>

#include "fc_led.h"

/* defined the LED0 pin: PF7 */
#define FC_LED0    GET_PIN(F, 7)

static rt_mq_t fc_led_mode_change_mq;

static int fc_leds[] = {
    FC_LED0,
};
static int fc_leds_num;
static int fc_led_on(int idx)
{
    rt_pin_write(fc_leds[idx], PIN_HIGH);
    return 0;
}

static int fc_led_breathe(int idx)
{
    // LOG_DBG("LDE MODE BREATHE");
    // static uint8_t led_breathe_current_level = 0;
    // static bool led_breathe_current_dir = false;
    // uint8_t blightness_index;
    // int err = 0;

    // if (led_breathe_current_level > BRIGHTNESS_LEVEL) {
    //     led_breathe_current_level = 0;
    //     led_breathe_current_dir = !led_breathe_current_dir;
    // }

    // if (led_breathe_current_dir) {
    //     blightness_index = led_breathe_current_level;
    // } else {
    //     blightness_index = BRIGHTNESS_LEVEL - led_breathe_current_level;
    // }

    // led_breathe_current_level++;
    
    // err = pwm_set_cycles(led_pwm, 1, period_cycles, \
    //         brightless_table[blightness_index], 0);
    // if (err < 0) {
    //     return err;
    // }

    // k_sem_take(&led_mode_change_sem, FADE_DELAY);

    // return err;
    return -ENOTSUP;
}

static int fc_led_blink(int idx)
{
//     LOG_DBG("LDE MODE BLINK");
//     int err = 0;
//     static bool led_blink_current_dir = false;
//     uint8_t blightness_index;

//     led_blink_current_dir = !led_blink_current_dir;
//     if (led_blink_current_dir) {
//         blightness_index = 0;
//     } else {
//         blightness_index = BRIGHTNESS_LEVEL;
//     }

//     err = pwm_set_cycles(led_pwm, 1, period_cycles, \
//             brightless_table[blightness_index], 0);
//     if (err < 0) {
//         return err;
//     }

//     k_sem_take(&led_mode_change_sem, BLINK_DELAY);

//     return err;
    return -ENOTSUP;
}

static int fc_led_off(int idx)
{
    rt_pin_write(fc_leds[idx], PIN_LOW);
    return 0;
}

static int fc_led_display(int led_index, fc_led_mode_def mode)
{
	int err = 0;
    switch (mode) {
    case LED_ON:
        err = fc_led_on(led_index);
        if (err < 0) {
            rt_kprintf("led%d mode=%d err=%d line%d\n", led_index, mode, err, __LINE__);
            return err;
        }

        // k_sem_take(&led_mode_change_sem, K_FOREVER);

        break;
    case LED_BREATHE:
        err = fc_led_breathe(led_index);
        if (err < 0) {
            rt_kprintf("led%d mode=%d err=%d line%d\n", led_index, mode, err, __LINE__);
            return err;
        }
        break;
    case LED_BLINK:
        err = fc_led_blink(led_index);
        if (err < 0) {
            rt_kprintf("led%d mode=%d err=%d line%d\n", led_index, mode, err, __LINE__);
            return err;
        }
        break;
    case LED_OFF:
    default:
        err = fc_led_off(led_index);
        if (err < 0) {
            rt_kprintf("led%d mode=%d err=%d line%d\n", led_index, mode, err, __LINE__);
            return err;
        }

        // k_sem_take(&led_mode_change_sem, K_FOREVER);

        break;
    }

    return err;
}

static void fc_led_process_thread(void* arg)
{
	// int err;
	// uint64_t ticks_per_sec;

    // err = pwm_get_cycles_per_sec(led_pwm, 1, &ticks_per_sec);
    // if (err < 0) {
	// 	return;
	// }

    // period_cycles = ticks_per_sec / MAX_PWM_CYCLE_MS;

    /* initialize blightness array for lookup */
	// for (uint8_t level = 0; level <= BRIGHTNESS_LEVEL; level++) {
    //     brightless_table[level] = period_cycles * level / BRIGHTNESS_LEVEL;    
    //     LOG_ERR("brightless_table[%d]=%d\n", \
    //              level, brightless_table[level]);
    // }

    fc_led_mode_change_msg led_mode_chande_msg;

    fc_leds_num = sizeof(fc_leds)/sizeof(fc_leds[0]);

    for (uint8_t i = 0; i < fc_leds_num; ++i) {
        rt_pin_mode(fc_leds[i], PIN_MODE_OUTPUT);
        fc_led_off(i);
    }

    while (1) {
        if (rt_mq_recv(fc_led_mode_change_mq, &led_mode_chande_msg, sizeof(fc_led_mode_change_msg), RT_WAITING_FOREVER) > 0) {
            fc_led_display(led_mode_chande_msg.led_index, led_mode_chande_msg.led_mode);
        }
    }
}

int fc_led_init(void)
{
    fc_led_mode_change_mq = rt_mq_create ("fc_led_mode_change_mq", sizeof(fc_led_mode_change_msg), 5, RT_IPC_FLAG_FIFO);

    rt_thread_t tid = rt_thread_create("fc_led_process_thread",
                            fc_led_process_thread,
                            NULL,
                            1024,
                            10,
                            1);

    if (tid == RT_NULL) {
        rt_kprintf("ERROR led thread create fail!\n");
        return -ECANCELED;
    }

    if (rt_thread_startup(tid) != RT_EOK) {
        rt_kprintf("ERROR led thread start fail!\n");
        return -ECANCELED;
    }

    return 0;
}

void fc_led_set_mode(int idx, fc_led_mode_def mode)
{
    fc_led_mode_change_msg mode_change_msg = {
        .led_index = idx,
        .led_mode = mode,
    };

    rt_mq_send(fc_led_mode_change_mq, &mode_change_msg, sizeof(fc_led_mode_change_msg));
}

static void fc_led_set_mode_cmd(int argc, char**argv)
{
    int idx;
    fc_led_mode_def mode;
    
    if (argc != 3)
    {
        rt_kprintf("fc led set mode <led_idx|mode>'\n");
        rt_kprintf("led_idx: from 0 to %d'\n", fc_leds_num - 1);
        rt_kprintf("mode: 0: LED_ON,\n"
                   "      1: LED_BREATHE,\n"
                   "      2: LED_BLINK,\n"
                   "      3: LED_OFF\n");
        return;
    }

    idx = atoi(argv[1]);
    mode = (fc_led_mode_def)atoi(argv[2]);

    if (idx < 0 || idx >=  fc_leds_num)
    {
        rt_kprintf("led_idx: from 0 to %d'\n", fc_leds_num - 1);
    }
    else if (mode < LED_ON || mode > LED_OFF)
    {
        rt_kprintf("mode: 0: LED_ON,\n"
                   "      1: LED_BREATHE,\n"
                   "      2: LED_BLINK,\n"
                   "      3: LED_OFF\n");
    }
    else
    {
        fc_led_set_mode(idx, mode);
    }

}

MSH_CMD_EXPORT(fc_led_set_mode_cmd, fc led set mode <led_idx|mode>);