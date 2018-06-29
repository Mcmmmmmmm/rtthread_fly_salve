#include "app_command.h"




extern btn_cmd cmd;

void rt_command_data(void *p)
{
	while(1)
	{
		switch(cmd.data)
		{
			case up:
				rt_kprintf("enter up\n");
				cmd.data = 0;
			
				break;	
			case down:
				rt_kprintf("enter down\n");
				cmd.data = 0;
				break;
			case left:
				rt_kprintf("enter left\n");
				cmd.data = 0;
				break;
			case right:
				rt_kprintf("enter right\n");
				cmd.data = 0;
				break;
		}			
	}
}



int rt_command_data_init(void)
{
	rt_thread_t thread;
	
	thread = rt_thread_create("nrf_receive_data",
								rt_command_data ,	
								RT_NULL,
								4096,
								23,
								10);
	
	
	if(thread == RT_NULL)
	{
		return RT_ERROR;
	}
	
	rt_thread_startup(thread);
	
	
	return RT_EOK;
}
INIT_APP_EXPORT(rt_command_data_init);























