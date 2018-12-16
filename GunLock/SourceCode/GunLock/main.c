#include "SmartM_M0.h"
#include <stdio.h>
#include <stdlib.h>

#include "74ls164.h"
#include "HX711.h"
#include "lock.h"
#include "i2c.h"
#include "rs485.h"
#include "led.h"
#include "watchdog.h"
#include "timer.h"

/***add***/
#define My_485
void F_ReadUart(void);
#define   nAdjSize   34

unsigned long  weight_[nAdjSize];    
uint8_t   nWeight_Inx;
unsigned long  nWeight_Get;
unsigned long  nWeight_Result;
unsigned long  nW_Max,nW_Min; 

volatile uint32_t  nDelayLock=0;
float nBulletWeight=0;
//volatile uint8_t    bStartRev=0;

volatile uint8_t  nUartDelay=0;

uint8_t     bFlag=0;
//uint8_t     nNNNN=0;

/***end***/

UINT8 dev_id[32] = {0};	//for save ID
UINT8 slave_id[8] = {0};   // slave id
INT8 gun_status_flag = -1;
INT8 lock_status_flag = -1;
UINT8 g_slave_addr = 0;
UINT8 lock_cmd = 1;
int ele_flag = 0;

float	HX711_Buffer	= 0;
float	Weight_Maopi	=	0;
float Weight_Shiwu	=	0;

#define    B2_CURRENT          1

UINT8         g_ucSegCurPosition_B2=0;

UINT8  CONST  g_ucSegCodeTbl[10]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};//������������0~9��ֵ������Ӳ����ǰ������3���ܷ������Ӳ��ʵ���ù���������ܣ�����ù������ַ�?

//UINT8  CONST  g_ucSegPositionTbl_B2[4]={0x07,0x0B,0x0D,0x0E};//P0.0~P0.3��������ѡ��,B2�����ɼ�
UINT8 CONST  g_ucSegPositionTbl_B2[4]={0X0E,0x0D,0x0B,0x07};//P0.3~P0.1��������ѡ��,B2�����ɼ�
UINT8 g_ucSegBuf[4]  ={0};

UINT8 weight_buf[4] = {0};

//static UINT8 all_status_flags = 0;


void write_dev_id(void)
{
	int i;
		
	for (i = 4; i < g_StRS485Packet.Cnt -1; i++)
	{
		I2CWrite(32 + i - 4, &g_StRS485Packet.Buf[i],1);
		Delayms(1);
	}	
}


UINT8 BCC(UINT8 ecc, RS485_PACKET RS485Packet)
{
	UINT8 i ;
	ecc = 0;
						
	for (i = 0; i < RS485Packet.Cnt -1; i++)
	{
		ecc ^= RS485Packet.Buf[i];
	}
		
	return ecc;
}


UINT8 BCC_send(UINT8 ecc, RS485_PACKET RS485Packet)
{
	UINT8 i ;
	ecc = 0;
						
	for (i = 0; i < RS485Packet.Cnt; i++)
	{
		ecc ^= RS485Packet.Buf[i];
	}
		
	return ecc;
}


/*����*/
/*
float Get_Weight(void)
{
	HX711_Buffer	=	HX711_Read();
	HX711_Buffer	=	HX711_Buffer/100;
	if (HX711_Buffer > Weight_Maopi)
	{
		Weight_Shiwu	=	HX711_Buffer;
		Weight_Shiwu	=	Weight_Shiwu	-	Weight_Maopi;
		Weight_Shiwu = (Weight_Shiwu/4.439+0.02);		
		//����ʵ���ʵ������
		//��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������4.30���������
	}
	
	return Weight_Shiwu; 
}
*/


float Get_Weight(void)
{
	unsigned long tmpa;
	HX711_Buffer	=	HX711_Read();
	HX711_Buffer	=	HX711_Buffer/100;
	if (HX711_Buffer > Weight_Maopi)
	{
		tmpa	=	HX711_Buffer;
		tmpa	=	tmpa	-	Weight_Maopi;
		//Weight_Shiwu = (Weight_Shiwu/4.439+0.02);		
		//����ʵ���ʵ������
		//��Ϊ��ͬ�Ĵ������������߲�һ������ˣ�ÿһ����������Ҫ���������4.30���������
	}
	else
	{
		tmpa = 0;
	}
	
	return tmpa; 
}


