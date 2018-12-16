#include "SmartM_M0.h"
#include "timer.h"

/****************************************
*��������:TMR1Init
*��    ��:��
*��    ��:��
*��    ��:��ʱ��1��ʼ��
******************************************/
VOID TMR1Init(VOID)
{
	PROTECT_REG
	(
	/* ʹ��TMR1ʱ��Դ */
    APBCLK |= TMR1_CLKEN;
	/* ѡ��TMR1ʱ��ԴΪ�ⲿ����12MHz */  
	CLKSEL1 = (CLKSEL1 & (~TM1_CLK)) | TM1_12M; 	

	/* ��λTMR1 */
	IPRSTC2 |= TMR1_RST;
	IPRSTC2 &= ~TMR1_RST;

	/* ѡ��TMR1�Ĺ���ģʽΪ����ģʽ*/	
	TCSR1 &= ~TMR_MODE;
	TCSR1 |= MODE_PERIOD;		
	
	/* ������� = (Period of timer clock input) * (8-bit Prescale + 1) * (24-bit TCMP)*/
	TCSR1 = TCSR1 & 0xFFFFFF00;		// ����Ԥ��Ƶֵ [0~255]
	TCMPR1 = 12000*2;			        // ���ñȽ�ֵ [0~16777215]

	/* ʹ��TMR1�ж� */
	TCSR1 |= TMR_IE;		
	NVIC_ISER |= TMR1_INT;	

	/* ��λTMR1������ */
	TCSR1 |= CRST;	

	/* ʹ��TMR1 */					
	TCSR1 |= CEN;	
	)
}


/****************************************
*��������:TMR2Init
*��    ��:��
*��    ��:��
*��    ��:��ʱ��2��ʼ��
******************************************/
VOID TMR2Init(VOID)
{
  PROTECT_REG
	(
		/* ʹ��TMR2ʱ��Դ */
	  APBCLK |= TMR2_CLKEN;
		/* ѡ��TMR2ʱ��ԴΪ�ⲿ����12MHz */  
		CLKSEL1 = (CLKSEL1 & (~TM2_CLK)) | TM2_12M; 	
	
		/* ��λTMR2 */
		IPRSTC2 |=  TMR2_RST;
		IPRSTC2 &= ~TMR2_RST;
	
		/* ѡ��TMR2�Ĺ���ģʽΪ����ģʽ*/	
		TCSR2 &= ~TMR_MODE;
		TCSR2 |=  MODE_PERIOD;		
		
		/* ������� = (Period of timer clock input) * (8-bit Prescale + 1) * (24-bit TCMP)*/
		TCSR2  = TCSR2 & 0xFFFFFF00;		// ����Ԥ��Ƶֵ [0~255]	��ʱ�ķ�ƵֵΪ0
		/*(12000*1000)/12000000     2000ms �ж�һ��*/
		TCMPR2 = 12000*1398;				    // ���ñȽ�ֵ [0~16777215]

		TCSR2 |= TMR_IE;					//ʹ��TMR2�ж�
		NVIC_ISER |= TMR2_INT;	
	
		TCSR2 |= CRST;						//��λTMR2������				
		TCSR2 |= CEN;						//ʹ��TMR2
	)
}


/****************************************
*��������:TMR3Init
*��    ��:��
*��    ��:��
*��    ��:��ʱ��3��ʼ��
******************************************/
VOID TMR3Init(VOID)
{
  PROTECT_REG
	(
		/* ʹ��TMR3ʱ��Դ */
	  APBCLK |= TMR3_CLKEN;
		/* ѡ��TMR3ʱ��ԴΪ�ⲿ����12MHz */  
		CLKSEL1 = (CLKSEL1 & (~TM3_CLK)) | TM3_12M; 	
	
		/* ��λTMR3 */
		IPRSTC2 |=  TMR3_RST;
		IPRSTC2 &= ~TMR3_RST;
	
		/* ѡ��TMR3�Ĺ���ģʽΪ����ģʽ*/	
		TCSR3 &= ~TMR_MODE;
		TCSR3 |=  MODE_PERIOD;		
		
		/* ������� = (Period of timer clock input) * (8-bit Prescale + 1) * (24-bit TCMP)*/
		TCSR3  = TCSR3 & 0xFFFFFF00;		// ����Ԥ��Ƶֵ [0~255]
		TCMPR3 = 12000*1000;				    // ���ñȽ�ֵ [0~16777215]//ʱ���������ʱ��Ϊ1.4s,��������Ϊ1s

		TCSR3 |= TMR_IE;					//ʹ��TMR3�ж�
		NVIC_ISER |= TMR3_INT;	
	
		TCSR3 |= CRST;						//��λTMR3������				
		TCSR3 |= CEN;						//ʹ��TMR3
	)
}
