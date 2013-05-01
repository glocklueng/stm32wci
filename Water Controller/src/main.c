/*********************************************************************
Copyright:Anxin Ming Co.,Ltd. 
Author: Jason.anming
Date:2013-3-10
Description:stm32f4xx main function
**********************************************************************/

#include <main.h>

/*********************************************************************
Function: main      
Description: Ӧ�ó��������ں���
Calls: 
		delay_init()		��ʱ��ʼ������
Table Accessed: 
Table Updated: 
Input:          
                 
Output:
Return:        
Others:      
**********************************************************************/
//#define _DS1302_DEBUG_
#define _GPIO_DEBUG_
//#define _RTC_DEBUG_
#define _PCF8563T_DEBUG_
u8 current_time = 0;
int main(void)
{
	GPIO_InitTypeDef gpio1;	/*gpio��ʼ���ṹ��*/
	RTC_InitTypeDef rtc0;	/*rtc��ʼ���ṹ��*/
	RTC_TimeTypeDef	rtct;	/*rtcʱ�����ݽṹ*/
	RTC_DateTypeDef	rtcd;	/*rtc�������ݽṹ*/
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);	/*gpiod���ߵ�ʱ��ʹ��*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	/*gpioa���ߵ�ʱ��ʹ��*/
    
#ifdef _RTC_DEBUG_
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);/*��PWRʱ��*/
    
    PWR_BackupAccessCmd(ENABLE);    /**/
    
#if 0
    RCC_BackupResetCmd(ENABLE);    /**/
    RCC_BackupResetCmd(DISABLE);    /**/
    
    RCC_LSEConfig(RCC_LSE_ON);/**/
    
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)    /**/
    {
    }
#endif   
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);     /*ʹ��ϵͳ�ڲ���32.65Mʱ��*/
    RCC_RTCCLKCmd(ENABLE);  /*ʹ��RTCCLK*/
    
    RTC_WaitForSynchro();    /**/
#endif
    
	delay_init(168);	/* 168MHZ set delay frequency*/			/*systickʱ�ӳ�ʼ��*/
	

#ifdef _GPIO_DEBUG_	/*��ʼ��gpiod��12����Ϊ�������led��ʾʹ��*/
	gpio1.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	gpio1.GPIO_Mode = GPIO_Mode_OUT;
	gpio1.GPIO_Speed = GPIO_Speed_25MHz;
	gpio1.GPIO_OType = GPIO_OType_PP;
	gpio1.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &gpio1);
#endif
	
#ifdef _DS1302_DEBUG_
	ds1302_init();		/* ��ʼ��ʵʱʱ�� */
#endif
	
#ifdef _RTC_DEBUG_	/*ʵʱʱ�Ӳ���*/
    
	rtc0.RTC_HourFormat = RTC_HourFormat_12;	/*ʱ�ӵ�Сʱ���ͣ���Ϊ12�ƺ�24��*/
	rtc0.RTC_AsynchPrediv = (uint32_t)0x80;
	rtc0.RTC_SynchPrediv = (uint32_t)0xFA;
	if (ERROR == RTC_Init(&rtc0))
    {
        GPIO_WriteBit(GPIOD, GPIO_Pin_13, Bit_SET);
        return -1;
    }
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

#ifdef _RTC_DEBUG_
        rtct.RTC_H12 = RTC_H12_AM;
        rtct.RTC_Hours = 5;
        rtct.RTC_Minutes = 20;
        rtct.RTC_Seconds = 10;
        RTC_SetTime(RTC_Format_BIN, &rtct);
        
        rtcd.RTC_WeekDay = RTC_Weekday_Monday;
        rtcd.RTC_Year = 12;
        rtcd.RTC_Month = RTC_Month_February;
        rtcd.RTC_Date = 10;
        RTC_SetDate(RTC_Format_BIN, &rtcd);
        #if 0
        RTC_DayLightSavingConfig();
        #endif
        memset(&rtct, 0, sizeof(rtct));
        memset(&rtcd, 0, sizeof(rtcd));
        RTC_GetTime(RTC_Format_BIN, &rtct);
		RTC_GetDate(RTC_Format_BIN, &rtcd);
        #if 0
		printf("time = %d	%d:%d:%d\n", rtct.RTC_H12, 
					rtct.RTC_Hours, rtct.RTC_Minutes, rtct.RTC_Seconds);
		printf("date = %d	%d:%d:%d\n", rtcd.RTC_WeekDay, 
					rtcd.RTC_Year, rtcd.RTC_Month, rtcd.RTC_Date);
#endif


#endif
#ifdef _PCF8563T_DEBUG_
        pcf8563t_init();
#endif
	}
	
	return 0;
}
