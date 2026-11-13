
#ifndef __AL_I2C_SMBUS_H__
#define __AL_I2C_SMBUS_H__

#define I2C_TIMEOUT                      0xFFFFFFF

//#define I2Cx_CLKCTRL                   0x4170FEFE   //10K
//#define I2Cx_CLKCTRL                   0x90F06666   //50K
#define I2Cx_CLKCTRL                     0x90F03030   //100K
//#define I2Cx_CLKCTRL                   0x20F07DDE   //200K

#define I2Cx_ADDRESS                     0xA0

#define I2Cx_PORT                        I2C1
#define I2Cx_CLK                         CRM_I2C1_PERIPH_CLOCK
#define I2Cx_DMA                         DMA1
#define I2Cx_DMA_CLK                     CRM_DMA1_PERIPH_CLOCK

#define I2Cx_SCL_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK
#define I2Cx_SCL_GPIO_PIN                GPIO_PINS_6
#define I2Cx_SCL_GPIO_PinsSource         GPIO_PINS_SOURCE6
#define I2Cx_SCL_GPIO_PORT               GPIOB
#define I2Cx_SCL_GPIO_MUX                GPIO_MUX_4

#define I2Cx_SDA_GPIO_CLK                CRM_GPIOB_PERIPH_CLOCK
#define I2Cx_SDA_GPIO_PIN                GPIO_PINS_7
#define I2Cx_SDA_GPIO_PinsSource         GPIO_PINS_SOURCE7
#define I2Cx_SDA_GPIO_PORT               GPIOB
#define I2Cx_SDA_GPIO_MUX                GPIO_MUX_4

#define I2Cx_DMA_TX_Channel              DMA1_CHANNEL1
#define I2Cx_DMA_TX_DMAMUX_Channel       DMA1MUX_CHANNEL1
#define I2Cx_DMA_TX_DMAREQ               DMAMUX_DMAREQ_ID_I2C1_TX
#define I2Cx_DMA_TX_IRQn                 DMA1_Channel1_IRQn

#define I2Cx_DMA_RX_Channel              DMA1_CHANNEL2
#define I2Cx_DMA_RX_DMAMUX_Channel       DMA1MUX_CHANNEL2
#define I2Cx_DMA_RX_DMAREQ               DMAMUX_DMAREQ_ID_I2C1_RX
#define I2Cx_DMA_RX_IRQn                 DMA1_Channel2_IRQn

#define I2Cx_EVT_IRQn                    I2C1_EVT_IRQn
#define I2Cx_ERR_IRQn                    I2C1_ERR_IRQn



typedef enum
{
	UMSG_REQUEST_I2C_EMPTY = 0,
	UMSG_REQUEST_I2C_SOFT,
	UMSG_REQUEST_I2C_HARD,
	UMSG_REQUEST_I2C_WRITE,
	UMSG_REQUEST_I2C_READ,
	UMSG_REQUEST_I2C_SLAVEREAD,
	UMSG_REQUEST_I2C_SLAVEWRITE,
}msg_i2c_te;


void mg_hi2c_init(msg_class_ts *msg);
void mg_hi2c_write(msg_class_ts *msg);
void mg_hi2c_read(msg_class_ts *msg);
void mg_hi2c_slavewrite(msg_class_ts *msg);
void mg_hi2c_slaveread(msg_class_ts *msg);

#endif


