/*********************************************************************
Copyright:Anxin Ming Co.,Ltd. 
Author: Jason.anming
Date:2013-3-10
Description:stm32f4xx fm1702 functions
**********************************************************************/
#include <fm1702.h>

/*********************************************************************
Function: spi_init
Description: ��ʼ��SPI�Ķ˿�
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
    ע��SPI1��NSS�˿ں�JTDI����
        SPI3��NSS�˿�/I2S3��WS�˿ں�JTDI����
        SPI3��SCK�˿�/I2S3��CK�˿ں�JTDO����
        ���Խ���JTAGʹ��SWD���г����Debugging���߽���JTAG��SWD���г�������
**********************************************************************/
void spi_init(void)
{
    SPI_InitTypeDef spi_sinit;  /*SPI�ĳ�ʼ���ṹ��*/
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);/*ʹ��SPI1������ʱ��*/
        
    SPI_Cmd(SPI1, ENABLE);  /*SPI1ʹ��*/
    
    /*SPI_InitTypeDef�ṹ���ʼ������*/
    spi_sinit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;/*SPI�˿ڵ�ѡ��*/
    spi_sinit.SPI_Mode = SPI_Mode_Master;/*SPI����ģʽѡ��*/
    spi_sinit.SPI_DataSize = SPI_DataSize_8b;/*SPI�շ����ݴ�Сѡ��*/
    spi_sinit.SPI_CPOL = SPI_CPOL_High;/*ʱ�Ӽ���ѡ�� ���Դ���Э��û���ش�Ӱ�죩*/
    spi_sinit.SPI_CPHA = SPI_CPHA_1Edge;/*ʱ����λ���ܹ��������ֲ�ͬ�Ĵ���Э��������ݴ��� */
    spi_sinit.SPI_NSS = SPI_NSS_Soft;/*NSSƬѡ���Ӳ������ѡ��*/
    spi_sinit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;/*SPI��SCK�Ĳ����ʷ�Ƶ��ѡ��*/
    spi_sinit.SPI_FirstBit = SPI_FirstBit_MSB;/*��һλ����λ��MSB�������Чλ��*/

    SPI_Init(SPI1, &spi_sinit); /*��ʼ������SPI1*/
}
/*********************************************************************
Function: fm1702_byte_write
Description: �ֽڷ�������
Calls: 
            SPI_NSSInternalSoftwareConfig
            SPI_I2S_SendData
Table Accessed: 
Table Updated: 
Input:          
            address : д��fm1702�Ĵ�����ַ
            data    : д���ַ������
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
            address : ��ȡ���ݵĵ�ַ     
Output:     ��ַ����    
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

/* ͨ����Ϊ����ֵ�ж� */
 #define FALSE 0
 #define TRUE 1

 u8 recv_buf[24] = {0};
 u8 UID[5] = {0};
/****************************************************************/
/*����: Write_FIFO */
/*����: �ú���ʵ����FM1702��FIFO��д��x bytes����*/
/*����: count, ��д���ֽڵĳ���*/
/* buff, ָ���д�����ݵ�ָ��*/
/*���: N/A */
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
/*����: Read_FIFO */
/*����: �ú���ʵ�ִ�FM1702��FIFO�ж���x bytes����*/
/*����: buff, ָ��������ݵ�ָ��*/
/*���: N/A */
/****************************************************************/
 u8 read_fifo(u8 *buff)
 {
    u8 tmp;
    u8 i;
    
    tmp = fm1702_byte_read(FIFO_Length);/* ��ȡFIFO���ж����ֽ� */
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
        *(buff + i) = fm1702_byte_read(FIFO);/* ��ȡFIFO��������ݷ���ָ�봦 */
    }
    return tmp;
 }
/****************************************************************/
/*����: Clear_FIFO */
/*����: �ú���ʵ�����FM1702��FIFO������*/
/*����: N/A */
/*���: 
 * TRUE, FIFO�����*/
/* FALSE, FIFOδ�����*/
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
/*����: Command_Send */
/*����: �ú���ʵ����FM1702��������Ĺ���*/
/*����: count, ����������ĳ���*/
/* buff, ָ����������ݵ�ָ��*/
/* Comm_Set, ������*/
/*���: 
 * TRUE, �����ȷִ��*/
