

#include "board.h"
#include "al_uart_func.h"


typedef struct {
    uint8_t     idx;
	uint8_t		StopBits;
	uint8_t		WordLength;
	uint8_t		Parity;
	uint16_t	RS485Mode;
	uint32_t	BaudRate;
	uint16_t    HardwareFlowControl;
}g_uart_t, *g_puart_t;

typedef struct{
    gpio_type *gpioport;
    uint16_t gpiopin;
}mygpio_type;

typedef struct{
    mygpio_type gpio_tx_type;
    mygpio_type gpio_rx_type;
}usart_gpio_type;

static usart_type *UART_PORT[] = {USART2,USART6};
static usart_gpio_type UART_GPIO[]  = {
    {GPIOA,GPIO_PINS_0,GPIOB,GPIO_PINS_6},
    

    
};


static void al_uart_init(g_puart_t puartx)
{
    UART_GPIO[puartx->idx].gpio_rx_type.gpiopin = GPIO_PINS_6;
    UART_GPIO[puartx->idx].gpio_rx_type.gpioport = 0;
}   
static void al_uart_write(uint8_t uart_idx,uint8_t *pData,uint16_t len)
{
    //usart_data_transmit(0,0);
}


void mg_uart_init(msg_class_ts *msg)
{
    al_uart_init(0);
}
