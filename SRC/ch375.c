/*******************************************************************************
* CopyRight   : 
* File        : CH375.c
* Author      :
* Date        : 
* Description : CH375 ������������ 
* Version     :
*******************************************************************************/

/* Includes --------------------------------------------------*/
#include"CH375.h"

/* Variables ------------------------------------------------*/
uint8_t BlockPerSector;  //ÿ�������ݿ���
uint32_t BytePerSector;  //ÿ�����ֽ���
uint8_t DATA_BUFFER[8];  //���ݻ�����

/* Functions ------------------------------------------------*/
/******************************************************************************
* Function    �� CH375_DelayNus
* Description :  ��ʱ����
* Input       :  nCount
* Output      �� None
* Return      �� None
*******************************************************************************/
void CH375_DelayNus(__IO uint32_t nCount)
{
    uint32_t i;
    for(i=nCount;i!=0;i--);
}

/*******************************************************************************
* Funtion     : CH375_Configuration
* Description : CH375 ��Ӧʱ�ӣ����ŵ�����
* Input       : None 
* Output      : None 
* Return      :  
*******************************************************************************/
void CH375_Configuration(void)
{
    /* ʹ�� CH375 ����ʱ��-------------------------------------*/
//    SystemInit();
	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CH375_Data | 
                           RCC_APB2Periph_GPIO_CH375_CTL , ENABLE);
    
    /* CH375 ������������--------------------------------------*/ 
    
    
    /* �ж������ INT# PB9 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    
    /* A0 , CS# , RD#, WR#  PB0-LED����*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11
                                 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
}

/*******************************************************************************
* Funtion     : CH375_WriteCmd
* Description : ��CH375д������ 
* Input       : ���� cmd 
* Output      : None 
* Return      :  
*******************************************************************************/
void CH375_WriteCmd(uint8_t cmd)
{
    uint16_t  TempData = 0 ;
    
    DATA_MODE_OUT;                // ���ģʽ
    
    //CS_L ;                      // ��CH375  
   
    TempData = GPIO_ReadOutputData(GPIO_CH375_Data);
    TempData &= 0xFF00;
    TempData |= (uint16_t)cmd ;    // 8λ������ǿ��ת��Ϊ16λ����8λ��Ϊ0�� 
    CH375_DelayNus(100);
    GPIO_Write(GPIO_CH375_Data, TempData);   // д������ 
    
    A0_Hx ;                      // ����ģʽ
    
    RD_Hx;                       // ��ѡͨ�ر�
    // CH375_DelayNus_Nus(50);
    WR_Lx;                       // дѡͨ�� 
    // CH375_DelayNus_Nus(100);
    
    WR_Hx;                       // дѡͨ�ر� 
    // CH375_DelayNus_Nus(50);
    //CS_H;                       // �ر�Ƭѡ  
    A0_Lx; 
    //CH375_DelayNus_Nus(100);   
}

/*******************************************************************************
* Funtion     : CH375_WriteData
* Description : ��CH375д������ 
* Input       : ���� data  
* Output      : None 
* Return      :  
*******************************************************************************/
void CH375_WriteDat(uint8_t dat)
{
    uint16_t  TempData;
    
    DATA_MODE_OUT;                // ���ģʽ
    
    //CS_L ;                        // ��CH375 
    
    TempData = GPIO_ReadOutputData(GPIO_CH375_Data);
    TempData &= 0xFF00;
    TempData |= (uint16_t)dat ;    // 8λ������ǿ��ת��Ϊ16λ����8λ��Ϊ0�� 
    CH375_DelayNus(100);
    GPIO_Write(GPIO_CH375_Data, TempData);   // д������ 
    
    A0_Lx ;                        // ����ģʽ 
    RD_Hx ;                          // ��ѡͨ�ر� 
    //CH375_DelayNus(50); 
    WR_Lx ;                          // дѡͨ��
    //CH375_DelayNus(150);
    
    WR_Hx;                         // дѡͨ�ر� 
    // CH375_DelayNus( 50 );
    //CS_H;                         // �ر�Ƭѡ  
    A0_Lx;                         // A0 �ָ�Ϊ1  ����������
    // CH375_DelayNus_Nus( 100 );	 
} 

