#include "SmartM_M0.h"
#include "watchdog.h"

/****************************************
*��������:WatchDogInit
*��    ��:��
*��    ��:��
*��    ��:���Ź���ʼ��
******************************************/
VOID WatchDogInit(VOID)
{
	PROTECT_REG
	(	
	  /* ʹ�ܿ��Ź�ʱ�� */	
		APBCLK |= WDT_CLKEN;		   
	
		/* ���ÿ��Ź�ʱ��ԴΪ10K */
		CLKSEL1 = (CLKSEL1 & (~WDT_CLK)) | WDT_10K;
					
		/* ʹ�ܿ��Ź���ʱ����λ���� */
		WTCR |= WTRE;	
	
		/* ���ÿ��Ź���ʱ���Ϊ1740.8ms */
		WTCR &= ~WTIS;
		WTCR |= TO_2T14_CK;	//(2^14+1024)*(1000000/10000)=17408*100=1740800us=1.7408s
	
		/* ʹ�ܿ��Ź��ж� */			
		WTCR |= WTIE;
		NVIC_ISER |= WDT_INT;
	
		/* ʹ�ܿ��Ź� */
		WTCR |= WTE;
	
		/* ��λ���Ź�����ֵ */
		WTCR |= CLRWTR;		
	)	
}
