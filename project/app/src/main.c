/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "board.h"
#include "at32f402_405_clock.h"
#include "usb_conf.h"
#include "usb_core.h"
#include "usbd_int.h"
#include "cdc_class.h"
#include "cdc_desc.h"
#include "audio_class.h"
#include "audio_desc.h"
#include "audio_codec.h"
#include "FreeRTOS.h"
#include "task.h"
#include "osal_board.h"


/** @addtogroup AT32F405_periph_examples
  * @{
  */
#define  WDT_TIMEOUT_MS  800

void led_task_function(void *p);
void msg_task_function(void *p);
void run_msg_task_function(void *p);
/** @addtogroup 405_USB_device_msc USB_device_msc
  * @{
  */

/* usb global struct define */
otg_core_type otgfs_core_struct;
otg_core_type otghs_core_struct;

void usb_clock48m_select(usb_clk48_s clk_s);
void usb_gpio_config(void);
void usb_low_power_wakeup_config(void);


/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
    nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x6000);
    
    system_clock_config();
    
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

    at32_board_init();
    
    /* qspi config */
    qspi_config();
    
    /* switch to cmd port */
    qspi_xip_enable(QSPI1, FALSE);
    
    /* set sclk */
    qspi_clk_division_set(QSPI1, QSPI_CLK_DIV_4);
    
    /* set sck idle mode 0 */
    qspi_sck_mode_set(QSPI1, QSPI_SCK_MODE_0);
    
    /* set wip in bit 0 */
    qspi_busy_config(QSPI1, QSPI_BUSY_OFFSET_0);
    
    /* enable auto ispc */
    qspi_auto_ispc_enable(QSPI1);
    
    /*configure xip mode*/
    en25qh128a_qspi_xip_init();
    
    if(crm_flag_get(CRM_WDT_RESET_FLAG) != RESET)
    {
        /* reset from wdt */
        crm_flag_clear(CRM_WDT_RESET_FLAG);

        at32_led_on(LED2);
    }
    else
    {
        /* reset from other mode */
        at32_led_off(LED2);
    }
    
    /* disable register write protection */
    wdt_register_write_enable(TRUE);

    /* set the wdt divider value */
    wdt_divider_set(WDT_CLK_DIV_4);
    
    /* set reload value*/
    wdt_reload_value_set((WDT_TIMEOUT_MS * 10) - 1);

    /* reload wdt counter */
    wdt_counter_reload();
  
    /* enable wdt */
    wdt_enable();
    
    /* init usart1 */
    uart_print_init(1500000);

    /* usb gpio config */
    usb_gpio_config();

#ifdef USB_LOW_POWER_WAKUP
    usb_low_power_wakeup_config();
#endif
    /* enable otg clock */
    crm_periph_clock_enable(OTGHS_CLOCK, TRUE);
    crm_periph_clock_enable(OTGFS_CLOCK, TRUE);

    /* select usb 48m clcok source */
    usb_clock48m_select(USB_CLK_HEXT);

    /* enable otg irq */
    nvic_irq_enable(OTGHS_IRQn, 0, 0);
    nvic_irq_enable(OTGFS1_IRQn, 0, 0);

            
    usbd_init(&otgfs_core_struct,
            USBFS_SPEED_CORE_ID,
            USBFS_ID,
            &cdc_class_handler,
            &cdc_desc_handler);        
    /* enter critical */
    taskENTER_CRITICAL();
    
    task_create(led_task_function,NULL,"led_task");
    
    task_create(msg_task_function,NULL,"msg_task");
    
    task_create(run_msg_task_function,NULL,"func_msg_task");
    
    /* exit critical */
    taskEXIT_CRITICAL();

    /* start scheduler */
    vTaskStartScheduler();      
    while(1);
}
/* led task function */
void led_task_function(void *p)
{
    while(1)
    {
        wdt_counter_reload();
        at32_led_toggle(LED3);
        vTaskDelay(300);
    }
}
extern uint8_t usbd_get_rx_completed(void);
extern void usbd_clear_rx_completed(void);
extern void usb_getmsg(uint8_t *pdata);
/* led task function */
extern uint8_t *pRx_Buffer_Func(void);
void msg_task_function(void *p)
{
    int flag = 0;
    
    while(1)
    {
        flag = usbd_get_rx_completed();
        if(flag)
        {
            usb_getmsg(pRx_Buffer_Func());
            usbd_clear_rx_completed();
        }
        vTaskDelay(1);
    }
}

