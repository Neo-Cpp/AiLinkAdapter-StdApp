
#include <stdbool.h>
#include "board.h"
#include "al_board.h"
#include "al_gpio_func.h"
#include "al_key_func.h"

#define SCL(n)  (1<<n)  	
#define SDA(n)  (1<<n)  

#define OLD_VERSIONIOPIN    GPIO_PINS_2   

static  gpio_type *     GPIO_PORT_ARRAY[] = {
GPIOC,GPIOC,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA,
GPIOB,GPIOB,GPIOB,GPIOC,GPIOC,GPIOA,GPIOB,GPIOB,
};

static const uint16_t    GPIO_PIN_ARRAY[]  = {
GPIO_PINS_0,GPIO_PINS_1,OLD_VERSIONIOPIN,GPIO_PINS_3,
GPIO_PINS_4,GPIO_PINS_5,GPIO_PINS_6,GPIO_PINS_7,
GPIO_PINS_1,GPIO_PINS_2,GPIO_PINS_12,GPIO_PINS_6,
GPIO_PINS_7,GPIO_PINS_10,GPIO_PINS_8,GPIO_PINS_9,
};

static uint16_t GPIO_MODE[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0
};

static uint16_t GPIO_KEY[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0
};

static const key_tbl_t key_table = {
    .scan_cnt = KEY_SCAN_TIMES,
    .up_cnt   = KEY_UP_TIMES,
    .long_cnt = KEY_LONG_TIMES,
    .hold_cnt = KEY_LONG_HOLD_TIMES,
};

static const uint8_t key_config_table[] = {
    NO_KEY,                     //[0]   None
    KEY_PLAY,                   //[1]   P/P
    KEY_POWER,                  //[2]   PWR
    KEY_NEXT_VOL_UP,            //[3]   NEXT/VOL+           ??NEXT??, ??VOL+
    KEY_PREV_VOL_DOWN,          //[4]   PREV/VOL-           ??PREV,     ??VOL-
    KEY_NEXT_VOL_DOWN,          //[5]   NEXT/VOL-           ??NEXT,     ??VOL-
    KEY_PREV_VOL_UP,            //[6]   PREV/VOL+           ??PREV??, ??VOL+
    KEY_VOL_UP_NEXT,            //[7]   VOL+/NEXT           ??VOL+,     ??NEXT
    KEY_VOL_DOWN_PREV,          //[8]   VOL-/PREV           ??VOL-,     ??PREV
    KEY_VOL_DOWN_NEXT,          //[9]   VOL-/NEXT           ??VOL-,     ??NEXT
    KEY_VOL_UP_PREV,            //[10]  VOL+/PREV           ??VOL+,     ??PREV
    KEY_VOL_UP,                 //[11]  VOL+
    KEY_VOL_DOWN,               //[12]  VOL-
    KEY_NEXT,                   //[13]  NEXT
    KEY_PREV,                   //[14]  PREV
    KEY_MODE,                   //[15]  MODE
    KEY_HSF,                    //[16]  HSF
    KEY_MODE_PWR,               //[17]  MODE/PWR            ??MODE,     ??POWER
    KEY_SIRI,                   //[18]  SIRI
    KEY_HOME,                   //[19]  IOS HOME
    KEY_PLAY_PWR_USER_DEF,      //[20]  PP/PWR/USER_DEF     ??PLAY,     ??POWER,  ??/??/???????
    KEY_PLAY_USER_DEF,          //[21]  PP/USER_DEF         ??PLAY,                 ??/??/???????
    KEY_VOL_UP_DOWN,            //[22]  VOL+/VOL-           ??VOL+,     ??VOL-
};


typedef struct
{
    uint16_t tHD_STA;
    uint16_t tSU_STA;
    uint16_t tLOW;
    uint16_t tHIGH;
    uint16_t tSU_DAT;
    uint16_t tSU_STO;
    uint16_t tDH;
    uint16_t tDH_DAT;
    uint16_t tAA;
    uint16_t tR;
    uint16_t tF;
	uint16_t tBuf;	
	uint8_t  tACK[4];	
	uint16_t tStart;	
	uint16_t tStop;							
}g_i2c_time_t;

