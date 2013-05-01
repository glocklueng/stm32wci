/*********************************************************************
Copyright:Anxin Ming Co.,Ltd. 
Author: Jason.anming
Date:2013-3-10
Description:stm32f4xx including all headers
**********************************************************************/

#ifndef _ALLINCLUDE_H_
#define _ALLINCLUDE_H_

#ifdef _WCAPI_
#define WCAPI
#else
#define WCAPI
#endif

/* IDE provided headers */
#include <stdio.h>
#include <string.h>
#include <stm32f4xx.h>      /*���ӿ����ݽṹ��*/
#include <stm32f4xx_gpio.h> /*����gpio�Ŀ�*/
#include <stm32f4xx_spi.h>  /*����SPI�Ŀ�*/

/* Water Controller System Headers */
#include <main.h>
#include <system.h>
#include <delay.h>

#include <ds1302.h>
#include <pcf8563t.h>
#include <fm1702.h>
#include <flash.h>
#include <seg_led.h>
#include <flower.h>
#include <relay.h>

#endif