/*******************************************************************************
* Funtion     : CH375_ReadCmd
* Description : ��CH375�������� 
* Input       : None 
* Output      :  
* Return      : ����������cmd 
*******************************************************************************/
uint8_t CH375_ReadCmd(void)
{
    uint8_t PortData; 
	 
    DATA_MODE_IN;             // ����ģʽ 
    //CS_L ;                    // ��Ƭѡ 
    A0_Hx ;                    // ����ģʽ
    
    WR_Hx ;                    // дѡͨ�ر� 
    //CH375_DelayNus( 50 );
    RD_Lx ;                    // ��ѡͨ�� 
    //CH375_DelayNus( 25 );
    PortData=(uint8_t)(GPIO_ReadInputData(GPIO_CH375_Data));
    //CH375_DelayNus( 25 );
    RD_Hx;
    //CH375_DelayNus(50);
    //CS_H;
    A0_Lx;           // ????????????????????????????????????
    //CH375_DelayNus(500);
    return PortData;

}
/*******************************************************************************
* Funtion     : CH375_ReadData
* Description : ��CH375�������� 
* Input       : None 
* Output      :  
* Return      : ����������data 
*******************************************************************************/
uint8_t  CH375_ReadDat(void)
{
    uint8_t PortData; 
	 
    DATA_MODE_IN;             // ����ģʽ 
    //CS_L ;                    // ��Ƭѡ 
    A0_Lx ;                    // ��ȡ���� 
    WR_Hx ;                    // дѡͨ�ر� 
    //CH375_DelayNus( 50 );
    RD_Lx ;                    // ��ѡͨ�� 
    //CH375_DelayNus( 25 );
    PortData=(uint8_t)(GPIO_ReadInputData(GPIO_CH375_Data));
    //CH375_DelayNus( 25 );
    RD_Hx;
    //CH375_DelayNus( 50 );
    //CS_H;
    A0_Hx;
    //CH375_DelayNus( 50 );
    return PortData;
}

/*******************************************************************************
* Funtion     : CH375_Wait_Interrupt
* Description : �ȴ��ж��źţ���INT�����ϲ����͵�ƽ������ȡ�ж�״̬�룬��ȡ��INT�ָ��ߵ�ƽ���Բ�����һ���ж��ź� 
* Input       : None 
* Output      :  
* Return      : �����ж�״̬ 
*******************************************************************************/
uint8_t CH375_WaitInterrupt(void)  
{    
    //CH375_DelayNus_Nus( 500 );
    while( GPIO_ReadInputDataBit(GPIO_CH375_CTL, GPIO_Pin_9) );  /* ��ѯ�ȴ�CH375��������ж�(INT#�͵�ƽ) */
    CH375_WriteCmd( CMD_GET_STATUS );  /* д�����������������ж�, ��ȡ�ж�״̬ */
    return( CH375_ReadDat( ) );
}

