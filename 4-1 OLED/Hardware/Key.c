#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //因為要讀取按鍵，所以是上拉輸入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_9 | GPIO_Pin_11);
}

uint8_t Key_GetNum(void) //uint8_t 是 unsigned char 的意思
{
	uint8_t KeyNum = 0; //默認為0，若無按鍵按下，就 return 0
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == 0)
	{
		Delay_ms(20); //因為會有一段抖動時間
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == 0) //一直是按下的狀態的話
		{
			Delay_ms(20);
			KeyNum = 1;
		}
	}
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0)
	{
		Delay_ms(20); //因為會有一段抖動時間
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0) //一直是按下的狀態的話
		{
			Delay_ms(20);
			KeyNum = 2;
		}
	}
	
	return KeyNum;
}
