/*********************************************************************
Copyright:Anxin Ming Co.,Ltd. 
Author: Jason.anming
Date:2013-3-10
Description:stm32f4xx fm1702 functions
**********************************************************************/
#include <fm1702.h>

/*********************************************************************
Function: spi_init
Description: 初始化SPI的端口
Calls: 
        RCC_APB2PeriphClockCmd
        SPI_Cmd
        SPI_Init
Table Accessed: 
Table Updated: 
Input:  NULL             
Output: NULL        
Return: NULL   
Others: 
    注：SPI1的NSS端口和JTDI复用
        SPI3的NSS端口/I2S3的WS端口和JTDI复用
        SPI3的SCK端口/I2S3的CK端口和JTDO复用
        可以禁用JTAG使用SWD进行程序的Debugging或者禁用JTAG和SWD进行程序运行
**********************************************************************/
void spi_init(void)
{
    SPI_InitTypeDef spi_sinit;  /*SPI的初始化结构体*/
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);/*使能SPI1的总线时钟*/
        
    SPI_Cmd(SPI1, ENABLE);  /*SPI1使能*/
    
    /*SPI_InitTypeDef结构体初始化配置*/
    spi_sinit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;/*SPI端口的选择*/
    spi_sinit.SPI_Mode = SPI_Mode_Master;/*SPI主从模式选择*/
    spi_sinit.SPI_DataSize = SPI_DataSize_8b;/*SPI收发数据大小选择*/
    spi_sinit.SPI_CPOL = SPI_CPOL_High;/*时钟极性选择 （对传输协议没有重大影响）*/
    spi_sinit.SPI_CPHA = SPI_CPHA_1Edge;/*时钟相位（能够配置两种不同的传输协议进行数据传输 */
    spi_sinit.SPI_NSS = SPI_NSS_Soft;/*NSS片选软件硬件控制选择*/
    spi_sinit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;/*SPI的SCK的波特率分频器选择*/
    spi_sinit.SPI_FirstBit = SPI_FirstBit_MSB;/*第一位数据位以MSB（最高有效位）*/

    SPI_Init(SPI1, &spi_sinit); /*初始化加载SPI1*/
}
/*********************************************************************
Function: fm1702_byte_write
Description: 字节发送数据
Calls: 
            SPI_NSSInternalSoftwareConfig
            SPI_I2S_SendData
Table Accessed: 
Table Updated: 
Input:          
            address : 写如fm1702寄存器地址
            data    : 写入地址的数据
Output:NULL     
Return:NULL        
Others:      
**********************************************************************/
void fm1702_byte_write(u8 address, u8 data)
{
    SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
    SPI_I2S_SendData(SPI1, address);
    SPI_I2S_SendData(SPI1, data);
    SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Reset);
}

/*********************************************************************
Function: 
Description: 
Calls: 
            SPI_NSSInternalSoftwareConfig
            SPI_I2S_ReceiveData
Table Accessed: 
Table Updated: 
Input:          
            address : 读取数据的地址     
Output:     地址数据    
Return:     data   
Others:      
**********************************************************************/
u8 fm1702_byte_read(u8 address)
{
    u8 data = 0;
    SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);
    SPI_I2S_SendData(SPI1, address);
    data = SPI_I2S_ReceiveData(SPI1);
    SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Reset);
    
    return data;
}

/* 通常作为返回值判断 */
 #define FALSE 0
 #define TRUE 1

 u8 recv_buf[24] = {0};
 u8 UID[5] = {0};
/****************************************************************/
/*名称: Write_FIFO */
/*功能: 该函数实现向FM1702的FIFO中写入x bytes数据*/
/*输入: count, 待写入字节的长度*/
/* buff, 指向待写入数据的指针*/
/*输出: N/A */
/****************************************************************/
 void write_fifo(u8 count, u8 *buff)
 {
    u8 i;

    for (i = 0; i < count; i++)
    {
        fm1702_byte_write(FIFO, *(buff + i));
    }
 }