/* FALSE, ����ִ�д���*/
/****************************************************************/
 u8 command_send(u8 count, u8 *buff, u8 comm_set)
 {
   u32 j;
   u8 tmp;

   fm1702_byte_write(Command, 0x00);
   clear_fifo();
   write_fifo(count, buff); /* ������д��FIFO */

   fm1702_byte_write(Command, comm_set);  /* ����ִ�� */
   for (j = 0; j < RF_TIMEOUT; j++)/* ����Ƿ�ִ�� */
   {
        tmp = fm1702_byte_read(Command);
        if (0 == tmp)   
        {
            return TRUE;/* ִ�гɹ� */
        }
   }    
   return FALSE;    /* ִ��ʧ�� */
 }

/****************************************************************/
/*����: Judge_Req */
/*����: �ú���ʵ�ֶԿ�Ƭ��λӦ���źŵ��ж�*/
/*����: *buff, ָ��Ӧ�����ݵ�ָ��*/
/*���: 
 * TRUE, ��ƬӦ���ź���ȷ*/
/* FALSE, ��ƬӦ���źŴ���*/
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
/*����: request */
/*����: �ú���ʵ�ֶԷ���FM1702������Χ֮�ڵĿ�Ƭ��Request����*/
/*����: mode: ALL(�������FM1702������Χ֮�ڵĿ�Ƭ) */
/* STD(�����FM1702������Χ֮�ڴ���HALT״̬�Ŀ�Ƭ) */
/*���: */
/* FM1702_NOTAGERR: �޿�*/
/* FM1702_OK: Ӧ����ȷ*/
/* FM1702_REQERR: Ӧ�����*/
/****************************************************************/
 u8 request(u8 mode)
 {
    u8 tmp = 0;
    
    recv_buf[0] = mode;
    fm1702_byte_write(TxControl, 0x58);/* �ط�������� */
    fm1702_byte_write(TxControl, 0x5b);/* ����������� */

    fm1702_byte_write(Bit_Frame, 0x07);/* ����λ��֡��ʽ */
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
    read_fifo(recv_buf);/* ��ȡFIFO��������ݷ���ָ�봦 */
    tmp = judge_req(recv_buf);/* �Կ�Ƭ��λӦ���źŵ��жϷ���ֵ�Ƿ���ȷ */
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
/*����: Check_UID */
/*����: �ú���ʵ�ֶ��յ��Ŀ�Ƭ�����кŵ��ж�*/
/*����: N/A */
/*���: 
* TRUE: ���к���ȷ*/
/* FALSE: ���кŴ���*/
/****************************************************************/
u8 Check_UID(void)
{
	u8 temp;
	u8 i;
	temp = 0x00;
	for(i = 0; i < 5; i++)
	{
		temp = temp ^ UID[i];//�Կ���������㣬��������ͬΪ0����ͬΪ1
	}						//��ȷ���Ϊ0
	if(temp == 0)
	{
		return TRUE;
	}
	return FALSE;
}
 /****************************************************************/
/*����: AntiColl */
/*����: �ú���ʵ�ֶԷ���FM1702������Χ֮�ڵĿ�Ƭ�ķ���ͻ���*/
/*����: N/A */
/*���: FM1702_NOTAGERR: �޿�*/
/* FM1702_BYTECOUNTERR: �����ֽڴ���*/
/* FM1702_SERNRERR: ��Ƭ���к�Ӧ�����*/
/* FM1702_OK: ��ƬӦ����ȷ*/
/****************************************************************/
u8 anti_coll(void)
{
	u8 temp;
	u8 i;
	recv_buf[0] = RF_CMD_ANTICOL;    //дѰ��ͨѶ����93H
	recv_buf[1] = 0x20;              //дѰ��ͨѶ����20H
    fm1702_byte_write(ChannelRedundancy, 0x03); //address 22H  ѡ������У�����������
	temp = command_send(2, recv_buf, Transceive);//Transceive=0X1E����
	while(1)
	{
		if(temp == FALSE)
		{
			return(1);   //�޿�
		}	  
        temp = fm1702_byte_read(FIFO_Length);   //��0x04��ķ��ؿ��ų���
		if(temp == 0)
		{
			return FM1702_BYTECOUNTERR; //�����ֽڳ��ȴ���
		}
		read_fifo(recv_buf);           //��ȡ����  					
		for(i = 0; i < temp; i++)
		{
			UID[i] = recv_buf[i]; //����ʱ�Ĵ����еĿ��ŷ���UIDר�üĴ�����
		}		
		temp = Check_UID();			/* У���յ���UID */
		if(temp == FALSE)
		{
			return(FM1702_SERNRERR);
		}
		return(0);
	}
		return(0);
}

 /*************************************/
//�������ܣ���ȡ���кţ�ͨ����һ���IC����д�󼴿���UID�еõ� 
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