/*******************************************************************************
* Funtion     : CH375_Init
* Description : CH375оƬ��ʼ��������CH375�Ĺ���״̬��������ΪUSB������ʽ 
* Input       : None 
* Output      :  
* Return      : ��ʼ���ɹ�����0��ʧ�ܷ���1 
*******************************************************************************/
uint8_t  CH375_Init(void)
{    
    uint8_t  c, i ;
    
    CS_Lx;
    RD_Hx;
    WR_Hx;
        
    CH375_WriteCmd(CMD_CHECK_EXIST);  /* ���Թ���״̬ */
    CH375_WriteDat( 0x55 );             /* �������� */
    c = CH375_ReadDat( );               /* ��������Ӧ���ǲ�������ȡ�� */
    if ( c != 0xaa ) 
    {  
		/* CH375����,�������� */
        for ( i = 100; i != 0; i -- )
        {  
            /* ǿ������ͬ�� */
	    	CH375_WriteCmd( CMD_RESET_ALL );   /* CH375ִ��Ӳ����λ */
            CH375_DelayNus( 3200000 );         /* ��ʱ����30mS */
            CH375_WriteCmd( CMD_CHECK_EXIST ); /* ���Թ���״̬ */
            CH375_WriteDat( 0x55 );            /* �������� */
            c = CH375_ReadDat( );              /* ��������Ӧ���ǲ�������ȡ�� */
            if ( c == 0xaa )
                break;                          /*��������������ѭ�� */
      }
      
    }

    CH375_WriteCmd( CMD_SET_USB_MODE );  /* д�����������ΪUSB����ģʽ */
    CH375_WriteDat( 6 );                 /* ģʽ����,�Զ����USB�豸���� */
    for ( i = 0xff; i != 0; i -- )  /* �ȴ������ɹ�,ͨ����Ҫ�ȴ�10uS-20uS */
    {	
        CH375_DelayNus( 150 );
        if ( CH375_ReadDat( ) == CMD_RET_SUCCESS )   // �������״̬�����ݣ�
	    	break;  /* �����ɹ� ������ѭ��������ִ��*/
    }
    if ( i != 0 ) 
	    return( 0 );  /* �����ɹ�������0 */
    else 
	    return( 0xff );  /* CH375����,����оƬ�ͺŴ���ߴ��ڴ��ڷ�ʽ���߲�֧�� */
}

/*******************************************************************************
* Funtion     : CH375_Disk_Connect
* Description : ����豸�ǹ�����-
* Input       : None 
* Output      :  
* Return      : �������ӣ�����0 ;   �Ͽ������ضϿ�״̬
*******************************************************************************/
uint8_t CH375_DiskConnect(void)
{
    uint8_t INTStatus ;
    INTStatus = CH375_WaitInterrupt(); //�ȴ�USB�豸�������ӣ������ж�״̬
   
    return (INTStatus);    //�����ж�״̬              
}


