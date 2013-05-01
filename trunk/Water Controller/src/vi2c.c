/*********************************************************************
Copyright:Anxin Ming Co.,Ltd.
Author: Jason.anming
Date:2013-4-21
Description:stm32f4xx vi2c functions
**********************************************************************/

#include <vi2c.h>

void vi2c_init(void)
{
    //PC0,PC1
    GPIO_InitTypeDef gpio;	/*gpio��ʼ���ṹ��*/
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	/*gpiod���ߵ�ʱ��ʹ��*/
    
	/*��ʼ��gpiod��12����Ϊ�������led��ʾʹ��*/
	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_25MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
    
	GPIO_Init(GPIOC, &gpio);
}
//�������ߺ���
void vi2c_start(void)
{
    VSDA_H;
    Delay(2);   /*������ʼ�����������ź�*/
    VSCL_H;
    delay_us(4);  /*��ʼ�����Ľ���ʱ�����4.7us��ʱ*/
    VSDA_L;     /*������ʼ�ź�*/
    delay_us(4);  /*��ʵ��������ʱ�����4us*/
    VSCL_L;     /*����ʱ��׼����������*/
    Delay(2);
}

//�������ߺ���
void vi2c_stop(void)
{
    VSDA_L;     /* ���ͽ��������������ź� */
    delay_us(4);
    VSCL_H;     /* ���ͽ���������ʱ���ź� */
    delay_us(4);  /* ������������ʱ�� */
    VSDA_H;     /* �������߽����ź� */
    delay_us(4);  /* ���ֽ����ź� */
}

//�ֽ����ݷ��ͺ���
u8 vi2c_send_byte(u8 data)
{
    u8 i;
    u8 ask;
    
    /* ���ͳ���Ϊ8λ��������Ϣ */
    for (i = 0; i < 8; i++)
    {
        delay_us(2);
        /* �жϷ��͵����� */
        if ((data << i) & 0x80)
        {
            VSDA_H;
        }
        else
        {
            VSDA_L;
        }
        delay_us(4);
        VSCL_H;
        delay_us(2);  /* ���������ź�ʱ�� */
        VSCL_L;
    }
    delay_us(2);
    ask = vi2c_wask();
    
    return ask;
}

//�ֽ����ݽ���
u8 vi2c_recv_byte(void)
{
    u8 i;
    u8 data = 0;
    
    //PC0,PC1
    GPIO_InitTypeDef gpio;	/*gpio��ʼ���ṹ��*/
    
//    VSDA_I;   /* �ͷ�SDA�˿ڿ��� */
	/*��ʼ��gpiod��12����Ϊ�������led��ʾʹ��*/
	gpio.GPIO_Pin = GPIO_Pin_0;
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Speed = GPIO_Speed_25MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
    
	GPIO_Init(GPIOC, &gpio);
    for (i = 0; i < 8; i++)
    {
        VSCL_L;
        delay_us(4);
        VSCL_H;
        data <<= 1;
        delay_us(2);
        if (VSDA_I == 1)
        {
            data += 1;
        }
        delay_us(2);
    }
    VSCL_L;
    Delay(2);
    
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOC, &gpio);
   
    return data;
}

//Ӧ���Ӻ���
void vi2c_ask(u8 ask)
{
    VSCL_L;
    delay_us(2);
    if (ask >= 1)
    {
        VSDA_H;
    }
    else
    {
        VSDA_L;
    }
    delay_us(2);
    VSCL_H;
    delay_us(4);
    VSCL_L;
    Delay(3);
}

//�ֽ����ݽ���
u8 vi2c_wask(void)
{
    u8 data = 0;
    
    //PC0,PC1
    GPIO_InitTypeDef gpio;	/*gpio��ʼ���ṹ��*/
    
//    VSDA_I;   /* �ͷ�SDA�˿ڿ��� */
	/*��ʼ��gpiod��12����Ϊ�������led��ʾʹ��*/
	gpio.GPIO_Pin = GPIO_Pin_0;
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Speed = GPIO_Speed_25MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
    
	GPIO_Init(GPIOC, &gpio);
    VSDA_H;     /* �������ͷţ�׼������Ӧ�� */
    delay_us(4);
    VSCL_H;     /* �ⲿ���ݵ�������Ч */
    delay_us(4);
    if (VSDA_I == 1)
    {
        data = 0;  
    }
    else
    {
        data = 1;
    }
    VSCL_L;
    
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOC, &gpio);
    delay_us(2);
   
    return data;
}