/*��ȡëƤ����*/
float Get_Maopi(void)
{
	int x;
	unsigned long  tmp = 0;
	unsigned long  nMin,nMax,dat;
	nMin = nMax = 0;
	for(x=0;x<10;x++)
	{
		dat = HX711_Read();
		if(x==0)
		{
			nMin = nMax=dat;
		}
		else
		{
			if(nMin>dat)
			{
				nMin=dat;
			}
			if(nMax<dat)
			{
				nMax=dat;
			}
		}

		tmp+=dat;
	}

	tmp-=nMax;
	tmp-=nMin;
	
	HX711_Buffer = tmp/8; //HX711_Read();
	Weight_Maopi = HX711_Buffer/100;
	return Weight_Maopi;
} 


/****************************************
*��������:RefreshDisplayBuf
*��    ��:��
*��    ��:��
*��    ��:ˢ����ʾ������
******************************************/
VOID RefreshDisplayBuf(int type,int channel)	 
{
//	unsigned long tempw;
//	uint32_t  nNum = 0;

	/*
	int i = 0, j = 0;
	unsigned long t = 0;
	unsigned long tempw;
	unsigned long temp[20] = {0};

	//Get_Weight();
	for (i = 0; i < 20; i++)
	{
	  Get_Weight();
	  temp[i] = (unsigned long)Weight_Shiwu;
	}

	for (j = 0; j < 20; j++)
	{
	  for(i = 0; i < 20 - j; i++)
	  {
	   if (temp[i] > temp[i+1])
	   {
	     t = temp[i];
	     temp[i] = temp[i+1];
	     temp[i+1] = t;
	   }
	  }
	}
    //tempw = temp[1]+temp[2]+temp[3]+temp[4]+temp[5]+temp[6]+temp[7]+temp[8];
    tempw = (temp[10]+temp[11])/2.0;
		*/
#if 0		
	tempw = (unsigned long) Weight_Shiwu;
	g_ucSegBuf[0] = tempw % 10;//��λ
    g_ucSegBuf[1] = tempw / 10 % 10;//ʮλ
    g_ucSegBuf[2] = tempw / 100 % 10;//��λ
	g_ucSegBuf[3] = tempw / 1000 % 10;//ǧλ

#endif

#if 1 
	uint32_t  nNum = 0;
	if(nBulletWeight!=0)
	nNum =  Weight_Shiwu  / nBulletWeight;
	if (nNum + 0.5 > (Weight_Shiwu / nBulletWeight))
	{
		nNum = nNum+1;
	}
	else
	{
		nNum = nNum;
	}
  g_ucSegBuf[0] = nNum % 10;
	g_ucSegBuf[1] = nNum/ 10 % 10;
	g_ucSegBuf[2] = nNum / 100 % 10;
	g_ucSegBuf[3] = nNum / 1000 % 10;
	
#endif

}


/****************************************
*��������:SegDisplay
*��    ��:��
*��    ��:��
*��    ��:�������ʾ
******************************************/
VOID SegDisplay_B2_B5_VOLTAGE(VOID)
{
	UINT8  t;
  	
 	//RefreshDisplayBuf(B2_CURRENT,0);
  	t = g_ucSegCodeTbl[g_ucSegBuf[g_ucSegCurPosition_B2]]; 
  	SEG_PORT_P0 |= 0x0F;//Ϩ��P0.0��P0.1��P0.2��P0.3�����
     
  	LS164Send_B2(t);
	SEG_PORT_P0 &= g_ucSegPositionTbl_B2[g_ucSegCurPosition_B2];	//�����λ λ�ñ�

  	if (++g_ucSegCurPosition_B2>=4) 
  	{
		g_ucSegCurPosition_B2 =0;
  	}
}