typedef struct
{
	uint32_t    speed;        
	uint8_t 	controlMode;  
	uint8_t 	subAddrWidth; 
	uint8_t 	slaveAddrMode; 
	uint8_t     MasterMode;		
}g_i2c_t;


static g_i2c_t SIIC_Info[20];	
static g_i2c_time_t	swi2c_time[20];
static gpio_type* SCL_GPIOx[20];
static gpio_type* SDA_GPIOx[20];
static uint8_t SCL_GPIO_PINx[20];
static uint8_t SDA_GPIO_PINx[20];



static void al_gpio_setinput(uint8_t index,gpio_pull_type pull)// ok
{
    gpio_init_type gpio_init_struct;
    
    if(index > 15)
        return;
    
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    
    /* set default parameter */
    gpio_default_para_init(&gpio_init_struct);

    /* configure the gpio */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pins = GPIO_PIN_ARRAY[index];
    gpio_init_struct.gpio_pull = pull;
    gpio_init(GPIO_PORT_ARRAY[index], &gpio_init_struct); 
    GPIO_MODE[index] = 1;
}


static void al_gpio_setoutput(uint8_t index,gpio_pull_type pull) // ok
{
    gpio_init_type gpio_init_struct;
    
    if(index > 15)
        return;
    
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    
    /* set default parameter */
    gpio_default_para_init(&gpio_init_struct);

    /* configure the gpio */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_pins = GPIO_PIN_ARRAY[index];
    gpio_init_struct.gpio_pull = pull;
    gpio_init(GPIO_PORT_ARRAY[index], &gpio_init_struct);
    GPIO_MODE[index] = 2;
}

static void al_gpio_ctl_pin(uint16_t pin,confirm_state bit_state)// ok
{
    //printf("ctl_pin = %x state= %d\n",pin,bit_state);
    volatile uint16_t value = 0;
    for(int i = 0;i < 16;i++)
    {
        //uint8_t count = 0;
        if(pin & 1<<i)
        {
            if(GPIO_MODE[i] == 2)
            {
                //do
                //{
                    gpio_bits_write(GPIO_PORT_ARRAY[i],GPIO_PIN_ARRAY[i],bit_state);
                    //value = gpio_output_data_bit_read(GPIO_PORT_ARRAY[i],GPIO_PIN_ARRAY[i]) << i;
                    //count++;
                //}while(value != (bit_state << i) && count < 0xFE);
            }
        }
    }
}


static volatile uint16_t al_gpio_read_pin(void) // ok
{
    static volatile uint16_t pre_value = 0;
    volatile uint16_t value = 0;
    for(int i = 0;i < 16;i++)
    {
        if(GPIO_MODE[i] == 1)
        {
            value |= gpio_input_data_bit_read(GPIO_PORT_ARRAY[i],GPIO_PIN_ARRAY[i]) << i;
        }
        if(GPIO_MODE[i] == 2)
        {
            value |= gpio_output_data_bit_read(GPIO_PORT_ARRAY[i],GPIO_PIN_ARRAY[i]) << i;
        }
    }
    pre_value = value;
    return value;
}
static const gpio_type* GPIOx[] = {GPIOA,GPIOB,GPIOC,GPIOD};

void SetSCL(uint8_t channel)      {SCL_GPIOx[channel]->scr |= SCL(SCL_GPIO_PINx[channel]);}
void ResetSCL(uint8_t channel)    {SCL_GPIOx[channel]->clr |= SCL(SCL_GPIO_PINx[channel]);}
 
void SetSDA(uint8_t channel)      {SDA_GPIOx[channel]->scr |= SDA(SDA_GPIO_PINx[channel]);}
void ResetSDA(uint8_t channel)    {SDA_GPIOx[channel]->clr |= SDA(SCL_GPIO_PINx[channel]);}
  
uint8_t ReadSDA(uint8_t channel)  {return((SDA_GPIOx[channel]->idt&SDA(SDA_GPIO_PINx[channel]))>>SDA_GPIO_PINx[channel]);}



