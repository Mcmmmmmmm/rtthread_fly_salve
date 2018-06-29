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

const rt_uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0xb0,0x43,0x10,0x10,0x01}; //���͵�ַ
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
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//ʹ��ͨ��0���Զ�Ӧ�� ACK
//		
//			reg = NRF_WRITE_REG+EN_RXADDR;
//			send_buffer[0] = 0x01;
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//ʹ��ͨ��0�Ľ��յ�ַ  
//  
//			reg = NRF_WRITE_REG+SETUP_RETR;
//			send_buffer[0] = 0xff;											//0x1a  10��
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//�����Զ��ط����ʱ��:4000us + 86us;����Զ��ط�����:15��
//					
//			reg = NRF_WRITE_REG+RF_CH;
//			send_buffer[0] = 40;											
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//����RFͨ��Ϊ40
//		
//			reg = NRF_WRITE_REG+RF_SETUP;
//			send_buffer[0] = 0x0f;											
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//����TX�������,0db����,2Mbps,���������濪��  

//			reg = NRF_WRITE_REG+CONFIG;
//			send_buffer[0] = 0x0e;											
//			rt_spi_send_then_send(&nrf_device, &reg, 1, &send_buffer, 1);	//����TX�������,0db����,2Mbps,���������濪�� 
			
			nrf24lxx_write_buf(NRF_WRITE_REG+TX_ADDR, (uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);	
			nrf24lxx_write_buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);

			nrf24lxx_write_reg(NRF_WRITE_REG+EN_AA,			0x01);		//ʹ��ͨ��0���Զ�Ӧ�� 
			nrf24lxx_write_reg(NRF_WRITE_REG+EN_RXADDR,		0x01); 		//ʹ��ͨ��0�Ľ��յ�ַ  
			nrf24lxx_write_reg(NRF_WRITE_REG+SETUP_RETR,		0xff);		//�����Զ��ط����ʱ��:4000us + 86us;����Զ��ط�����:15��
														//0x1a  10��
			nrf24lxx_write_reg(NRF_WRITE_REG+RF_CH,			40);		//����RFͨ��Ϊ40
			nrf24lxx_write_reg(NRF_WRITE_REG+RF_SETUP,		0x0f);  	//����TX�������,0db����,2Mbps,���������濪��   
			nrf24lxx_write_reg(NRF_WRITE_REG+CONFIG,			0x0e);    	//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
			
		break;
		
		case SET_RX_MODE:
			
			nrf24lxx_write_buf(NRF_WRITE_REG+RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ

			nrf24lxx_write_reg(NRF_WRITE_REG+RX_PW_P0,	RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	

			nrf24lxx_write_reg(NRF_WRITE_REG+EN_AA,		0x01);    //ʹ��ͨ��0���Զ�Ӧ�� 

			nrf24lxx_write_reg(NRF_WRITE_REG+EN_RXADDR,	0x01);//ʹ��ͨ��0�Ľ��յ�ַ 

			nrf24lxx_write_reg(NRF_WRITE_REG+RF_CH,		40);	     //����RFͨ��Ƶ��

			nrf24lxx_write_reg(NRF_WRITE_REG+RF_SETUP,	0x0f);//����TX�������,0db����,2Mbps,���������濪�� 

			nrf24lxx_write_reg(NRF_WRITE_REG+CONFIG, 	0x0F);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC 
		
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
				rt_kprintf("IQR_PIN ����!!!\n");
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
				nrf24lxx_read_buf(RD_RX_PLOAD, buf, RX_PLOAD_WIDTH);//��ȡ����
				nrf24lxx_write_reg(FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
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

	
	//���豸SPI2 �� 0 �� ���ص�SPI2��
	ret = rt_spi_bus_attach_device(&nrf_device, "spi10", "spi1", (void *)&spi_cs);
	
	if(ret != RT_EOK)
	{
		return -RT_ERROR;
	}
	
	{
		struct rt_spi_configuration cfg;
		
		cfg.data_width			= 8;
		cfg.mode				= RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;	//RT_SPI_MODE_0
		cfg.max_hz				= 15000000; //24L01 ���Ϊ16Mhz 16000000
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
























