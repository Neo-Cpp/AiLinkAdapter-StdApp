
#ifndef __AL_GPIO_FUNC_H__
#define __AL_GPIO_FUNC_H__

typedef enum
{
	UMSG_GPIO_EMPTY,
    UMSG_GPIO_SETINPUT,
    UMSG_GPIO_SETOUTPUT,
    UMSG_GPIO_CTLPIN,
    UMSG_GPIO_READPIN,
    UMSG_GPIO_PULSEPIN,
}msg_gpio_te;


void mg_gpio_setinput(msg_class_ts *msg);
void mg_gpio_setoutput(msg_class_ts *msg);
void mg_gpio_ctlpin(msg_class_ts *msg);
void mg_gpio_readpin(msg_class_ts *msg);
void mg_gpio_pulse_set(msg_class_ts *msg);
#endif
