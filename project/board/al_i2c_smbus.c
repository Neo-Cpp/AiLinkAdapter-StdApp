#include "board.h"
#include "at32f402_405_clock.h"
#include "i2c_application.h"
#include "al_i2c_smbus.h"


static i2c_type *I2C_PORT[] = {I2C3};

i2c_handle_type hi2cx;

static i2c_status_type i2c_status;

/**
  * @brief  error handler program
  * @param  i2c_status
  * @retval none
  */
void error_handler(uint32_t error_code)
{
  //while(1)
  {
    //at32_led_toggle(LED2);
    //delay_ms(500);
  }
}

static void al_hi2c_init(void)
{
    hi2cx.i2cx = I2Cx_PORT;
    /* i2c config */
    i2c_config(&hi2cx);
}

static void al_hi2c_write_dma(uint16_t SlaveAddr,uint32_t SubAddr,uint8_t *pWriteData,uint16_t Len)
{
    /* start the request reception process */
    if((i2c_status = i2c_master_transmit_dma(&hi2cx, SlaveAddr, pWriteData, Len, I2C_TIMEOUT)) != I2C_OK)
    {
        error_handler(i2c_status);
    }   
//    /* wait for the communication to end */
//    if(i2c_wait_end(&hi2cx, I2C_TIMEOUT) != I2C_OK)
//    {
//        error_handler(i2c_status);
//    }    
}

static void al_i2c_read_dma(uint16_t SlaveAddr,uint32_t SubAddr,uint8_t *pReadData,uint16_t Len)
{
    /* start the request reception process */
    if((i2c_status = i2c_master_receive_dma(&hi2cx, SlaveAddr, pReadData, Len, I2C_TIMEOUT)) != I2C_OK)
    {
        error_handler(i2c_status);
    }   
    /* wait for the communication to end */
//    if(i2c_wait_end(&hi2cx, I2C_TIMEOUT) != I2C_OK)
//    {
//        error_handler(i2c_status);
//    }
}

static void al_i2c_slave_write_dma(uint8_t* pWriteData, uint16_t Len)
{
    if((i2c_status = i2c_slave_transmit_dma(&hi2cx, pWriteData, Len, I2C_TIMEOUT)) != I2C_OK)
    {
        error_handler(i2c_status);
    }

    /* wait for the communication to end */
//    if(i2c_wait_end(&hi2cx, I2C_TIMEOUT) != I2C_OK)
//    {
//        error_handler(i2c_status);
//    }   
}

static void al_i2c_slave_read_dma(uint8_t* pReadData, uint16_t Len)
{
    /* start the transmission process */
    if((i2c_status = i2c_slave_receive_dma(&hi2cx, pReadData, Len, I2C_TIMEOUT)) != I2C_OK)
    {
        error_handler(i2c_status);
    }

    /* wait for the communication to end */
//    if(i2c_wait_end(&hi2cx, I2C_TIMEOUT) != I2C_OK)
//    {
//        error_handler(i2c_status);
//    }  
}

//void al_smbus_write(void)
//{
//    if((i2c_status = i2c_smbus_master_transmit(&hi2cx, I2Cx_ADDRESS, tx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK)
//    {
//        error_handler(i2c_status);
//    }    
//}

//void al_smbus_read(void)
//{
//    /* start the request reception process */
//    if((i2c_status = i2c_smbus_master_receive(&hi2cx, I2Cx_ADDRESS, rx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK)
//    {
//        error_handler(i2c_status);
//    }
//}

//void al_smbus_slave_write(void)
//{
//    if((i2c_status = i2c_smbus_slave_transmit(&hi2cx, tx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK)
//    {
//        error_handler(i2c_status);
//    }
//}
//void al_smbus_slave_read(void)
//{
//    /* start the transmission process */
//    if((i2c_status = i2c_smbus_slave_receive(&hi2cx, rx_buf, BUF_SIZE, I2C_TIMEOUT)) != I2C_OK)
//    {
//        error_handler(i2c_status);
//    }  
//}
/**
  * @brief  initializes peripherals used by the i2c.
  * @param  none
  * @retval none
  */
