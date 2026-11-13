
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "board.h"
#include "osal_board.h"
#include "osal_board_api.h"


struct task_info{
    const char *name;
    uint8_t prio;
    uint16_t core_tick;
    uint16_t stack_size;
    uint16_t qsize;
};

const struct task_info task_info_table[] = {
	{"led_task",	        3,	30,	2048,	10},
    {"msg_task",	        3,	30,	2048,	10},
    {"func_msg_task",	    3,	30,	2048,	10},
};

void *k_task[10];
void *sys_mq_handle[10];


void *sys_mq_handle_get(const char *name)
{
	bool flag = FALSE;
	for(int i = 0;i<sizeof(task_info_table)/sizeof(struct task_info);i++)
	{
		if(strcmp(name,task_info_table[i].name) == 0)
		{
			return sys_mq_handle[i];
		}
	}
	if(flag == FALSE)
	{
		return NULL;
	}
	return NULL;
}

int task_create(void (*task)(void *p), void *p, const char *name)
{
	int i = 0;
	bool flag = FALSE;
	for(i = 0;i<sizeof(task_info_table)/sizeof(struct task_info);i++)
	{
		if(strcmp(name,task_info_table[i].name) == 0)
		{
			flag = TRUE;
			break;
		}
	}
	if(flag == FALSE)
	{
		return -1;
	}
    if(xTaskCreate((TaskFunction_t)task,task_info_table[i].name,
                       task_info_table[i].stack_size / 4,0,task_info_table[i].prio, k_task[i])!= pdPASS)
    {
        printf("Task could not be created as there was insufficient heap memory remaining.\n");
    }
    else
    {
        printf("Task was created successfully.\n");
    }
						
	//sys_mq_handle[i] = (void *)xQueueCreate(task_info_table[i].qsize, task_info_table[i].qsize);				
	return 0;
}
    
uint16_t os_task_queue_post_type(const char *name,int argc, int *argv)
{
	int buff[6];
	int len = 0;
    int  tmp = 0;
	len = argc*4;
	if(len>24)
	{
		len = 24;
	}
	if(argv == 0)
	{
		return -1;
	}
	void* mq_t = sys_mq_handle_get(name);
	vTaskResume(xTaskGetHandle(name));
	memcpy(buff,argv,len);
	if(len != 0 && mq_t != NULL )
	{
#if 1
        tmp = xQueueSendToFront(mq_t, buff, NULL);
#else
        tmp = xQueueSendToFrontFromISR(mq_t, buff, NULL);
#endif
	}
    if (tmp == 1) {
        return 0;
    } else if (tmp == 0) {
        return -11;
    }
	return 0;
}



