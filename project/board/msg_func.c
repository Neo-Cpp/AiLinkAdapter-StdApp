

#include "board.h"
#include "al_board.h"
#include "al_gpio_func.h"
#include "al_i2c_smbus.h"
#include "al_uart_func.h"
#include "al_adc_func.h"
#include "al_spi_func.h"
#include "al_pwm_func.h"

#pragma pack(1)
static volatile uint8_t  DATA_Rx_Buffer[1024];
static volatile uint8_t  DATA_Tx_Buffer[1024];
#pragma pack()




typedef  void (pMSG_FUNC)(msg_class_ts *msg);

typedef struct  
{
    int32_t mess;
    pMSG_FUNC *proc;
}msg_func_ts;

void usb_vcp_send_data1(uint8_t *pbuff,uint32_t len);

static const msg_func_ts msgFunc[]=
{
	{GMSG_REQUEST_DEVICE_SETTING | UMSG_REQUEST_DEVICE_GETID,mg_get_device_id},
    {GMSG_REQUEST_DEVICE_SETTING | UMSG_REQUEST_DEVICE_GETINFO,mg_get_device_info},
    
    {GMSG_REQUEST_GPIO_SETTING   | UMSG_GPIO_SETINPUT,mg_gpio_setinput},
    {GMSG_REQUEST_GPIO_SETTING   | UMSG_GPIO_SETOUTPUT,mg_gpio_setoutput},
    {GMSG_REQUEST_GPIO_SETTING   | UMSG_GPIO_CTLPIN,mg_gpio_ctlpin},
    {GMSG_REQUEST_GPIO_SETTING   | UMSG_GPIO_READPIN,mg_gpio_readpin},
    {GMSG_REQUEST_GPIO_SETTING   | UMSG_GPIO_PULSEPIN,mg_gpio_pulse_set},
    
    
    {GMSG_REQUEST_I2C_SETTING    | UMSG_REQUEST_I2C_HARD,mg_hi2c_init},
    {GMSG_REQUEST_I2C_SETTING    | UMSG_REQUEST_I2C_WRITE,mg_hi2c_write},
    {GMSG_REQUEST_I2C_SETTING    | UMSG_REQUEST_I2C_READ,mg_hi2c_read},
    {GMSG_REQUEST_I2C_SETTING    | UMSG_REQUEST_I2C_SLAVEREAD,mg_hi2c_slaveread},
    {GMSG_REQUEST_I2C_SETTING    | UMSG_REQUEST_I2C_SLAVEWRITE,mg_hi2c_slavewrite},
    
    {GMSG_REQUEST_UART_SETTING   | UMSG_REQUEST_UART_INITS,mg_uart_init},
    
    {GMSG_REQUEST_ADC_SETTING    | UMSG_REQUEST_ADC_INITS ,mg_adc_init},
    
    {GMSG_REQUEST_SPI_SETTING    | UMSG_REQUEST_SPI_INITS , mg_spi_init},
    
    {GMSG_REQUEST_PWM_SETTING    | UMSG_REQUEST_PWM_INIT , mg_pwm_init},
    
};

uint8_t *pRx_Buffer_Func(void)
{
    return (uint8_t *)DATA_Rx_Buffer;
}

void usb_getmsg(uint8_t *pdata)
{
	msg_class_ts pcmd;

	pcmd.mess = pdata[0] << 24 | pdata[1] << 16 | pdata[2] << 8 | pdata[3];
	pcmd.size = pdata[5] << 8 | pdata[4];
	pcmd.PackNum = pdata[6];
	pcmd.status = pdata[7];
	pcmd.lParam = &pdata[8];
	
	for(int16_t i = 0;i<sizeof(msgFunc);i++)
	{
		if(pcmd.mess == msgFunc[i].mess)
		{
			msgFunc[i].proc(&pcmd);
			break;
		}
	}
}

void USBSendMsg(msg_class_ts *pevt)
{
    uint8_t *x = (uint8_t *)pevt->lParam;
	DATA_Tx_Buffer[0] = pevt->mess >> 24;
	DATA_Tx_Buffer[1] = pevt->mess >> 16;
	DATA_Tx_Buffer[2] = pevt->mess >> 8;
	DATA_Tx_Buffer[3] = pevt->mess >> 0;
	DATA_Tx_Buffer[4] = pevt->size;
	DATA_Tx_Buffer[5] = pevt->size >> 8;
	DATA_Tx_Buffer[6] = pevt->PackNum;
	DATA_Tx_Buffer[7] = pevt->status;    
    
	if (pevt->size)
	{
		for (int i = 0; i < pevt->size; i++)
		{
			DATA_Tx_Buffer[8+i] = x[i];
		}
	}
    usb_vcp_send_data1((uint8_t *)DATA_Tx_Buffer,pevt->size+8);
}

void USBSendStatus(uint8_t status)
{
	DATA_Tx_Buffer[0] = (msg_sta_te)status;
    usb_vcp_send_data1((uint8_t *)DATA_Tx_Buffer,1);
}

