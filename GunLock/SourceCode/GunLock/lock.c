#include "SmartM_M0.h"
#include "lock.h"

extern volatile uint32_t   nDelayLock;

void LockInit(void)
{
	P3_PMD &= ~(3UL<<(LOCK_GATE_PIN<<1));//IOģʽ����,P3.2
	P3_PMD &= ~(1UL<<(LOCK_GATE_PIN<<1));//IOģʽ��P3.2����Ϊ����ģʽ
	
	P3_PMD &= ~(3UL<<(GUN_STATUS_PIN<<1));//IOģʽ����,P3.3
	P3_PMD &= ~(1UL<<(GUN_STATUS_PIN<<1));//IOģʽ��P3.3����Ϊ����ģʽ
	
	P2_PMD &= ~(3UL<<(POWER_CONTROL_PIN <<1));//P2.0
	P2_PMD |= 1UL<<(POWER_CONTROL_PIN <<1);//IOģʽ��P2.0����Ϊ���ģʽ
}


int gun_status(void)
{
	/*the gun is not online*/
	if((P3_PIN & (1 << 3)) == 0)//ǹ���Կ�,�͵�ƽǹ��λ
	{
		return 66;//the gun is not online
	}
	else
	{
		return 98;//��λ
	}
}


void lock_open(void)
{
	nDelayLock=2000;//(uint32_t)(500*10);//500*4 4s
	POWER_DATA(0);
}


void lock_close(void)
{
	nDelayLock=0;
	POWER_DATA(1);	
}


int lock_status(void)
{
	/*lock is open*/
	if((P3_PIN & (1 << 2)) == 0)//�͵�ƽ����
	{	
		lock_cmd = 0;
		return 65;//����
	}
	else
	{
		return 97;//����
	}
}


int lock_control_pin_status(void)
{
	/*MCU control open the lock*/
	if (P2_PIN & (1 << 0) == 0)
	{
		return 0;
	}
	else
	{		
		return 1;
	}
}
