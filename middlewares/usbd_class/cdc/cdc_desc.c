/**
  **************************************************************************
  * @file     cdc_desc.c
  * @brief    usb cdc device descriptor
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
#include "stdio.h"
#include "usb_std.h"
#include "usbd_sdr.h"
#include "usbd_core.h"
#include "cdc_desc.h"

/** @addtogroup AT32F402_405_middlewares_usbd_class
  * @{
  */

/** @defgroup USB_cdc_desc
  * @brief usb device cdc descriptor
  * @{
  */

/** @defgroup USB_cdc_desc_private_functions
  * @{
  */

static usbd_desc_t *get_device_descriptor(void);
static usbd_desc_t *get_device_qualifier(void);
static usbd_desc_t *get_device_configuration(void);
static usbd_desc_t *get_device_other_speed(void);
static usbd_desc_t *get_device_lang_id(void);
static usbd_desc_t *get_device_manufacturer_string(void);
static usbd_desc_t *get_device_product_string(void);
static usbd_desc_t *get_device_serial_string(void);
static usbd_desc_t *get_device_interface_string(void);
static usbd_desc_t *get_device_config_string(void);
static usbd_desc_t *get_hs_device_configuration(void);

static uint16_t usbd_unicode_convert(uint8_t *string, uint8_t *unicode_buf);
static void usbd_int_to_unicode (uint32_t value , uint8_t *pbuf , uint8_t len);
static void get_serial_num(void);
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_desc_buffer[256] ALIGNED_TAIL;

/**
  * @brief device descriptor handler structure
  */
usbd_desc_handler cdc_desc_handler =
{
  get_device_descriptor,
  get_device_qualifier,
  get_device_configuration,
  get_device_other_speed,
  get_device_lang_id,
  get_device_manufacturer_string,
  get_device_product_string,
  get_device_serial_string,
  get_device_interface_string,
  get_device_config_string,
  get_hs_device_configuration,
};

/**
  * @brief usb device standard descriptor
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_descriptor[USB_DEVICE_DESC_LEN] ALIGNED_TAIL =
{
  USB_DEVICE_DESC_LEN,                   /* bLength */
  USB_DESCIPTOR_TYPE_DEVICE,             /* bDescriptorType */
  0x00,                                  /* bcdUSB */
  0x02,
  0x00,                                  /* bDeviceClass */
  0x00,                                  /* bDeviceSubClass */
  0x00,                                  /* bDeviceProtocol */
  USB_MAX_EP0_SIZE,                      /* bMaxPacketSize */
  LBYTE(USBD_CDC_VENDOR_ID),             /* idVendor */
  HBYTE(USBD_CDC_VENDOR_ID),             /* idVendor */
  LBYTE(USBD_CDC_PRODUCT_ID),            /* idProduct */
  HBYTE(USBD_CDC_PRODUCT_ID),            /* idProduct */
  0x00,                                  /* bcdDevice rel. 2.00 */
  0x02,
  USB_MFC_STRING,                        /* Index of manufacturer string */
  USB_PRODUCT_STRING,                    /* Index of product string */
  USB_SERIAL_STRING,                     /* Index of serial number string */
  1                                      /* bNumConfigurations */
};

/**
  * @brief usb configuration standard descriptor
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_configuration[USBD_CDC_CONFIG_DESC_SIZE] ALIGNED_TAIL =
{
  USB_DEVICE_CFG_DESC_LEN,               /* bLength: configuration descriptor size */
  USB_DESCIPTOR_TYPE_CONFIGURATION,      /* bDescriptorType: configuration */
  LBYTE(USBD_CDC_CONFIG_DESC_SIZE),          /* wTotalLength: bytes returned */
  HBYTE(USBD_CDC_CONFIG_DESC_SIZE),          /* wTotalLength: bytes returned */
  0x01,                                  /* bNumInterfaces: 2 interface */
  0x01,                                  /* bConfigurationValue: configuration value */
  0x00,                                  /* iConfiguration: index of string descriptor describing
                                            the configuration */
  0xe0,                                  /* bmAttributes: self powered */
  0xFA,                                  /* MaxPower 100 mA: this current is used for detecting vbus */

  USB_DEVICE_IF_DESC_LEN,                /* bLength: interface descriptor size */
  USB_DESCIPTOR_TYPE_INTERFACE,          /* bDescriptorType: interface descriptor type */
  0x00,                                  /* bInterfaceNumber: number of interface */
  0x00,                                  /* bAlternateSetting: alternate set */
  0x04,                                  /* bNumEndpoints: number of endpoints */
  0xDC,                                  /* bInterfaceClass: CDC class code */
  0xA0,                                  /* bInterfaceSubClass: subclass code, Abstract Control Model*/
  0xB0,                                  /* bInterfaceProtocol: protocol code, AT Command */
  0x00,                                  /* iInterface: index of string descriptor */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  0x05,      /* bDescriptorType: Endpoint */
  2,                        /* bEndpointAddress */
  0x03,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x0a,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  5,      /* bDescriptorType: Endpoint */
  0x82,                         /* bEndpointAddress */
  0x03,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x0a,                               /* bInterval: ignore for Bulk transfer */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  0x05,      /* bDescriptorType: Endpoint */
  1,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  5,      /* bDescriptorType: Endpoint */
  0x81,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x00, 
};


