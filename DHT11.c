#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Timer.h"

#define DHT11_BitThreshold 50

extern uint8_t Time[100];

uint8_t DHT11_BitNum;
uint8_t DHT11_Interval;
uint8_t DHT11_StartFlag;
uint8_t DHT11_RxFlag;

uint8_t DHT11_Response;

uint8_t *DHT11_Data;

void DHT11_WriteBit(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)BitValue);
}

void DHT11_Init(void)
{
	Timer_Init();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	DHT11_WriteBit(1);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
}

void DHT11_Mode_In(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void DHT11_Mode_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void DHT11_Reset(void)
{
	DHT11_BitNum = 0;
	DHT11_StartFlag = 0;
	DHT11_Response = 0;
}

void DHT11_Start(void)
{
	for(uint8_t i = 0; i < 5; i++) DHT11_Data[i] = 0;
	DHT11_Mode_Out();
	DHT11_WriteBit(0);
	Delay_ms(20);
	
	DHT11_Mode_In();
	DHT11_StartFlag = 1;
}

uint8_t DHT11_ReadData(void)
{
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
}

uint8_t DHT11_GetRxFlag(void)
{
	if (DHT11_RxFlag == 1)
	{
		DHT11_RxFlag = 0;
		return 1;
	}
	return 0;
}


void DHT11_GetDataPack(uint8_t *Data)
{
	DHT11_Data = Data;
	DHT11_Start();
}

void DHT11_WaitRxFlag(void)
{
	uint32_t Timeout = 100000;
	while(Timeout-- && !DHT11_GetRxFlag());
}

void DHT11_ClearRxFlag(void)
{
	DHT11_RxFlag = 0;
}

uint8_t DHT11_GetData(float *Rh, float *Temp)
{
	if(DHT11_Data[0] + DHT11_Data[1] + DHT11_Data[2] + DHT11_Data[3] == DHT11_Data[4])
	{
		*Rh = DHT11_Data[0] + (float)DHT11_Data[1] / 100;
		*Temp = DHT11_Data[2] + (float)DHT11_Data[3] / 100;
		return 1;
	}
	return 0;
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line11) == SET && DHT11_StartFlag)
	{
		EXTI_ClearITPendingBit(EXTI_Line11);
		if(DHT11_Response < 3)
		{
			DHT11_Response++;
			Timer_Reset();
		}
		else if(DHT11_ReadData())
		{
			Timer_Reset();
		}
		else
		{
			DHT11_Interval = Timer_Count * 2;
			Time[DHT11_BitNum] = DHT11_Interval;
			Timer_Disable();
			DHT11_Data[DHT11_BitNum / 8] |= (DHT11_Interval > DHT11_BitThreshold) ? 0x80 >> (DHT11_BitNum % 8) : 0x00;
			if(DHT11_BitNum == (40 - 1))
			{
				DHT11_Reset();
				DHT11_RxFlag = 1;
			}
			else DHT11_BitNum++;
		}
//		EXTI_ClearITPendingBit(EXTI_Line11);
	}
	EXTI_ClearITPendingBit(EXTI_Line11);
}