//public
void mg_gpio_setkey(msg_class_ts *msg)
{
    
}

void mg_gpio_getkeystate(msg_class_ts *msg)
{
    
}

void mg_gpio_setinput(msg_class_ts *msg)
{
    uint8_t *pData = msg->lParam;
    al_gpio_setinput(pData[0],pData[1] & 0x03);
    USBSendStatus(MSG_DEVICE_OK);
}

void mg_gpio_setoutput(msg_class_ts *msg)
{
    uint8_t *pData = msg->lParam;
    al_gpio_setoutput(pData[0],pData[1] & 0x03);
    USBSendStatus(MSG_DEVICE_OK);
}

void mg_gpio_ctlpin(msg_class_ts *msg)
{
    uint8_t *pData = msg->lParam;
    uint16_t pin = 0;
    uint8_t state = 0;
    
    pin = pData[0] << 8 | pData[1];
    state = pData[2];
    
    al_gpio_ctl_pin(pin,state);
    USBSendStatus(MSG_DEVICE_OK);
}
void mg_gpio_readpin(msg_class_ts *msg)
{
    uint8_t data[255];
    uint8_t i = 0;
    uint16_t pin = 0;
    pin = al_gpio_read_pin();
    
    data[i++] = pin>>8;
    data[i++] = pin;
    
	msg->size = i;
	msg->status = 0;
	msg->lParam = data;
    USBSendMsg(msg);
}

void mg_gpio_pulse_set(msg_class_ts *msg)
{
//    uint8_t *pData = msg->lParam;
//    uint16_t pin = 0;
//    uint16_t pulsetime = 0;
//    uint8_t state = 0;
//    
//    pin = pData[0] << 8 | pData[1];
//    pulsetime = pData[2] << 8 | pData[3];
//    state = pData[4];
//    
//    al_gpio_ctl_pin(pin,!state);
//    
//    delay_ms(1);
//    
//    al_gpio_ctl_pin(pin,state);
//    
//    delay_ms(pulsetime);
//    
//    al_gpio_ctl_pin(pin,!state);
}


#if 1
extern uint16_t key_process(uint16_t key_val);
extern uint16_t key_multi_press_process(uint16_t key_val);

bool mg_bsp_key_port_pressed(uint8_t index)
{
    return (!(al_gpio_read_pin() >> index));
}

uint8_t get_iokey(uint8_t index)
{
    uint8_t key_val = NO_KEY;

	if (mg_bsp_key_port_pressed(index)) 
	{
		key_val = key_config_table[1];
	}
	return key_val;

}
uint8_t bsp_key_scan_do(void)
{
    uint8_t key_val = NO_KEY;
    
    if (key_val == NO_KEY) {
        key_val = get_iokey(0);
    }
    return key_val;
}

uint16_t bsp_key_process(uint16_t key_val)
{
    volatile uint16_t key_return = NO_KEY;

	key_return = key_process(key_val);

#if 1//USER_MULTI_PRESS_EN
    if (1)
	{//xcfg_cb.user_key_multi_press_en) {
        key_return = key_multi_press_process(key_return);
    }
#endif
    return key_return;
}

void bsp_key_scan(void)
{
    uint8_t key_val;
    uint16_t key = NO_KEY;

    key_val = bsp_key_scan_do();
	
    key = bsp_key_process(key_val);
    if ((key != NO_KEY))
	{
        //msg_enqueue(key);
		//push_bt_task_demo();
    }
}
uint8_t get_double_key_time(void)
{
    if(DOUBLE_KEY_TIME > 7) {
        return 60;
    } else {
        return (uint8_t)((uint8_t)(DOUBLE_KEY_TIME + 2) * 20 + 1);
    }
}

