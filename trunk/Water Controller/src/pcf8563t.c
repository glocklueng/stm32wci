/*********************************************************************
Copyright:Anxin Ming Co.,Ltd.
Author: Jason.anming
Date:2013-4-21
Description:stm32f4xx pcf8563t functions
**********************************************************************/
#include <pcf8563t.h>
#include <vi2c.h>

/*********************************************************************
Function:pcf8563t_init
Description:pcf8563t芯片初始化
Calls:
GPIO_Init
Table Accessed:
Table Updated:
Input:
    dat     需要输出的数据
Output:
Return:
Others:
**********************************************************************/
u8 pcf8563t_init(void)
{
    vi2c_init();
    
    vi2c_start();
    vi2c_send_byte(0xa2);
    vi2c_send_byte(0x02);
    vi2c_start();
    vi2c_send_byte(0xa3);
    vi2c_recv_byte();
    vi2c_ask(1);
    vi2c_stop();

    return 0;
}
