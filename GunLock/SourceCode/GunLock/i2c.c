#include "SmartM_M0.h"
#include "i2c.h"

/*****************************************
*��������:Timed_Write_Cycle
*��    ��:��
*��    ��:��
*��    ��:ͬ����д����
******************************************/
void Timed_Write_Cycle(void)                   
{
	while (I2STATUS != 0x18)
  {
		//����
    I2CON |= STA;                          
    I2CON |= SI; 
    while ((I2CON & SI) != SI);
    I2CON &= ((~STA) & (~SI));

		//�豸��ַ
    I2DAT = EEPROM_SLA | EEPROM_WR;
    I2CON |= SI; 
    while ((I2CON & SI) != SI);
  }
		
	//ֹͣ
  I2CON |= STO;
  I2CON |= SI;
  while (I2CON & STO);
}



/*****************************************
*��������:I2CInit
*��    ��:��
*��    ��:��
*��    ��:I2C��ʼ��
******************************************/
VOID I2CInit(VOID)
{
	P3_PMD &= ~(Px4_PMD | Px5_PMD);
	P3_PMD |= (Px4_OD | Px5_OD);               	//ʹ��I2C0����

  P3_MFP &= ~(P34_T0_I2CSDA | P35_T1_I2CSCL);
	P3_MFP |= (I2CSDA | I2CSCL);     			      //ѡ��P3.4,P3.5��ΪI2C0��������   
    
	APBCLK |= I2C0_CLKEN;                       //ʹ��I2C0ʱ��
  /*baud rate 214KHz   12000000/[4*(13+1)]*/
  I2CLK = I2C_CLOCK;
  /*����������Ϊ 125KHz/s  50/[4*(99+1)]*/
		
  I2CON |= ENSI;                             	//ʹ��I2C
}


/*****************************************
*��������:I2CWrite
*��    ��:unAddr   д��ַ
          pucData  д����
		      unLength д����
*��    ��:TRUE/FALSE
*��    ��:AT24C0Xд����
******************************************/
BOOL I2CWrite(UINT32 unAddr,UINT8 *pucData,UINT32 unLength)
{
	UINT32 i;

  I2CON |= STA;          				//����
  I2CON |= SI;                       	

  while ((I2CON & SI) != SI);	       

  I2CON &= ((~STA)&(~SI));           	

  if (I2STATUS != 0x08)	            
  {
		return FALSE;
  }

  //�����д���Ʋ��� 
  I2DAT = EEPROM_SLA | EEPROM_WR;
  I2CON |= SI;
  while ((I2CON & SI) != SI);

  if (I2STATUS != 0x18)              
  {
		return FALSE;
  }

	//д��ַ
  I2DAT = unAddr;
  I2CON |= SI; 
  while ((I2CON & SI) != SI);
  if (I2STATUS != 0x28)              
  {
	  return FALSE;
  }
	
	//д����
  for(i=0; i<unLength; i++)
	{
		I2DAT = *(pucData+i);
		I2CON |= SI; 
		while ((I2CON & SI) != SI);
		if (I2STATUS != 0x28)              
		{
			return FALSE;
		}	
	}

  //ֹͣ
  I2CON |= STO;
  I2CON |= SI;
  while (I2CON & STO);               

  Timed_Write_Cycle();               
                                       							
	return TRUE;												 
}


/*****************************************
*��������:I2CRead
*��    ��:unAddr   ����ַ
          pucData  ������
		      unLength ������
*��    ��:TRUE/FALSE
*��    ��:AT24C0X������
******************************************/
BOOL I2CRead(UINT32 unAddr,UINT8 *pucData,UINT32 unLength)
{
	UINT32 i;

  I2CON |= STA;          			    //����
  I2CON |= SI;                       	

  while ((I2CON & SI) != SI);	        

  I2CON &= ((~STA)&(~SI));           	

  if (I2STATUS != 0x08)	            
  {
		return FALSE;
  }
		
  //�����д���Ʋ���  
  I2DAT = EEPROM_SLA | EEPROM_WR;
  I2CON |= SI;
  while ((I2CON & SI) != SI);

  if (I2STATUS != 0x18)              
  {
		return FALSE;
  }

	//д�����ַ
  I2DAT = unAddr;
  I2CON |= SI; 
  while ((I2CON & SI) != SI);
  if (I2STATUS != 0x28)              
  {
		return FALSE;
  }
	
  // ��������
  I2CON |= STA;          
  I2CON |= SI;                               	
  while ((I2CON & SI) != SI);                	
  I2CON &= ((~STA)&(~SI));                   	

  if (I2STATUS != 0x10)                      	
  {
		return FALSE;
  }

  //���������  
  I2DAT = EEPROM_SLA | EEPROM_RD;
  I2CON |= SI;
  while ((I2CON & SI) != SI);	    

  if (I2STATUS != 0x40)              
  {
		while (1);
  }

  //��ȡ����
	I2CON |= AA;

  for(i=0; i<unLength; i++)
	{
		I2CON |= SI; 
		while ((I2CON & SI) != SI);
		
	  if (I2STATUS != 0x50)              
	  {
			return FALSE;
		}
		
		*(pucData+i) = I2DAT;	
	} 

  //����NACK��AT24C02��ִ�жϿ����Ӳ���
  I2CON &= (~AA);
  I2CON |= SI; 
  while ((I2CON & SI) != SI);
        
  //ֹͣ
  I2CON |= STO;
  I2CON |= SI;
  while (I2CON & STO);                      

	return TRUE;
}
