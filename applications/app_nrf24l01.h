#ifndef _APP_NRF24L01_H_
#define _APP_NRF24L01_H_

#include <rtthread.h>
#include <rtdevice.h>
#include "drv_nrf24.h"

enum
{
	up = 1,
	down,
	left,
	right
};

typedef struct
{
	rt_uint8_t data;
	
}btn_cmd;


void rt_nrf_receive_data(void *p);
int rt_nrf_receive_data_init(void);


#endif

