#ifndef MY_FC_LED_H
#define MY_FC_LED_H

typedef enum {
    LED_ON,
    LED_BREATHE,
    LED_BLINK,
    LED_OFF,
} fc_led_mode_def;

typedef struct fc_led_mode_change_msg
{
    int led_index;
    fc_led_mode_def led_mode;
} fc_led_mode_change_msg;


int fc_led_init(void);

void fc_led_set_mode(int idx, fc_led_mode_def mode);
#endif /* MY_FC_LED_H */