void F_GetWeight(unsigned long dat)
{
	uint8_t i;
	uint64_t  tmp=0;
	for(i=0;i<nAdjSize-1;i++)
	{
		weight_[i]=weight_[i+1];
	}

	weight_[nAdjSize-1]=dat;
	nW_Min = nW_Max = 0;
	for(i=0;i<nAdjSize;i++)
	{
		if(nW_Max<weight_[i])
		{
			nW_Max=weight_[i];
		}
		if(nW_Min>weight_[i])
		{
			nW_Min=weight_[i];
		}

		tmp+=weight_[i];
	}

	tmp-=nW_Max;
	tmp-=nW_Min;
	nWeight_Result = (unsigned long)(tmp/(nAdjSize-2)); 
	Weight_Shiwu = (nWeight_Result/4.439f + 0.02);
		
	return; 
}


/****************************************
*��������:main
*��    ��:��
*��    ��:��
*��    ��:��������
******************************************/
INT32 main(VOID)
{
	int gun_flag,lock_flag;  //status flags
	
	int	thousand,	hundred, ten,	bit;
//	int n_thousand, n_hundred, n_ten, n_bit;
	UINT8	ecc = 0;			// BCC check 
	UINT32	i = 1;
	uint64_t  nCount_=0;
		
	UINT8  write_len = 0;
	nUartDelay = 0;	
  	PROTECT_REG
	(												//ISP����ʱ����FLASH�洢��
		PWRCON |= XTL12M_EN;						//Ĭ��ʱ��ԴΪ�ⲿ����
		while((CLKSTATUS & XTL12M_STB) == 0); 		//�ȴ�12MHzʱ���ȶ�	
		
		CLKSEL0 = (CLKSEL0 & (~HCLK)) | HCLK_12M;	//�����ⲿ����Ϊϵͳʱ��	 
	)
	
	LedInit();
	LockInit();	
	POWER_DATA(1);		// lock output low
	WatchDogInit();
	I2CInit();	
	UartInit(12000000,9600);	//����������Ϊ9600bps
	InitRS485_DR();
	
	Hx711Init();
	LS164Init();							   //74LS164��ʼ��
	TMR1Init();
	
	/*��ʱ��2��ʼ��2000ms����һ���ж�*/
	TMR2Init();
	//TMR3Init();
	
	I2CRead(24, slave_id, 1);
	Get_Maopi();
	
	EN_RS485_R();
	nCount_ = 0;
	nW_Max = nW_Min = 0;
	for(i=0;i<nAdjSize;i++)
  	{
		weight_[i]= Get_Weight();
		nCount_+=weight_[i];
		if(i==0)
		{
			nW_Max = nW_Min= weight_[i];
		}
		else
		{
			if(nW_Min>weight_[i]);
			{
				nW_Min=weight_[i];
			}
			if(nW_Max<weight_[i]);
			{
				nW_Max=weight_[i];
			}
		}
    }
		
	nCount_-=nW_Max;
	nCount_-=nW_Min;
	nWeight_Result = (unsigned long)(nCount_/(nAdjSize-2));
	bFlag = 1;
	while (1)
	{
		F_GetWeight(Get_Weight());
		RefreshDisplayBuf(B2_CURRENT,0);
	
		/*ι��*/
		PROTECT_REG
		(
			WTCR |= CLRWTR;
		)
#if 1			
 	//	Delayus(1000);//Ϊʲô�����ʱ����ɾ��
		lock_status();
			
		/*ǹ��λ���𣬲���λǹ��*/
		if(0 == (P3_PIN & (1 << 2)))
		{
			lock_led_off();
		}
		else
		{
			lock_led_on();
		}
#endif
		//if ((0 == lock_cmd )&&((65 != lock_status()) || (98 != gun_status())))
		//{
		//	all_status_flags = 1;
		//}
		//else
		//{
		//	all_status_flags = 0;
		//}
			
		/*******BCC*********/
#if 1		
		
		ecc = BCC(ecc, g_StRS485Packet);
//		data_len = g_StRS485Packet.Cnt - 4;//���ݳ���
		if(
		  ((0x88 == g_StRS485Packet.Buf[1])//Ŀ�ĵ�ַΪ88��ʾ�����û�
		  ||(slave_id[0] == (g_StRS485Packet.Buf[0])
		  && 0x55 == g_StRS485Packet.Buf[1]))//55��ʾ��ͨ�û�
//		  && data_len == g_StRS485Packet.Buf[2]//���ݳ���
		  && ecc == g_StRS485Packet.Buf[g_StRS485Packet.Cnt-1]//У����	
		  )
		{
			switch (g_StRS485Packet.Buf[3])//g_StRS485Packet  ��λ������
			{																
				/*��ѯ��Ϣ*/
				case 0x21:
					rs485_send_packet_clean();
			
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];	
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 2;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 64;	// output @ 
									 
					ecc = BCC_send(ecc, RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);
			
					break;
							
				/*ǹ������״̬*/
				case 0x22:
					gun_status_flag = -1;
					lock_status_flag = -1;
			
					gun_flag = gun_status();
					lock_flag = lock_status();
		
					rs485_send_packet_clean();
				
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 3;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = lock_flag;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = gun_flag; //Ϊʲôgun_flag������ȡ��ͨѶ����
					ecc = BCC_send(ecc, RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);
					
					break;
					
				/*����*/
				case 0x23:
					lock_open();
    				lock_cmd = 1;
					gun_status_flag = -1;
					lock_status_flag = -1;
					gun_flag = gun_status();
					lock_flag = lock_status();
									
					rs485_send_packet_clean();
						
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 3;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = lock_flag;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = gun_flag;
									
					ecc = BCC_send(ecc, RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);
							
					break;
					
				/*�޸�����ַ*/
				case 0x24:		
					memset(slave_id, 0, 8);
					slave_id[0] = g_StRS485Packet.Buf[4];
 					I2CWrite(24, slave_id, 1);
			
					rs485_send_packet_clean();
				
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 3;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 79; // 'O'
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 75; // 'K'
					ecc = BCC_send(ecc, RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);
				  
					break;
				
				/*��ʾ����*/
				case 0x2D:  //0x40:
				
			    thousand = (unsigned long)Weight_Shiwu / 1000 % 10;
					hundred	= (unsigned long)Weight_Shiwu / 100 % 10;
					ten	= (unsigned long)Weight_Shiwu/ 10 % 10;
					bit	= (unsigned long)Weight_Shiwu % 10;
			  
					rs485_send_packet_clean();
					
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];	// src_addr
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 5;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = thousand;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = hundred;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ten;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = bit;

					//RS485Packet_send.Buf[RS485Packet_send.Cnt++] = (UINT8)((nNum<<24)&0xFF);
					//RS485Packet_send.Buf[RS485Packet_send.Cnt++] = (UINT8)((nNum<<16)&0xFF);
					//RS485Packet_send.Buf[RS485Packet_send.Cnt++] = (UINT8)((nNum<<8)&0xFF);
					//RS485Packet_send.Buf[RS485Packet_send.Cnt++] = (UINT8)(nNum&0xFF);

					ecc = BCC_send(ecc, RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf, RS485Packet_send.Cnt);				
					break;
				/*�޸�ΨһID*/
				case 0x78:
   				    write_len = 0;
					memset(dev_id, 0, 32);
					write_len = g_StRS485Packet.Cnt - 5;
					I2CWrite(64,&write_len,1);
					write_dev_id();
									
					rs485_send_packet_clean();
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 3;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 79; // 'O'
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 75; // 'K'
									
					ecc = BCC_send(ecc, RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);
					
					break;

				/*��ΨһID*/
				case	0x79:
					write_len = 0;
					I2CRead(64,&write_len,1);
					memset(dev_id, 0, 32);
					I2CRead(32,dev_id,write_len);
						
					rs485_send_packet_clean();
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = write_len + 1;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
					for (i = 0; i < write_len; i++,RS485Packet_send.Cnt++)
					{
						RS485Packet_send.Buf[RS485Packet_send.Cnt] = dev_id[i];
					}
																		
					ecc = BCC_send(ecc,RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);
							
					break;
					
				/*��λ*/
				case 0x44:
					rs485_send_packet_clean();
				
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 3;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 79; // 'O'
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 75; // 'K'
									
					ecc = BCC_send(ecc, RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);
									
					PROTECT_REG
					(
						IPRSTC1|= 0x01; //ִ�и�λ
					)
					
					break;
				case 0x2B:      // �趨�����ӵ�����
					
					nBulletWeight = Weight_Shiwu/20.0;
				  	rs485_send_packet_clean();
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];	// src_addr
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 1;	//8
				    RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
					//RS485Packet_send.Buf[RS485Packet_send.Cnt++] = nBulletWeight;
					 
					ecc = BCC_send(ecc,RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);						
					
					break;
				
				case 0x2E:
				{      //��ȡ����
					uint32_t  nNum = 0;
					if(nBulletWeight!=0)
					{
						nNum =  Weight_Shiwu  / nBulletWeight;
					}
					if (nNum + 0.5 > (Weight_Shiwu / nBulletWeight))
					{
						nNum = nNum;
					}
					else
					{
						nNum = nNum + 1;
					}
                    //  n_thousand = nNum / 1000 % 10;
				    //	n_hundred	= nNum / 100 % 10;
					//  n_ten	= nNum/ 10 % 10;
					//  n_bit	= nNum % 10;

					rs485_send_packet_clean();
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];	// src_addr
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 5;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
						
					//  RS485Packet_send.Buf[RS485Packet_send.Cnt++] = n_thousand;
					//  RS485Packet_send.Buf[RS485Packet_send.Cnt++] = n_hundred;
					//  RS485Packet_send.Buf[RS485Packet_send.Cnt++] = n_ten;
					//  RS485Packet_send.Buf[RS485Packet_send.Cnt++] = n_bit;
					
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = (UINT8)((nNum>>24)&0xFF);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = (UINT8)((nNum>>16)&0xFF);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = (UINT8)((nNum>>8)&0xFF);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = (UINT8)(nNum&0xFF);
					  
					ecc = BCC_send(ecc,RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);						
					
				    break;
				}
					
				 case 0x27: ///����Ƥ��
					Get_Maopi();
				    rs485_send_packet_clean();
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];	// src_addr
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 1;
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
						
					ecc = BCC_send(ecc,RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);						
						
					break;

				default:
					rs485_send_packet_clean();
	
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[1];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 6;	//count
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = g_StRS485Packet.Buf[3];
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 68; //ʮ�����Ƶ�44
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 68; //ʮ�����Ƶ�44
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 68; //ʮ�����Ƶ�44
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 68; //ʮ�����Ƶ�44
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 68; //ʮ�����Ƶ�44
					ecc = BCC_send(ecc, RS485Packet_send);
					RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
					UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);				
					
					break;
			}

			clean_rev_buf();
			rs485_send_packet_clean();
		}
		else //(0x51 != g_StRS485Packet.Buf[0])
		{
			clean_rev_buf();
			rs485_send_packet_clean();	
		}
		
		clean_rev_buf();
		rs485_send_packet_clean();
		#endif	 
	 }

}

 
/****************************************
*��������:UART0_IRQHandler
*��    ��:��
*��    ��:��
*��    ��:����0�жϷ�����
******************************************/
VOID UART0_IRQHandler(VOID)
{	
#ifdef My_485
	uint8_t nIx=0;
	if(UA0_ISR & RDA_INT)				 //����Ƿ���������ж�
	{
		if(nUartDelay>=60)    //60  * 2   ����120ms
		{
			SET_RS485_R_FLAG(FALSE);
			memset(RS485Packet_send.Buf, 0, sizeof(RS485Packet_send.Buf));
			RS485Packet_send.Cnt = 0;
			nUartDelay = 0;
		}
		if(nUartDelay>12)
		{					
			while(UA0_ISR & RDA_IF) 		 //��ȡ���н��յ�������
			{
				nIx = g_StRS485Packet.Cnt;
				nIx++;
				if(nIx<sizeof(g_StRS485Packet.Buf))
				{
					g_StRS485Packet.Buf[g_StRS485Packet.Cnt++]=UA0_RBR;
				}
				else
				{
					g_StRS485Packet.Cnt = 0;
				}
						  
			}

			SET_RS485_R_FLAG(TRUE);
		}
	}
	//nUartDelay = 0;
#else	
	memset(RS485Packet_send.Buf, 0, sizeof(RS485Packet_send.Buf));
	RS485Packet_send.Cnt = 0;
	SET_RS485_R_FLAG(FALSE);
	
	if(UA0_ISR & RDA_INT)				 //����Ƿ���������ж�
	{			
		while(UA0_ISR & RDA_IF) 		 //��ȡ���н��յ�������
		{
			g_StRS485Packet.Buf[g_StRS485Packet.Cnt++]=UA0_RBR;
			if(g_StRS485Packet.Cnt>=sizeof(g_StRS485Packet.Buf))
			{
				g_StRS485Packet.Cnt = 0;
			}
		}
	}	

	SET_RS485_R_FLAG(TRUE);
	Delayms(10);//Ϊʲô���ʱ�䲻��ɾ����С��1ms
#endif	
}


