/*********************************************************************
Copyright:Anxin Ming Co.,Ltd. 
Author: Jason.anming
Date:2013-4-21
Description:stm32f4xx vi2c header
**********************************************************************/
#ifndef _VI2C_H_
#define _VI2C_H_

#include <all_include.h>

/*****************************ºê¶¨Òå**********************************/
#define VSDA_L GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_RESET)
#define VSDA_H GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET)

#define VSCL_L GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_RESET)
#define VSCL_H GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_SET)

#define VSDA_I GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)

/*****************************º¯Êý************************************/
void vi2c_init(void);

void vi2c_start(void);

void vi2c_stop(void);

u8 vi2c_send_byte(u8 data);

u8 vi2c_recv_byte(void);

void vi2c_ask(u8 ask);

u8 vi2c_wask(void);


#endif
