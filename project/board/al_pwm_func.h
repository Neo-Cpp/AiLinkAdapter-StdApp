
#ifndef __AL_PWM_H__
#define __AL_PWM_H__

typedef enum
{
	UMSG_REQUEST_PWM_EMPTY = 0,
	UMSG_REQUEST_PWM_INIT,	
	UMSG_REQUEST_PWM_START,
	UMSG_REQUEST_PWM_STOP,
	UMSG_REQUEST_PWM_SETPULSE,
	UMSG_REQUEST_PWM_SETPERIOD,
}msg_pwm_te;

void mg_pwm_init(msg_class_ts *msg);


#endif


