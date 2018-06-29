/*
 * @File:   drv_nrf24l01.c 
 * @Author: Dwyane 
 * @Date:   2017-10-02 23:48:19 
 * 
 * @LICENSE: 
 * The code owner is liuguang. Other personnel may 
 * not be used or modified.
 * 
 * Change Logs: 
 * Date           Author       Notes 
 *
 * 2018-6-26      Dwyane	   The first version.
 */ 

#include "drv_nrf24.h"

static struct rt_spi_device			nrf_device;	

struct 
{
	rt_uint32_t pin;
	
}spi_cs;

const rt_uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0xb0,0x43,0x10,0x10,0x01}; //发送地址
const rt_uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0xb0,0x43,0x10,0x10,0x01};

extern rt_uint8_t tmp_buf[20];


static rt_err_t nrf24lxx_check(void)
{
	
	rt_uint8_t	send_buf[5] = {0xA5,0xA5,0xA5,0xA5,0xA5};
	rt_uint8_t  read_buf[5] = {0};
	rt_uint8_t	i;
	
	nrf24lxx_write_buf(NRF_WRITE_REG | TX_ADDR, send_buf, 5);	//0x10 0x01
	
	nrf24lxx_read_buf (NRF_READ_REG  | TX_ADDR, read_buf, 5);	//0x0E 0x00 0x00 0x00 0x00
	
	for(i = 0; i < 5; i++)
	{
		rt_kprintf("rbuf = %d\n", read_buf[i]);
		if(read_buf[i]!= 0xA5)
		{
			return RT_ERROR;
		}
	}
	return RT_EOK;
	
}

static rt_err_t nrf24lxx_write_buf(rt_uint8_t reg, rt_uint8_t *buf, rt_uint8_t len)
{
	return rt_spi_send_then_send(&nrf_device, &reg, 1, buf, len);
}

static rt_err_t nrf24lxx_read_buf(rt_uint8_t reg, rt_uint8_t *buf, rt_uint8_t len)
{
	return rt_spi_send_then_recv(&nrf_device, &reg, 1, buf, len);
}

static rt_err_t nrf24lxx_write_reg(rt_uint8_t reg, rt_uint8_t value)
{
	rt_spi_send_then_send(&nrf_device, &reg, 1, &value, 1);
}

static rt_err_t nrf24lxx_read_reg(rt_uint8_t reg)
{
	
	rt_uint8_t data;
	rt_spi_send_then_recv(&nrf_device, &reg, 1, &data, 1);
	return data;
	
}