uint16_t key_process(uint16_t key_val)
{
	static uint8_t key_tmp = 0,key_flag = 0;
    static uint16_t scan_cnt_tmp = 0;;
    static uint16_t up_cnt_tmp = 0;
    static uint16_t long_cnt_tmp = 0;
    static uint16_t hold_cnt_tmp = 0;
	
	static uint16_t key_count = 0;
	uint16_t tmp = 0;
	if(key_val != 0 && key_tmp != key_val)
	{
		key_flag = 1;
		key_tmp = key_val;
	}
	if(key_flag && scan_cnt_tmp < key_table.scan_cnt)
	{
		if(key_tmp == key_val)
		{
			scan_cnt_tmp++;
		}
		
		if(key_val == 0)
		{
			scan_cnt_tmp = 0;
			key_flag = 0;
			key_tmp = 0;
		}
		return 0;
	}
	
	if(key_flag == 1)
	{
		if(key_val == 0)
		{
			if(up_cnt_tmp<key_table.up_cnt)
			{
				up_cnt_tmp++;
				return 0;
			}
			key_flag = 0;
			scan_cnt_tmp = 0;
			up_cnt_tmp = 0;
			long_cnt_tmp = 0;
			tmp = key_tmp | KEY_SHORT_UP;
			key_tmp = 0;
			return (tmp);
		}
		if(key_val != 0)
		{
			up_cnt_tmp = 0;
			if(long_cnt_tmp < key_table.long_cnt)
			{
				long_cnt_tmp++;
				if(long_cnt_tmp == 1)
				{
					return (key_tmp | KEY_SHORT);
				}
				return (0);
			}
			key_flag = 2;
		}
	}
	if(key_flag == 2)
	{
		if(key_val == 0)
		{
			if(up_cnt_tmp<key_table.up_cnt)
			{
				up_cnt_tmp++;
				return 0;
			}
			scan_cnt_tmp = 0;
			long_cnt_tmp = 0;
			up_cnt_tmp = 0;
			key_flag = 0;
			key_count = 0;
			if(hold_cnt_tmp >=key_table.hold_cnt){
				tmp = 0;
			}else{
				tmp = key_tmp | KEY_LONG_UP;
			}
			hold_cnt_tmp = 0;
			key_tmp = 0;
			if(hold_cnt_tmp == key_table.hold_cnt)
			{
				scan_cnt_tmp = 0;
				long_cnt_tmp = 0;
				up_cnt_tmp = 0;
				hold_cnt_tmp = 0;
				key_flag = 0;
				key_tmp = 0;
				key_count = 0;
				return 0;
			}
			return (tmp);
		}
		if(key_val != 0)
		{
			up_cnt_tmp = 0;
			if(hold_cnt_tmp < key_table.hold_cnt)
			{
				hold_cnt_tmp++;
				if(hold_cnt_tmp == 1)
				{
					return (key_tmp | KEY_LONG);
				}
				return (0);
			}
			if(key_count < key_table.hold_cnt - key_table.long_cnt)
			{
				key_count++;
				return 0;
			}
			key_count = 0;
			return (key_tmp | KEY_LHOLD);
		}
	}
	return 0;
}

uint16_t key_multi_press_process(uint16_t key_val)
{
    static uint16_t key_tmp = 0, key_flag = 0;
    static uint8_t key_cnt = 0;
    static uint8_t key_func = 0;

    if (!key_flag && ((key_val & 0x1f00) == KEY_SHORT_UP))
    {
        key_flag = 1;
        key_tmp = key_val;
    }

//    if (key_flag && (key_cnt < get_double_key_time()))
    if (key_flag && (key_cnt < get_double_key_time()))
    {
        key_cnt++;
        if (((key_val & 0x1f00) == KEY_SHORT_UP))
        {
            key_func++;
        }
        return 0;
    }
    
    if (key_flag)
    {
        switch (key_func)
        {
            case 1:
                key_tmp = (key_tmp & 0xff) | KEY_SHORT_UP;
                break;
            case 2:
                key_tmp = (key_tmp & 0xff) | KEY_DOUBLE;
                break;
            case 3:
                key_tmp = (key_tmp & 0xff) | KEY_THREE;
                break;
            default:
                break;
        }
        
        key_flag = 0;
        key_cnt = 0;
	
        return key_tmp;
    }

    key_func = 0;
    key_flag = 0;
    key_cnt = 0;
    key_tmp = key_val;
    return key_tmp;
}



#endif