/*******************************************************************************
* Funtion     : CH375_Disk_Init
* Description : ��ʼ�����̣� ��ʹ��U��֮ǰ����Ҫ���ô˺������˺����������ж�
                �����Ƿ����ӣ���ʼ���Ƿ�ɹ��Ȳ���

* Input       : None 
* Output      :  
* Return      : ��ʼ���ɹ�����0��ʧ�ܷ���1 
*******************************************************************************/
uint8_t CH375_DiskInit(void)
{
    uint8_t INTStatus, i ;
//	u32 a,b;
    
    CH375_Init();//�ȳ�ʼ��CH375оƬ
    CH375_DiskConnect();               // �������Ƿ�����
    
    
    CH375_WriteCmd(CMD_DISK_INIT);     // ��ʼ��USB�洢��
    INTStatus = CH375_WaitInterrupt(); // �ȴ��жϲ���ȡ�ж�״̬   
    if (INTStatus != USB_INT_SUCCESS) 
    {
        return (INTStatus);             // ���ִ���
    }
    
    CH375_WriteCmd(CMD_DISK_SIZE);     // ��ȡ�洢��������
    INTStatus = CH375_WaitInterrupt(); // ��ȡ�ж�״̬  
    if(INTStatus != USB_INT_SUCCESS)
    { 
        CH375_DelayNus(320000);              // ��������
        CH375_WriteCmd(CMD_DISK_SIZE);  
        INTStatus = CH375_WaitInterrupt(); // ��ȡ�ж�״̬  
    }
    if(INTStatus != USB_INT_SUCCESS)
    {
        return (INTStatus);            // ��������ĳ�ʼ�� ���ִ���
    }
    
    /* ������CMD_RD_USB_DATA����������ݶ���,����ÿ�����ֽ��� */
    CH375_WriteCmd( CMD_RD_USB_DATA );  /* ��CH375��������64�ֽڣ���ȡ���ݿ� ,����������ݿ鳤��*/
    i = CH375_ReadDat( );                 /* �������ݵĳ��� */ // 64�ֽڷּ��ζ��ꣿ��������������
    if ( i != 8 )                        //   ��������������������
    {
        return( USB_INT_DISK_ERR );  /* �쳣 */
    }
    for ( i = 0; i != 8; i ++ )               // ���ݳ��ȶ�ȡ���ݣ���������������������������
    {  
        /* ���ݳ��ȶ�ȡ���� */
	DATA_BUFFER[ i ] = CH375_ReadDat( );  /* �������ݲ����� */
    }
    i = DATA_BUFFER[ 6 ];    /* U�����������е�ÿ�����ֽ���,��˸�ʽ */
    if ( i == 0x04 ) 
    {
        BlockPerSector = 1024/CH375_BLOCK_SIZE;  /* ���̵�����������1K�ֽ� */
    }
    else if ( i == 0x08 ) 
    {
        BlockPerSector = 2048/CH375_BLOCK_SIZE;  /* ���̵�����������2K�ֽ� */
    }
    else if ( i == 0x10 ) 
    {
        BlockPerSector = 4096/CH375_BLOCK_SIZE;  /* ���̵�����������4K�ֽ� */
    }
    else 
    {
        BlockPerSector = 512/CH375_BLOCK_SIZE;  /* Ĭ�ϵĴ��̵�����������512�ֽ� */
    }
    BytePerSector = BlockPerSector*CH375_BLOCK_SIZE;  /* ������̵�������С */
//	a=(DATA_BUFFER[7]<<24)+(DATA_BUFFER[6]<<16)+(DATA_BUFFER[5]<<8)+(DATA_BUFFER[4]<<0);
//	b=(DATA_BUFFER[3]<<24)+(DATA_BUFFER[2]<<16)+(DATA_BUFFER[1]<<8)+(DATA_BUFFER[0]<<0);
//	printf("%d   \r\n",a*b);
    CH375_WriteCmd( CMD_SET_PKT_P_SEC );      /* ����USB�洢����ÿ�������ݰ����� */
    CH375_WriteDat( 0x39 );
    CH375_WriteDat( BlockPerSector );  /* ����ÿ�������ݰ����� */
    return( 0 );  /* U���Ѿ��ɹ���ʼ�� */      
}

/*******************************************************************************
* Funtion     : CH375_DiskReady
* Description : ���USB�豸�Ƿ�׼����(USB�豸�Ѿ�����ʼ�������Ѿ��õ������ַ)
* Input       : None 
* Output      :  
* Return      : �ж�״̬
*******************************************************************************/
uint8_t   CH375_DiskReady( void )
{
    CH375_WriteCmd( CMD_DISK_READY );  //д������
    return (CH375_WaitInterrupt());    //�����ж�״̬
}

/*******************************************************************************
* Funtion     : CH375_Write_Sector
* Description : ��U��һ����д������
* Input       : None 
* Output      :  
* Return      : д��ɹ�����0��ʧ�ܷ����ж�״̬
*******************************************************************************/

