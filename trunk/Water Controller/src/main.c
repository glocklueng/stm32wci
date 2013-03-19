/*********************************************************************
Copyright:Anxin Ming Co.,Ltd. 
Author: Jason.anming
Date:2013-3-10
Description:stm32f4xx main function
**********************************************************************/

#include <main.h>

/*********************************************************************
Function: main      
Description: 应用程序的总入口函数
Calls: 
		delay_init()		延时初始化函数
Table Accessed: 
Table Updated: 
Input:          
                 
Output:
Return:        
Others:      
**********************************************************************/
//#define _DS1302_DEBUG_
#define _GPIO_DEBUG_
#define _RTC_DEBUG_
u8 current_time = 0;
int main(void)
{
	GPIO_InitTypeDef gpio1;	/*gpio初始化结构体*/
	RTC_InitTypeDef rtc0;	/*rtc初始化结构体*/
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	/*gpiod总线的时钟使能*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	/*gpioa总线的时钟使能*/
	delay_init(168);	/* 168MHZ set delay frequency*/			/*systick时钟初始化*/
	

#ifdef _GPIO_DEBUG_	/*初始化gpiod的12引脚为输出，做led显示使用*/
	gpio1.GPIO_Pin = GPIO_Pin_12;
	gpio1.GPIO_Mode = GPIO_Mode_OUT;
	gpio1.GPIO_Speed = GPIO_Speed_25MHz;
	gpio1.GPIO_OType = GPIO_OType_PP;
	gpio1.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &gpio1);
#endif
	
#ifdef _DS1302_DEBUG_
	ds1302_init();		/* 初始化实时时钟 */
#endif
	
#ifdef _RTC_DEBUG_	/*实时时钟测试*/
	rtc0.RTC_HourFormat = RTC_HourFormat_12;	/*时钟的小时类型，分为12制和24制*/
	rtc0.RTC_AsynchPrediv = (uint32_t)0x7F;
	rtc0.RTC_SynchPrediv = (uint32_t)0xFF;
	RTC_Init(&rtc0);
#endif
	
	
	while(1)
	{
#ifdef _DS1302_DEBUG_
		ds1302_write(0x8e, 0x00);
		ds1302_write(0x80, 0x00);
		current_time = ds1302_read(0x81);
		if (0 != (current_time & 0x80))
		{
			ds1302_write(0x80, 0x00);
			current_time = ds1302_read(0x81);
		}
#endif
#ifdef _GPIO_DEBUG_
		//GPIO_SetBits(GPIOD, GPIO_Pin_12);
		GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_RESET);
		delay_ms(500);
		GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_SET);
		//GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		delay_ms(500);
#endif
	}
	
	return 0;
}