/****************************************
*��������:TMR1_IRQHandler
*��    ��:��
*��    ��:��
*��    ��:��ʱ��1�жϷ�����
******************************************/
VOID TMR1_IRQHandler(VOID)
{
  //if(ele_flag)
  //{
	SegDisplay_B2_B5_VOLTAGE();
#ifdef My_485	
	nUartDelay++;
	if(nUartDelay==0)
	{
		nUartDelay = 0xFF;
	}
	else
	{
		if(nUartDelay>20)
		{
			SET_RS485_R_FLAG(TRUE);
		}
	}
#endif			
  //}
	if(nDelayLock>0)
	{
		nDelayLock--;
		if(nDelayLock==0)
		{
			lock_close();
		}
	}
			
	/* ���TMR1�жϱ�־λ */
	TISR1 |= TMR_TIF;
}


/****************************************
*��������:TMR2_IRQHandler
*��    ��:��
*��    ��:��
*��    ��:��ʱ��2�жϷ�����
******************************************/
VOID TMR2_IRQHandler(VOID)
{
	STATIC UINT32 t2_cnt=0;
//	UINT8 ecc = 0;
	
	t2_cnt++;
	RUN_LED(t2_cnt%2);

#if 0	
	if (all_status_flags)
	{
		gun_status_flag = gun_status();
		lock_status_flag = lock_status();
														
		rs485_send_packet_clean();
	
		RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 136;
		RS485Packet_send.Buf[RS485Packet_send.Cnt++] = slave_id[0];
		RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 3;
		RS485Packet_send.Buf[RS485Packet_send.Cnt++] = 250;	// send  abnormal flags
		RS485Packet_send.Buf[RS485Packet_send.Cnt++] = gun_status_flag;
		RS485Packet_send.Buf[RS485Packet_send.Cnt++] = lock_status_flag;
		
		ecc = BCC_send(ecc, RS485Packet_send);
		RS485Packet_send.Buf[RS485Packet_send.Cnt++] = ecc;
		UartSend(RS485Packet_send.Buf,RS485Packet_send.Cnt);
	}
#endif
	
	/* ���TMR2�жϱ�־λ */
	TISR2 |= TMR_TIF;
}


/****************************************
*��������:TMR3_IRQHandler
*��    ��:��
*��    ��:��
*��    ��:��ʱ��3�жϷ�����
******************************************/
// VOID TMR3_IRQHandler(VOID)
// {
// 	memset(weight_buf,0,sizeof(weight_buf));
//   I2CWrite(16,weight_buf,4);				
// 	I2CRead(16,weight_buf,4);
// 	
// 	/* ���TMR3�жϱ�־λ */
// 	TISR3 |= TMR_TIF;
// }
