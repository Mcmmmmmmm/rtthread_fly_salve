#include "app_nrf24l01.h"

rt_uint8_t tmp_buf = {0};


btn_cmd cmd;


void rt_nrf_receive_data(void *p)
{
	while(1)
	{
		if(nrf24lxx_data_packet(SET_RX_MODE, &tmp_buf)==0)
		{
			cmd.data = tmp_buf;
			rt_kprintf("tmp_buf = %d\n", tmp_buf);  //数据打印不了
			rt_memset(&tmp_buf, 0x00, sizeof(tmp_buf)); 
		}
		rt_thread_delay(10);
		
	}
}


int rt_nrf_receive_data_init(void)
{
	rt_thread_t thread;
	
	thread = rt_thread_create("nrf_receive_data",
								rt_nrf_receive_data,	
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
INIT_APP_EXPORT(rt_nrf_receive_data_init);