/****************************************************************/
/*名称: Read_FIFO */
/*功能: 该函数实现从FM1702的FIFO中读出x bytes数据*/
/*输入: buff, 指向读出数据的指针*/
/*输出: N/A */
/****************************************************************/
 u8 read_fifo(u8 *buff)
 {
    u8 tmp;
    u8 i;
    
    tmp = fm1702_byte_read(FIFO_Length);/* 读取FIFO里有多少字节 */
    if (FALSE == tmp)
    {
        return tmp;
    }
    else if (tmp >= 24)
    {
        tmp = 24;        
    }
    for (i = 0; i < tmp; i++)
    {
        *(buff + i) = fm1702_byte_read(FIFO);/* 读取FIFO里面的数据放在指针处 */
    }
    return tmp;
 }
/****************************************************************/
/*名称: Clear_FIFO */
/*功能: 该函数实现清空FM1702中FIFO的数据*/
/*输入: N/A */
/*输出: 
 * TRUE, FIFO被清空*/
/* FALSE, FIFO未被清空*/
/****************************************************************/
 u8 clear_fifo(void)
 {
     u8 tmp;
     u32 i;

     tmp = fm1702_byte_read(Control);
     tmp |= 0x01;
     fm1702_byte_write(Control, tmp);
     for (i = 0; i < RF_TIMEOUT; i++)
     {
        tmp = fm1702_byte_read(FIFO_Length);
        if (0 == tmp)
        {
            return TRUE;
        }
     }
     return FALSE;
 }
/****************************************************************/
/*名称: Command_Send */
/*功能: 该函数实现向FM1702发送命令集的功能*/
/*输入: count, 待发送命令集的长度*/
/* buff, 指向待发送数据的指针*/
/* Comm_Set, 命令码*/
/*输出: 
 * TRUE, 命令被正确执行*/
/* FALSE, 命令执行错误*/
/****************************************************************/
 u8 command_send(u8 count, u8 *buff, u8 comm_set)
 {
   u32 j;
   u8 tmp;

   fm1702_byte_write(Command, 0x00);
   clear_fifo();
   write_fifo(count, buff); /* 把数据写入FIFO */

   fm1702_byte_write(Command, comm_set);  /* 命令执行 */
   for (j = 0; j < RF_TIMEOUT; j++)/* 检查是否执行 */
   {
        tmp = fm1702_byte_read(Command);
        if (0 == tmp)   
        {
            return TRUE;/* 执行成功 */
        }
   }    
   return FALSE;    /* 执行失败 */
 }

/****************************************************************/
/*名称: Judge_Req */
/*功能: 该函数实现对卡片复位应答信号的判断*/
/*输入: *buff, 指向应答数据的指针*/
/*输出: 
 * TRUE, 卡片应答信号正确*/
/* FALSE, 卡片应答信号错误*/
/****************************************************************/
 u8 judge_req(u8 *buff)
 {
    u8 tmp1, tmp2;

    tmp1 = *buff;
    tmp2 = *(buff + 1);
    if (((0x03 == tmp1) || (0x53 == tmp1)
        ||(0x04 == tmp1) || (0x05 == tmp1))
        && (0x00 == tmp2))
    {
        return TRUE;
    }
    return FALSE;
 }

/****************************************************************/
/*名称: request */
/*功能: 该函数实现对放入FM1702操作范围之内的卡片的Request操作*/
/*输入: mode: ALL(监测所以FM1702操作范围之内的卡片) */
/* STD(监测在FM1702操作范围之内处于HALT状态的卡片) */
/*输出: */
/* FM1702_NOTAGERR: 无卡*/
/* FM1702_OK: 应答正确*/
/* FM1702_REQERR: 应答错误*/
/****************************************************************/
 u8 request(u8 mode)
 {
    u8 tmp = 0;
    
    recv_buf[0] = mode;
    fm1702_byte_write(TxControl, 0x58);/* 关发射控制器 */
    fm1702_byte_write(TxControl, 0x5b);/* 开发射控制器 */

    fm1702_byte_write(Bit_Frame, 0x07);/* 调整位的帧格式 */
    tmp = fm1702_byte_read(Control); /*  */
    tmp &= 0xf7;
    fm1702_byte_write(Control, tmp);
    fm1702_byte_write(ChannelRedundancy, 0x03);
    tmp = command_send(1, recv_buf, Transceive);
    if (FALSE == tmp)
    {
        return FM1702_NOTAGERR;
    }
    memset(recv_buf, 0, sizeof(recv_buf));
    read_fifo(recv_buf);/* 读取FIFO里面的数据放在指针处 */
    tmp = judge_req(recv_buf);/* 对卡片复位应答信号的判断返回值是否正确 */
    if (TRUE == tmp)
    {
        return FM1702_OK;
    }
    else
    {
        return FM1702_REQERR;
    }

 }
