
#ifndef __AL_UART_FUNC_H__
#define __AL_UART_FUNC_H__

typedef enum
{
	UMSG_REQUEST_UART_EMPTY = 0,
	UMSG_REQUEST_UART_INITS,
	UMSG_REQUEST_UART_WRITE,
	UMSG_REQUEST_UART_READ,
}msg_uart_te;



void mg_uart_init(msg_class_ts *msg);

#endif