#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_hs_configuration[USBD_CDC_CONFIG_DESC_SIZE] ALIGNED_TAIL =
{
   USB_DEVICE_CFG_DESC_LEN,               /* bLength: configuration descriptor size */
  USB_DESCIPTOR_TYPE_CONFIGURATION,      /* bDescriptorType: configuration */
  LBYTE(USBD_CDC_CONFIG_DESC_SIZE),          /* wTotalLength: bytes returned */
  HBYTE(USBD_CDC_CONFIG_DESC_SIZE),          /* wTotalLength: bytes returned */
  0x01,                                  /* bNumInterfaces: 2 interface */
  0x01,                                  /* bConfigurationValue: configuration value */
  0x00,                                  /* iConfiguration: index of string descriptor describing
                                            the configuration */
  0xC0,                                  /* bmAttributes: self powered */
  0xFA,                                  /* MaxPower 100 mA: this current is used for detecting vbus */

  USB_DEVICE_IF_DESC_LEN,                /* bLength: interface descriptor size */
  USB_DESCIPTOR_TYPE_INTERFACE,          /* bDescriptorType: interface descriptor type */
  0x00,                                  /* bInterfaceNumber: number of interface */
  0x00,                                  /* bAlternateSetting: alternate set */
  0x02,                                  /* bNumEndpoints: number of endpoints */
  0xDC,                                  /* bInterfaceClass: CDC class code */
  0xA0,                                  /* bInterfaceSubClass: subclass code, Abstract Control Model*/
  0xB0,                                  /* bInterfaceProtocol: protocol code, AT Command */
  0x00,                                  /* iInterface: index of string descriptor */
  
  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  0x05,      /* bDescriptorType: Endpoint */
  1,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  5,      /* bDescriptorType: Endpoint */
  0x81,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x00,                               /* bInterval: ignore for Bulk transfer */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  0x05,      /* bDescriptorType: Endpoint */
  2,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  5,      /* bDescriptorType: Endpoint */
  0x82,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x00, 
};


#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_other_speed[USBD_CDC_CONFIG_DESC_SIZE] ALIGNED_TAIL =
{
  USB_DEVICE_CFG_DESC_LEN,               /* bLength: configuration descriptor size */
  USB_DESCIPTOR_TYPE_CONFIGURATION,      /* bDescriptorType: configuration */
  LBYTE(USBD_CDC_CONFIG_DESC_SIZE),          /* wTotalLength: bytes returned */
  HBYTE(USBD_CDC_CONFIG_DESC_SIZE),          /* wTotalLength: bytes returned */
  0x01,                                  /* bNumInterfaces: 2 interface */
  0x01,                                  /* bConfigurationValue: configuration value */
  0x00,                                  /* iConfiguration: index of string descriptor describing
                                            the configuration */
  0xC0,                                  /* bmAttributes: self powered */
  0xFA,                                  /* MaxPower 100 mA: this current is used for detecting vbus */

  USB_DEVICE_IF_DESC_LEN,                /* bLength: interface descriptor size */
  USB_DESCIPTOR_TYPE_INTERFACE,          /* bDescriptorType: interface descriptor type */
  0x00,                                  /* bInterfaceNumber: number of interface */
  0x00,                                  /* bAlternateSetting: alternate set */
  0x02,                                  /* bNumEndpoints: number of endpoints */
  0xDC,                                  /* bInterfaceClass: CDC class code */
  0xA0,                                  /* bInterfaceSubClass: subclass code, Abstract Control Model*/
  0xB0,                                  /* bInterfaceProtocol: protocol code, AT Command */
  0x00,                                  /* iInterface: index of string descriptor */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  0x05,      /* bDescriptorType: Endpoint */
  1,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  5,      /* bDescriptorType: Endpoint */
  0x81,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x00,                               /* bInterval: ignore for Bulk transfer */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  0x05,      /* bDescriptorType: Endpoint */
  2,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  5,      /* bDescriptorType: Endpoint */
  0x82,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LBYTE(64),  /* wMaxPacketSize: */
  HBYTE(64),
  0x00, 
};