void i2c_lowlevel_init(i2c_handle_type* hi2c)
{
  gpio_init_type gpio_init_structure;

  if(hi2c->i2cx == I2Cx_PORT)
  {
    /* i2c periph clock enable */
    crm_periph_clock_enable(I2Cx_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SCL_GPIO_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SDA_GPIO_CLK, TRUE);

    /* gpio configuration */
    gpio_pin_mux_config(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_GPIO_PinsSource, I2Cx_SCL_GPIO_MUX);

    gpio_pin_mux_config(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_GPIO_PinsSource, I2Cx_SDA_GPIO_MUX);

    /* configure i2c pins: scl */
    gpio_init_structure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_structure.gpio_mode           = GPIO_MODE_MUX;
    gpio_init_structure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_init_structure.gpio_pull           = GPIO_PULL_UP;//GPIO_PULL_NONE;

    gpio_init_structure.gpio_pins           = I2Cx_SCL_GPIO_PIN;
    gpio_init(I2Cx_SCL_GPIO_PORT, &gpio_init_structure);

    /* configure i2c pins: sda */
    gpio_init_structure.gpio_pins           = I2Cx_SDA_GPIO_PIN;
    gpio_init(I2Cx_SDA_GPIO_PORT, &gpio_init_structure);

    /* configure and enable i2c interrupt */
    nvic_irq_enable(I2Cx_EVT_IRQn, 0, 0);
    nvic_irq_enable(I2Cx_ERR_IRQn, 0, 0);

    /* configure and enable i2c dma channel interrupt */
    nvic_irq_enable(I2Cx_DMA_TX_IRQn, 0, 0);
    nvic_irq_enable(I2Cx_DMA_RX_IRQn, 0, 0);

    /* i2c dma tx and rx channels configuration */
    /* enable the dma clock */
    crm_periph_clock_enable(I2Cx_DMA_CLK, TRUE);

    /* i2c dma channel configuration */
    hi2c->dma_tx_channel = I2Cx_DMA_TX_Channel;
    hi2c->dma_rx_channel = I2Cx_DMA_RX_Channel;

    dma_reset(hi2c->dma_tx_channel);
    dma_reset(hi2c->dma_rx_channel);

    hi2c->dma_init_struct.peripheral_base_addr    = (uint32_t)&hi2c->i2cx->txdt;
    hi2c->dma_init_struct.memory_base_addr        = 0;
    hi2c->dma_init_struct.direction               = DMA_DIR_MEMORY_TO_PERIPHERAL;
    hi2c->dma_init_struct.buffer_size             = 0xFFFF;
    hi2c->dma_init_struct.peripheral_inc_enable   = FALSE;
    hi2c->dma_init_struct.memory_inc_enable       = TRUE;
    hi2c->dma_init_struct.peripheral_data_width   = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    hi2c->dma_init_struct.memory_data_width       = DMA_MEMORY_DATA_WIDTH_BYTE;
    hi2c->dma_init_struct.loop_mode_enable        = FALSE;
    hi2c->dma_init_struct.priority                = DMA_PRIORITY_LOW;

    dma_init(hi2c->dma_tx_channel, &hi2c->dma_init_struct);
    dma_init(hi2c->dma_rx_channel, &hi2c->dma_init_struct);

    dmamux_init(I2Cx_DMA_TX_DMAMUX_Channel, I2Cx_DMA_TX_DMAREQ);
    dmamux_init(I2Cx_DMA_RX_DMAMUX_Channel, I2Cx_DMA_RX_DMAREQ);

    dmamux_enable(I2Cx_DMA, TRUE);

    /* config i2c */
    i2c_init(hi2c->i2cx, 0x0F, I2Cx_CLKCTRL);

    i2c_own_address1_set(hi2c->i2cx, I2C_ADDRESS_MODE_7BIT, I2Cx_ADDRESS);
  }
}

//public
void mg_hi2c_init(msg_class_ts *msg)
{

}

void mg_hi2c_write(msg_class_ts *msg)
{

}

void mg_hi2c_read(msg_class_ts *msg)
{

}

void mg_hi2c_slavewrite(msg_class_ts *msg)
{

}

void mg_hi2c_slaveread(msg_class_ts *msg)
{

}


