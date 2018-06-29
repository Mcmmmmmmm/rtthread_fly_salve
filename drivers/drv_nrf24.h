/*
 * @File:   drv_nrf24l01.c 
 * @Author: Dwyane 
 * @Date:   2018-6-26 14:48:19 
 * 
 * @LICENSE: 
 * The code owner is liuguang. Other personnel may 
 * not be used or modified.
 * 
 * Change Logs: 
 * Date           Author       Notes 
 * 2018-6-26      Dwyane     The first version. 
 */ 

#ifndef __DRV_NRF24_H_
#define __DRV_NRF24_H_

#include "rtthread.h"
#include "rtdevice.h"
#include "board.h"

/*

STM32F407VE
SPI1

VCC		3.3V
SCK		A5
MISO	A6
MOSI	A7
CE		A2
CSN		A3
IRQ		A4

*/

#define CS_PIN		26
#define CE_PIN		25
#define	IRQ_PIN		29

enum{
	
	SET_TX_MODE = 0,
	SET_RX_MODE,
};

// NRF24L01���ͽ������ݿ�ȶ���
#define TX_ADR_WIDTH                                  5   	//5�ֽڵĵ�ַ���
#define RX_ADR_WIDTH                                  5   	//5�ֽڵĵ�ַ���
#define TX_PLOAD_WIDTH                                32  	//32�ֽڵ��û����ݿ��
#define RX_PLOAD_WIDTH                                32  	//32�ֽڵ��û����ݿ��

//NRF24L01�Ĵ�����������
#define NRF_READ_REG                                  0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define NRF_WRITE_REG                                 0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define RD_RX_PLOAD                                   0x61  //��RX��Ч����,1~32�ֽ�
#define WR_TX_PLOAD                                   0xA0  //дTX��Ч����,1~32�ֽ�
#define FLUSH_TX                                      0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define FLUSH_RX                                      0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define REUSE_TX_PL                                   0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NOP                                           0xFF  //�ղ���,����������״̬�Ĵ���	 
//SPI(NRF24L01)�Ĵ�����ַ
#define CONFIG                                        0x00  //���üĴ�����ַ;bit0:1����ģʽ,0����ģʽ;bit1:��ѡ��;bit2:CRCģʽ;bit3:CRCʹ��;
                                                            //bit4:�ж�MAX_RT(�ﵽ����ط������ж�)ʹ��;bit5:�ж�TX_DSʹ��;bit6:�ж�RX_DRʹ��
#define EN_AA                                         0x01  //ʹ���Զ�Ӧ����  bit0~5,��Ӧͨ��0~5
#define EN_RXADDR                                     0x02  //���յ�ַ����,bit0~5,��Ӧͨ��0~5
#define SETUP_AW                                      0x03  //���õ�ַ���(��������ͨ��):bit1,0:00,3�ֽ�;01,4�ֽ�;02,5�ֽ�;
#define SETUP_RETR                                    0x04  //�����Զ��ط�;bit3:0,�Զ��ط�������;bit7:4,�Զ��ط���ʱ 250*x+86us
#define RF_CH                                         0x05  //RFͨ��,bit6:0,����ͨ��Ƶ��;
#define RF_SETUP                                      0x06  //RF�Ĵ���;bit3:��������(0:1Mbps,1:2Mbps);bit2:1,���书��;bit0:�������Ŵ�������
#define STATUS                                        0x07  //״̬�Ĵ���;bit0:TX FIFO����־;bit3:1,��������ͨ����(���:6);bit4,�ﵽ�����ط�
                                                            //bit5:���ݷ�������ж�;bit6:���������ж�;
#define MAX_TX  		                                  0x10  //�ﵽ����ʹ����ж�
#define TX_OK   		                                  0x20  //TX��������ж�
#define RX_OK   		                                  0x40  //���յ������ж�

#define OBSERVE_TX                                    0x08  //���ͼ��Ĵ���,bit7:4,���ݰ���ʧ������;bit3:0,�ط�������
#define CD                                            0x09  //�ز����Ĵ���,bit0,�ز����;
#define RX_ADDR_P0                                    0x0A  //����ͨ��0���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P1                                    0x0B  //����ͨ��1���յ�ַ,��󳤶�5���ֽ�,���ֽ���ǰ
#define RX_ADDR_P2                                    0x0C  //����ͨ��2���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P3                                    0x0D  //����ͨ��3���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P4                                    0x0E  //����ͨ��4���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define RX_ADDR_P5                                    0x0F  //����ͨ��5���յ�ַ,����ֽڿ�����,���ֽ�,����ͬRX_ADDR_P1[39:8]���;
#define TX_ADDR                                       0x10  //���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ��,RX_ADDR_P0��˵�ַ���
#define RX_PW_P0                                      0x11  //��������ͨ��0��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1                                      0x12  //��������ͨ��1��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P2                                      0x13  //��������ͨ��2��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3                                      0x14  //��������ͨ��3��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4                                      0x15  //��������ͨ��4��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P5                                      0x16  //��������ͨ��5��Ч���ݿ��(1~32�ֽ�),����Ϊ0��Ƿ�
#define NRF_FIFO_STATUS                               0x17  //FIFO״̬�Ĵ���;bit0,RX FIFO�Ĵ����ձ�־;bit1,RX FIFO����־;bit2,3,����
                                                            //bit4,TX FIFO�ձ�־;bit5,TX FIFO����־;bit6,1,ѭ��������һ���ݰ�.0,��ѭ��;


/* �ڲ����� ------------------------------------------------------------------*/
static rt_err_t nrf24lxx_check(void);
static rt_err_t nrf24lxx_write_buf(rt_uint8_t reg, rt_uint8_t *buf, rt_uint8_t len);
static rt_err_t nrf24lxx_read_buf(rt_uint8_t reg, rt_uint8_t *buf, rt_uint8_t len);
static rt_err_t nrf24lxx_write_reg(rt_uint8_t reg, rt_uint8_t value);
static rt_err_t nrf24lxx_set_mode(struct rt_spi_device *dev, rt_uint8_t cmd);

/* �ⲿ���� ------------------------------------------------------------------*/
rt_err_t nrf24lxx_data_packet(rt_uint8_t cmd, rt_uint8_t *buf);
rt_err_t rt_hw_nrf_init(void);


#endif