/**
  * @brief usb device qualifier
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_qualifier[USB_DEVICE_QUALIFIER_DESC_LEN] ALIGNED_TAIL =
{
  USB_DEVICE_QUALIFIER_DESC_LEN,
  USB_DESCIPTOR_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @brief usb string lang id
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_string_lang_id[USBD_CDC_SIZ_STRING_LANGID] ALIGNED_TAIL =
{
  USBD_CDC_SIZ_STRING_LANGID,
  USB_DESCIPTOR_TYPE_STRING,
  0x09,
  0x04,
};

/**
  * @brief usb string serial
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_string_serial[USBD_CDC_SIZ_STRING_SERIAL] ALIGNED_TAIL =
{
  USBD_CDC_SIZ_STRING_SERIAL,
  USB_DESCIPTOR_TYPE_STRING,
};


/* device descriptor */
static usbd_desc_t device_descriptor =
{
  USB_DEVICE_DESC_LEN,
  g_usbd_descriptor
};

/* config descriptor */
static usbd_desc_t config_descriptor =
{
  USBD_CDC_CONFIG_DESC_SIZE,
  g_usbd_configuration
};


/* hs config descriptor */
static usbd_desc_t config_descriptor_hs =
{
  USBD_CDC_CONFIG_DESC_SIZE,
  g_usbd_hs_configuration
};

/* config othe speed */
static usbd_desc_t config_other_speed =
{
  USBD_CDC_CONFIG_DESC_SIZE,
  g_usbd_other_speed
};

/* langid descriptor */
static usbd_desc_t langid_descriptor =
{
  USBD_CDC_SIZ_STRING_LANGID,
  g_string_lang_id
};

/* serial descriptor */
static usbd_desc_t serial_descriptor =
{
  USBD_CDC_SIZ_STRING_SERIAL,
  g_string_serial
};

/* qualifier descriptor */
static usbd_desc_t device_qualifier =
{
  USB_DEVICE_QUALIFIER_DESC_LEN,
  g_usbd_qualifier,
};

static usbd_desc_t vp_desc;

/**
  * @brief  standard usb unicode convert
  * @param  string: source string
  * @param  unicode_buf: unicode buffer
  * @retval length
  */
static uint16_t usbd_unicode_convert(uint8_t *string, uint8_t *unicode_buf)
{
  uint16_t str_len = 0, id_pos = 2;
  uint8_t *tmp_str = string;

  while(*tmp_str != '\0')
  {
    str_len ++;
    unicode_buf[id_pos ++] = *tmp_str ++;
    unicode_buf[id_pos ++] = 0x00;
  }

  str_len = str_len * 2 + 2;
  unicode_buf[0] = (uint8_t)str_len;
  unicode_buf[1] = USB_DESCIPTOR_TYPE_STRING;

  return str_len;
}

/**
  * @brief  usb int convert to unicode
  * @param  value: int value
  * @param  pbus: unicode buffer
  * @param  len: length
  * @retval none
  */
static void usbd_int_to_unicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;

  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2 * idx] = (value >> 28) + '0';
  }
  else
  {
      pbuf[2 * idx] = (value >> 28) + 'A' - 10;
    }

    value = value << 4;

    pbuf[2 * idx + 1] = 0;
  }
}

/**
  * @brief  usb get serial number
  * @param  none
  * @retval none
  */
static void get_serial_num(void)
{
  uint32_t serial0, serial1, serial2;

  serial0 = *(uint32_t*)MCU_ID1;
  serial1 = *(uint32_t*)MCU_ID2;
  serial2 = *(uint32_t*)MCU_ID3;

  serial0 += serial2;

  if (serial0 != 0)
  {
    usbd_int_to_unicode (serial0, &g_string_serial[2] ,8);
    usbd_int_to_unicode (serial1, &g_string_serial[18] ,4);
  }
}

/**
  * @brief  get device descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_descriptor(void)
{
  return &device_descriptor;
}

/**
  * @brief  get device qualifier
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t * get_device_qualifier(void)
{
  return &device_qualifier;
}

/**
  * @brief  get config descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_configuration(void)
{
  return &config_descriptor;
}


/**
  * @brief  get hs config descriptor
  * @param  none
  * @retval usbd_desc                         
  */
static usbd_desc_t *get_hs_device_configuration(void)
{
  return &config_descriptor_hs;
}

/**
  * @brief  get other speed descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_other_speed(void)
{
  return &config_other_speed;
}

/**
  * @brief  get lang id descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_lang_id(void)
{
  return &langid_descriptor;
}


/**
  * @brief  get manufacturer descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_manufacturer_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_CDC_DESC_MANUFACTURER_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

/**
  * @brief  get product descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_product_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_CDC_DESC_PRODUCT_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

/**
  * @brief  get serial descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_serial_string(void)
{
//  get_serial_num();
//  return &serial_descriptor;
    
    vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_CDC_DESC_SERIAL_STRING, g_usbd_desc_buffer);
    vp_desc.descriptor = g_usbd_desc_buffer;
    return &vp_desc;
}

/**
  * @brief  get interface descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_interface_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_CDC_DESC_INTERFACE_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

/**
  * @brief  get device config descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_config_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_CDC_DESC_CONFIGURATION_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