uint8_t CH375_WriteSector(uint32_t addr , const uint8_t *pbuff)
{
    uint8_t i,j;
    uint8_t status;
    CH375_WriteCmd(CMD_DISK_WRITE); // ����д����
    
    CH375_WriteDat(addr);          // ����32λ��������ַ�����͵�λ�����͸�λ
    CH375_WriteDat(addr >> 8);
    CH375_WriteDat(addr >> 16);
    CH375_WriteDat(addr >> 24);
    
    CH375_WriteDat(1);                 	//������������1������
    for(i = 0; i < 8; i++)  // 8��д�룬һ��д��64�ֽڣ������������Ĵ�С64�ֽڣ�
    {
        status = CH375_WaitInterrupt();
	if(status  == USB_INT_DISK_WRITE)      //usb�豸д����,�������������û��д�꣬�����
	{
	    CH375_WriteCmd(CMD_WR_USB_DATA7); //��USB�����˵�ķ��ͻ�����д�����ݿ�
	    CH375_WriteDat(64);	       //��д��������ݵĳ��ȣ������ֽڣ�
	    for(j = 0; j < 64; j++)	       //�����pbuf�е�����д��CH375��������64���ֽڣ�
	    {
		CH375_WriteDat(*pbuff);
		pbuff++;
	    }
	    CH375_WriteCmd(CMD_DISK_WR_GO);    // ����д����
	}
	else
	{
	    return status;
	}
	}
    
	status = CH375_WaitInterrupt();
	if(status == USB_INT_SUCCESS)	//���״̬��ΪUSB_INT_SUCCESS��˵��д�����ɹ�������0
	{
	 	return 0;	
	}
	else
	{
	 	return (status);
	}
}

/*******************************************************************************
* Funtion     : CH375_ReadSector
* Description : ��U��һ�����������ݣ��ŵ���������
* Input       : None 
* Output      :  
* Return      : ��ȡ�ɹ�����0����ȡʧ�ܷ����ж�״̬
*******************************************************************************/
uint8_t CH375_ReadSector(uint32_t addr, uint8_t *pbuff)
{
    uint8_t   mIntStatus;	
    uint8_t   mBlockCount;
    uint8_t   mLength;
    
    //��USB�洢�������ݿ�:Ҫ����������ַ�������� 
    CH375_WriteCmd( CMD_DISK_READ );  /* ��USB�洢�������ݿ�*/ 
    
    CH375_WriteDat( (u8)addr );            /* LBA�����8λ */
    CH375_WriteDat( (u8)( addr >> 8 ) );
    CH375_WriteDat( (u8)( addr >> 16 ) );
    CH375_WriteDat( (u8)( addr >> 24 ) );  /* LBA�����8λ */
    
    CH375_WriteDat( 1 );  /* ������ */
    
    //CH375�����ݻ�����Ϊ64�ֽڣ����Զ�ȡһ������Ҫ��8��
    for ( mBlockCount = 8; mBlockCount != 0; mBlockCount -- )  /* ���ݿ���� */
    { 
	mIntStatus = CH375_WaitInterrupt( );  /* �ȴ��жϲ���ȡ״̬ */
	if ( mIntStatus == USB_INT_DISK_READ )  /* USB�洢�������ݿ�,�������ݶ��� */
        {  
	    CH375_WriteCmd( CMD_RD_USB_DATA );  /* ��CH375��������ȡ���ݿ� */
	    mLength = CH375_ReadDat( );  /* �ȶ����������ݵĳ��� */ //�ȶ����������ݿ�ĳ��ȣ�1�����ݿ��м����ֽڣ�
	    while ( mLength )           /* ���ݳ��ȶ�ȡ���� */
            {  
		*pbuff = CH375_ReadDat( );  /* �������ݲ����� */
		pbuff ++;
		mLength --;
	    }
	    CH375_WriteCmd( CMD_DISK_RD_GO );  /* ����ִ��USB�洢���Ķ����� */
        }
	else 
            break;  /* ���ش���״̬ */
    }
    
   mIntStatus = CH375_WaitInterrupt( );  /* �ȴ��жϲ���ȡ״̬ */
   if ( mIntStatus == USB_INT_SUCCESS ) 
   {
       return( 0 );  /* �����ɹ� */
   }
   else
   {
      if(mIntStatus == USB_INT_DISK_ERR )
      {
          CH375_DelayNus(2000);
          CH375_WriteCmd(CMD_DISK_R_SENSE); /* ��ȡUSB�洢�������� */
          mIntStatus = CH375_WaitInterrupt();                 /* �ȴ��жϲ���ȡ״̬ */
          if(mIntStatus!=USB_INT_SUCCESS)            /* ���ִ��� */
          {
              return mIntStatus;
          }
      }
      return mIntStatus;

   }
  
}