/*-----------------------------------------------------------------------------------*/
/****************************************************************/
/*名称: Check_UID */
/*功能: 该函数实现对收到的卡片的序列号的判断*/
/*输入: N/A */
/*输出: 
* TRUE: 序列号正确*/
/* FALSE: 序列号错误*/
/****************************************************************/
u8 Check_UID(void)
{
	u8 temp;
	u8 i;
	temp = 0x00;
	for(i = 0; i < 5; i++)
	{
		temp = temp ^ UID[i];//对卡号异或运算，即二数相同为0，不同为1
	}						//正确结果为0
	if(temp == 0)
	{
		return TRUE;
	}
	return FALSE;
}
 /****************************************************************/
/*名称: AntiColl */
/*功能: 该函数实现对放入FM1702操作范围之内的卡片的防冲突检测*/
/*输入: N/A */
/*输出: FM1702_NOTAGERR: 无卡*/
/* FM1702_BYTECOUNTERR: 接收字节错误*/
/* FM1702_SERNRERR: 卡片序列号应答错误*/
/* FM1702_OK: 卡片应答正确*/
/****************************************************************/
u8 anti_coll(void)
{
	u8 temp;
	u8 i;
	recv_buf[0] = RF_CMD_ANTICOL;    //写寻卡通讯命令93H
	recv_buf[1] = 0x20;              //写寻卡通讯命令20H
    fm1702_byte_write(ChannelRedundancy, 0x03); //address 22H  选择数据校验种类和类型
	temp = command_send(2, recv_buf, Transceive);//Transceive=0X1E命令
	while(1)
	{
		if(temp == FALSE)
		{
			return(1);   //无卡
		}	  
        temp = fm1702_byte_read(FIFO_Length);   //读0x04里的返回卡号长度
		if(temp == 0)
		{
			return FM1702_BYTECOUNTERR; //接收字节长度错误
		}
		read_fifo(recv_buf);           //读取卡号  					
		for(i = 0; i < temp; i++)
		{
			UID[i] = recv_buf[i]; //把临时寄存器中的卡号放在UID专用寄存器中
		}		
		temp = Check_UID();			/* 校验收到的UID */
		if(temp == FALSE)
		{
			return(FM1702_SERNRERR);
		}
		return(0);
	}
		return(0);
}

 /*************************************/
//函数功能：获取序列号，通常在一般对IC卡读写后即可在UID中得到 
/*************************************/
u8 get_id(void)
{
    u8 st;
    st= anti_coll();

    if(0 != st)				 
    {
        return (1);
    }
    return (0);
}

/*-----------------------------------------------------------------------------------*/


/*********************************************************************
Function: 
Description: 
Calls: 
Table Accessed: 
Table Updated: 
Input:          
                 
Output:         
Return:        
Others:      
**********************************************************************/

/*********************************************************************
Function: 
Description: 
Calls: 
Table Accessed: 
Table Updated: 
Input:          
                 
Output:         
Return:        
Others:      
**********************************************************************/

/*********************************************************************
Function: 
Description: 
Calls: 
Table Accessed: 
Table Updated: 
Input:          
                 
Output:         
Return:        
Others:      
**********************************************************************/

/*********************************************************************
Function: 
Description: 
Calls: 
Table Accessed: 
Table Updated: 
Input:          
                 
Output:         
Return:        
Others:      
**********************************************************************/

/*********************************************************************
Function: 
Description: 
Calls: 
Table Accessed: 
Table Updated: 
Input:          
                 
Output:         
Return:        
Others:      
**********************************************************************/
