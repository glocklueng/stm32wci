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
    GPIO_InitTypeDef gpio;	/*gpio初始化结构体*/
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	/*gpiod总线的时钟使能*/
    
	/*初始化gpiod的12引脚为输出，做led显示使用*/
	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_Speed = GPIO_Speed_25MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
    
	GPIO_Init(GPIOC, &gpio);
}
//启动总线函数
void vi2c_start(void)
{
    VSDA_H;
    Delay(2);   /*发送起始条件的数据信号*/
    VSCL_H;
    delay_us(4);  /*起始条件的建立时间大于4.7us延时*/
    VSDA_L;     /*发送起始信号*/
    delay_us(4);  /*其实条件锁定时间大于4us*/
    VSCL_L;     /*拉低时钟准备发送数据*/
    Delay(2);
}

//结束总线函数
void vi2c_stop(void)
{
    VSDA_L;     /* 发送结束条件的数据信号 */
    delay_us(4);
    VSCL_H;     /* 发送结束条件的时钟信号 */
    delay_us(4);  /* 结束条件建立时间 */
    VSDA_H;     /* 发送总线结束信号 */
    delay_us(4);  /* 保持结束信号 */
}

//字节数据发送函数
u8 vi2c_send_byte(u8 data)
{
    u8 i;
    u8 ask;
    
    /* 发送长度为8位的数据信息 */
    for (i = 0; i < 8; i++)
    {
        delay_us(2);
        /* 判断发送的数据 */
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
        delay_us(2);  /* 保持数据信号时间 */
        VSCL_L;
    }
    delay_us(2);
    ask = vi2c_wask();
    
    return ask;
}

//字节数据接收
u8 vi2c_recv_byte(void)
{
    u8 i;
    u8 data = 0;
    
    //PC0,PC1
    GPIO_InitTypeDef gpio;	/*gpio初始化结构体*/
    
//    VSDA_I;   /* 释放SDA端口控制 */
	/*初始化gpiod的12引脚为输出，做led显示使用*/
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

//应答子函数
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

//字节数据接收
u8 vi2c_wask(void)
{
    u8 data = 0;
    
    //PC0,PC1
    GPIO_InitTypeDef gpio;	/*gpio初始化结构体*/
    
//    VSDA_I;   /* 释放SDA端口控制 */
	/*初始化gpiod的12引脚为输出，做led显示使用*/
	gpio.GPIO_Pin = GPIO_Pin_0;
	gpio.GPIO_Mode = GPIO_Mode_IN;
	gpio.GPIO_Speed = GPIO_Speed_25MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_UP;
    
	GPIO_Init(GPIOC, &gpio);
    VSDA_H;     /* 数据线释放，准备接收应答 */
    delay_us(4);
    VSCL_H;     /* 外部数据的输入有效 */
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