static rt_err_t nrf24lxx_set_mode(struct rt_spi_device *dev, rt_uint8_t cmd)
{
	rt_uint8_t send_buffer[4];
	rt_uint8_t reg;
	RT_ASSERT(dev != RT_NULL);
	
	rt_pin_write(CE_PIN, PIN_LOW);	
	
	switch(cmd)
	{
		
		case SET_TX_MODE:
//			nrf_write_buf(NRF_WRITE_REG+TX_ADDR, (uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);	
//			nrf_write_buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);
//		
//			reg = NRF_WRITE_REG+EN_AA;
//			send_buffer[0] = 0x01;
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//使能通道0的自动应答 ACK
//		
//			reg = NRF_WRITE_REG+EN_RXADDR;
//			send_buffer[0] = 0x01;
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//使能通道0的接收地址  
//  
//			reg = NRF_WRITE_REG+SETUP_RETR;
//			send_buffer[0] = 0xff;											//0x1a  10次
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//设置自动重发间隔时间:4000us + 86us;最大自动重发次数:15次
//					
//			reg = NRF_WRITE_REG+RF_CH;
//			send_buffer[0] = 40;											
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//设置RF通道为40
//		
//			reg = NRF_WRITE_REG+RF_SETUP;
//			send_buffer[0] = 0x0f;											
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//设置TX发射参数,0db增益,2Mbps,低噪声增益开启  

//			reg = NRF_WRITE_REG+CONFIG;
//			send_buffer[0] = 0x0e;											
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//设置TX发射参数,0db增益,2Mbps,低噪声增益开启 
			
			nrf24lxx_write_buf(NRF_WRITE_REG+TX_ADDR, (uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);	
			nrf24lxx_write_buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);

			nrf24lxx_write_reg(NRF_WRITE_REG+EN_AA,			0x01);		//使能通道0的自动应答 
			nrf24lxx_write_reg(NRF_WRITE_REG+EN_RXADDR,		0x01); 		//使能通道0的接收地址  
			nrf24lxx_write_reg(NRF_WRITE_REG+SETUP_RETR,		0xff);		//设置自动重发间隔时间:4000us + 86us;最大自动重发次数:15次
														//0x1a  10次
			nrf24lxx_write_reg(NRF_WRITE_REG+RF_CH,			40);		//设置RF通道为40
			nrf24lxx_write_reg(NRF_WRITE_REG+RF_SETUP,		0x0f);  	//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
			nrf24lxx_write_reg(NRF_WRITE_REG+CONFIG,			0x0e);    	//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
			
		break;
		
		case SET_RX_MODE:
			
			nrf24lxx_write_buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址

			nrf24lxx_write_reg(NRF_WRITE_REG+RX_PW_P0,	RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	

			nrf24lxx_write_reg(NRF_WRITE_REG+EN_AA,		0x01);    //使能通道0的自动应答 

			nrf24lxx_write_reg(NRF_WRITE_REG+EN_RXADDR,	0x01);//使能通道0的接收地址 

			nrf24lxx_write_reg(NRF_WRITE_REG+RF_CH,		40);	     //设置RF通信频率

			nrf24lxx_write_reg(NRF_WRITE_REG+RF_SETUP,	0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启 

			nrf24lxx_write_reg(NRF_WRITE_REG+CONFIG, 	0x0F);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC 
		
		break;	
			
	}
	
	rt_pin_write(CE_PIN, PIN_HIGH);
	return RT_EOK;
}

rt_err_t nrf24lxx_data_packet(rt_uint8_t cmd, rt_uint8_t *buf)
{
	rt_uint8_t sta;
	
	switch(cmd)
	{
		case SET_TX_MODE:
		{
			rt_pin_write(CE_PIN, PIN_LOW);
			
			nrf24lxx_write_buf(WR_TX_PLOAD, buf, TX_PLOAD_WIDTH);
			
			rt_pin_write(CE_PIN, PIN_HIGH);
			
			sta = rt_pin_read(IRQ_PIN);		//0x01
			
			rt_kprintf("sta = %d\n", sta);
			
			while((rt_pin_read(IRQ_PIN)) != 0)
			{
				rt_thread_delay(1000);
				rt_kprintf("IQR_PIN 进入!!!\n");
			}
			
			sta = nrf24lxx_read_reg(STATUS);			//0x0F
			
			nrf24lxx_write_reg(NRF_WRITE_REG + STATUS, sta);
			
			if(sta & MAX_TX)
			{
				nrf24lxx_write_reg(FLUSH_TX, 0xff);
				return MAX_TX;
			}
			if(sta & TX_OK)
			{
				return TX_OK;
			}
			
			return -RT_ERROR;
		}
		case SET_RX_MODE:
		{
			sta = nrf24lxx_read_reg(STATUS);

			nrf24lxx_write_reg(NRF_WRITE_REG+STATUS, sta);

			if(sta & RX_OK)
			{
				nrf24lxx_read_buf(RD_RX_PLOAD, buf, RX_PLOAD_WIDTH);//读取数据
				nrf24lxx_write_reg(FLUSH_RX,0xff);//清除RX FIFO寄存器 
				return RT_EOK; 
			}
			return -RT_ERROR;
		}
	}
	
}

rt_err_t rt_hw_nrf_init(void)
{
	rt_err_t ret;
	rt_uint8_t sta;
	
	spi_cs.pin	= CS_PIN;
	
	rt_pin_mode(CS_PIN, PIN_MODE_OUTPUT);
	rt_pin_mode(CE_PIN, PIN_MODE_OUTPUT);

	
	//将设备SPI2 第 0 个 挂载到SPI2上
	ret = rt_spi_bus_attach_device(&nrf_device, "spi10", "spi1", (void *)&spi_cs);
	
	if(ret != RT_EOK)
	{
		return -RT_ERROR;
	}
	
	{
		struct rt_spi_configuration cfg;
		
		cfg.data_width			= 8;
		cfg.mode				= RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;	//RT_SPI_MODE_0
		cfg.max_hz				= 15000000; //24L01 最大为16Mhz 16000000
											//SPI_BAUDRATEPRESCALER_8
											// polarity	= 0; phase = 1;
		rt_spi_configure(&nrf_device, &cfg);
		
	}
	
	{
		if(nrf24lxx_check())
		{
			rt_kprintf("nrf device not find!\n");
			return -RT_ERROR;
		}

		nrf24lxx_set_mode(&nrf_device, SET_RX_MODE);
	}
	
	return RT_EOK;
}

INIT_APP_EXPORT(rt_hw_nrf_init);
























