




#include "board.h"
#include "osal_board_api.h"

typedef int (*task_cb_ptr)(...); 

void task_cb_run(void)
{
	int buff[6];
	task_cb_ptr p_task = 0;
	void* mq_t = 0;//sys_mq_handle_get(csi_kernel_get_task_name());
	if(mq_t != 0)
	{
		if(1)//csi_kernel_msgq_get(mq_t,buff, 0) == 0)
		{
			p_task = (task_cb_ptr)buff[0];
			switch(buff[1] & 0x07)
			{
				case 0x01:
					p_task(buff[2]);
					break;
				case 0x02:
					p_task(buff[2],buff[3]);
					break; 
				case 0x03:
					p_task(buff[2],buff[3],buff[4]);
					break;
				case 0x04:
					p_task(buff[2],buff[3],buff[4],buff[5]);
					break;
				default:
					p_task();
					break;
			}
		}		
	}
}
