#include "SmartM_M0.h"
#include "led.h"

void LedInit(void)
{
	P2_PMD &= ~(3UL<<(LED_GATE_PIN <<1));//P2.2
	P2_PMD |= 1UL<<(LED_GATE_PIN <<1);//IOģʽ��P2.2����Ϊ���ģʽ
	
	P4_PMD	&=	~(3UL	<<(RUN_LED_PIN	<<	1));	//IO ģʽ���ƣ�P4.0
	P4_PMD |= 1UL<<(RUN_LED_PIN <<1); //IOģʽ��P4.0����Ϊ���ģʽ
}


void lock_led_on(void)//the led on the board
{
	LED_DATA(0);
}


void lock_led_off(void)//the led on the board
{
	LED_DATA(1);
}