void run_msg_task_function(void *p)
{
    while(1)
    {
        vTaskDelay(500);
    }
}
/**
  * @brief  usb 48M clock select
  * @param  clk_s:USB_CLK_HICK, USB_CLK_HEXT
  * @retval none
  */
void usb_clock48m_select(usb_clk48_s clk_s)
{
  if(clk_s == USB_CLK_HICK)
  {
    crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_HICK);

    /* enable the acc calibration ready interrupt */
    crm_periph_clock_enable(CRM_ACC_PERIPH_CLOCK, TRUE);

    /* update the c1\c2\c3 value */
    acc_write_c1(7980);
    acc_write_c2(8000);
    acc_write_c3(8020);

    /* open acc calibration */
    acc_calibration_mode_enable(ACC_CAL_HICKTRIM, TRUE);
  }
  else
  {
    crm_pllu_output_set(TRUE);
    /* wait till pll is ready */
    while(crm_flag_get(CRM_PLLU_STABLE_FLAG) != SET)
    {
    }
    crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_PLLU);
  }
}

/**
  * @brief  this function config gpio.
  * @param  none
  * @retval none
  */
void usb_gpio_config(void)
{
  gpio_init_type gpio_init_struct;

  crm_periph_clock_enable(OTGFS_PIN_GPIO_CLOCK, TRUE);
  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

#ifdef USB_SOF_OUTPUT_ENABLE
  crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
  gpio_init_struct.gpio_pins = OTG_PIN_SOF;
  gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
  gpio_pin_mux_config(OTG_PIN_SOF_GPIO, OTG_PIN_SOF_SOURCE, OTG_PIN_MUX);
#endif

  /* otgfs use vbus pin */
#ifndef USB_VBUS_IGNORE
  gpio_init_struct.gpio_pins = OTG_PIN_VBUS;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_pin_mux_config(OTG_PIN_GPIO, OTG_PIN_VBUS_SOURCE, OTG_PIN_MUX);
  gpio_init(OTG_PIN_GPIO, &gpio_init_struct);
#endif

}
#ifdef USB_LOW_POWER_WAKUP
/**
  * @brief  usb low power wakeup interrupt config
  * @param  none
  * @retval none
  */
void usb_low_power_wakeup_config(void)
{
  exint_init_type exint_init_struct;

  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  exint_default_para_init(&exint_init_struct);

  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPT;
  exint_init_struct.line_select = OTG_WKUP_EXINT_LINE;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(OTG_WKUP_IRQ, 0, 0);
}

/**
  * @brief  this function handles otg wakup interrupt.
  * @param  none
  * @retval none
  */
void OTGHS_WKUP_HANDLER(void)
{
    exint_flag_clear(OTGHS_WKUP_EXINT_LINE);
}

/**
  * @brief  this function handles otg wakup interrupt.
  * @param  none
  * @retval none
  */
void OTGFS_WKUP_HANDLER(void)
{
    exint_flag_clear(OTGFS_WKUP_EXINT_LINE);
}

#endif

/**
  * @brief  this function handles otg interrupt.
  * @param  none
  * @retval none
  */
void OTGHS_IRQ_HANDLER(void)
{
    usbd_irq_handler(&otghs_core_struct);
}

/**
  * @brief  this function handles otg interrupt.
  * @param  none
  * @retval none
  */
void OTGFS_IRQ_HANDLER(void)
{
    usbd_irq_handler(&otgfs_core_struct);
}

/**
  * @brief  usb delay millisecond function.
  * @param  ms: number of millisecond delay
  * @retval none
  */
void usb_delay_ms(uint32_t ms)
{
    /* user can define self delay function */
    delay_ms(ms);
}

/**
  * @brief  usb delay microsecond function.
  * @param  us: number of microsecond delay
  * @retval none
  */
void usb_delay_us(uint32_t us)
{
    delay_us(us);
}

/**
  * @}
  */

/**
  * @}
  */
