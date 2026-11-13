
#ifndef __AL_ADC_FUNC_H__
#define __AL_ADC_FUNC_H__

typedef enum
{
	UMSG_REQUEST_ADC_EMPTY = 0,
	UMSG_REQUEST_ADC_INITS,
	UMSG_REQUEST_ADC_READDATAS,
}msg_adc_te;


void mg_adc_init(msg_class_ts *msg);


#endif

