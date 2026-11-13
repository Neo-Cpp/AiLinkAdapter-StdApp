
#include <string.h>
#include "board.h"
#include "al_board.h"

#define MCU_ID1                     (0x1FFFF7E8)
#define MCU_ID2                     (0x1FFFF7EC)
#define MCU_ID3                     (0x1FFFF7F0)

#define PRODUCT_NAME                "AiAdapter_Tool"
#define PRODUCT_HWVERSION           "HW1.0.0"
#define PRODUCT_SWVERSION           "SW1.0.0"

typedef  struct {
	uint8_t		ProductName[32];	///< Name
	uint8_t		FirmwareVersion[10];	///< firmware version
	uint8_t		HardwareVersion[10];	///< hardware version
}_dev_board_info_t, *_pdev_board_info_t;

static void al_board_id(uint32_t *p0,uint32_t *p1,uint32_t *p2)
{
    if(!p0 && !p1 && !p2)
    {
        return ;
    }
    *p0 = *(uint32_t*)MCU_ID1;
    *p1 = *(uint32_t*)MCU_ID2;
    *p2 = *(uint32_t*)MCU_ID3;
}

static void al_board_info(_pdev_board_info_t bsp_info)
{
    memcpy(bsp_info->ProductName,PRODUCT_NAME,sizeof(PRODUCT_NAME));
    memcpy(bsp_info->HardwareVersion,PRODUCT_HWVERSION,sizeof(PRODUCT_HWVERSION));
    memcpy(bsp_info->FirmwareVersion,PRODUCT_SWVERSION,sizeof(PRODUCT_SWVERSION));
}


void mg_get_device_id(msg_class_ts *msg)
{
	uint32_t mcu_id[3];
    uint8_t data[12];
    uint8_t i = 0;
    
    al_board_id(&mcu_id[0],&mcu_id[1],&mcu_id[2]);
    
    data[i++] = mcu_id[0] >> 24;
    data[i++] = mcu_id[0] >> 16;
    data[i++] = mcu_id[0] >> 8;
    data[i++] = mcu_id[0];
    
    data[i++] = mcu_id[1] >> 24;
    data[i++] = mcu_id[1] >> 16;
    data[i++] = mcu_id[1] >> 8;
    data[i++] = mcu_id[1];
    
    data[i++] = mcu_id[2] >> 24;
    data[i++] = mcu_id[2] >> 16;
    data[i++] = mcu_id[2] >> 8;
    data[i++] = mcu_id[2];
    
	msg->size = i;
	msg->status = MSG_GET_DEVICE_ID_OK;
	msg->lParam = data;
    USBSendMsg(msg);
}

void mg_get_device_info(msg_class_ts *msg)
{
    _dev_board_info_t dev_info;
    uint8_t data[255];
    uint8_t i = 0;
    al_board_info(&dev_info);

    memcpy(data,dev_info.ProductName,sizeof(PRODUCT_NAME));
    i+= sizeof(PRODUCT_NAME);
    memcpy(data,dev_info.FirmwareVersion,sizeof(PRODUCT_HWVERSION));
    i+= sizeof(PRODUCT_HWVERSION);
    memcpy(data,dev_info.HardwareVersion,sizeof(PRODUCT_SWVERSION));
    i+= sizeof(PRODUCT_SWVERSION);
    
	msg->size = i;
	msg->status = 0;
	msg->lParam = data;
    USBSendMsg(msg);
}


