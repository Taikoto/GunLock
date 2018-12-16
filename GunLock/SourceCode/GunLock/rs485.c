#include "SmartM_M0.h"
#include "rs485.h"


BOOL g_bRS485RxEnd = TRUE;
RS485_PACKET g_StRS485Packet = {0};
RS485_PACKET RS485Packet_send = {0};



/****************************************
*��������:UartInit
*��    ��:unFosc 	 ����Ƶ��
          unBaud	 ������
*��    ��:��
*��    ��:���ڳ�ʼ��
******************************************/
VOID UartInit(UINT32 unFosc,UINT32 unBaud)
{
	P3_MFP &= ~(P31_TXD0 | P30_RXD0);   
  P3_MFP |= (TXD0 | RXD0);    		//P3.0 ʹ��Ϊ����0����  P3.1 ʹ��Ϊ����0����
										
  UART0_Clock_EN;         //����0ʱ��ʹ��
  UARTClkSource_ex12MHZ;  //����ʱ��ѡ��Ϊ�ⲿ����
  CLKDIV &= ~(15<<8); 	//����ʱ�ӷ�ƵΪ0

	IPRSTC2 |= UART0_RST;   //��λ����0
  IPRSTC2 &= ~UART0_RST;  //��λ����
  UA0_FCR |= TX_RST;      //����FIFO��λ
  UA0_FCR |= RX_RST;      //����FIFO��λ
	
	UA0_LCR &= ~PBE;     	//У��λ����ȡ��
	UA0_LCR &= ~WLS;
  UA0_LCR |= WL_8BIT;     //8λ����λ
  UA0_LCR &= NSB_ONE;     //1λֹͣλ

	UA0_BAUD |= DIV_X_EN|DIV_X_ONE;   //���ò����ʷ�Ƶ
    
  UA0_BAUD |= ((unFosc / unBaud) -2);	//����������  UART_CLK/(A+2) = 115200, UART_CLK=12MHz 

	UA0_IER	|= RDA_IEN;					//���������ж�ʹ��
	NVIC_ISER |= UART0_INT;	 			//ʹ�ܴ���0�ж�
}


VOID InitRS485_DR(void)
{
	/*��ת�ų�ʼ��*/
	P2_PMD &= ~(3UL<<(UART_485_PIN<<1));//IOģʽ����,P2.1
	P2_PMD |= 1UL<<(UART_485_PIN<<1);//IOģʽ��P2.1����Ϊ���ģʽ
	
  //UA0_FUN_SEL = RS485_EN;        //����Ϊ485����  
  //UA0_FCR |= (RX_DIS);           //��ֹ�������� 
	//UA0_RS485_CSR |= RS485_NMM;    //����Ϊ485��ͨ����ģʽ  
  //UA0_RS485_CSR |= RS485_AUD;    //����Ϊ�����Զ�����ģ
	///* ����UART0�ж� */  
  //UA0_IER |= RDA_IEN;           //�����ɽ��������жϺ�  
  //UA0_IER |= RLS_IEN;           //���������ж�״̬ʹ��,��һ�������жϣ���M0�ֲ�
}


/****************************************
*��������:UartSend
*��    ��:pBuf 			 �������ݻ�����
          unNumOfBytes	 �����ֽ�����
*��    ��:��
*��    ��:���ڷ�������
******************************************/
VOID UartSend(UINT8 *pBuf,UINT32 unNumOfBytes)
{
	UINT32 i;
	/*wait the rs485 rev data over*/
	WAIT_RS485_R_END();
	/*enable rs485 send data */
	EN_RS485_S();
		
	for(i=0; i<unNumOfBytes; i++)
	{	
		/*move data to send rigister*/
		UA0_THR = *(pBuf+i);
					
    while ((UA0_FSR&TX_EMPTY) == 0x00); //��鷢��FIFO�Ƿ�Ϊ��	 
	}

	/*wait rs485 send data over	*/
	WAIT_RS485_S_END();
		
	/*enable rs485 rev data */
	EN_RS485_R();
	Delayms(5);
}


void clean_rev_buf(void)
{
	/*clean the rev buf*/
	memset(g_StRS485Packet.Buf, 0, sizeof(g_StRS485Packet.Buf));
  /*set count zero  to rev next data count*/
	g_StRS485Packet.Cnt = 0;
}


void rs485_send_packet_clean(void)
{
	/*clean the rev buf*/
	memset(RS485Packet_send.Buf, 0, sizeof(RS485Packet_send.Buf));
	/*set count zero  to rev next data count*/
	RS485Packet_send.Cnt = 0;
}


void rs485_packet_addr(void)
{
	rs485_send_packet_clean();
		
	/*Ŀ���ַ*/
	RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 85;	
}
