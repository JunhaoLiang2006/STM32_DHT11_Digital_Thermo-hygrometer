#include "stm32f10x.h"                  // Device header

#include "Delay.h"
#include "OLED.h"
#include "App_W25Q64_Serial.h"
#include "Serial.h"
#include "DHT11.h"

#include "Timer.h"

uint8_t Data[5];
uint8_t Time[100];

float Rh;
float Temp;

int main(void)
{
	Serial_USART1_Init();
	DHT11_Init();
	OLED_Init();
	
	OLED_ShowString(1, 1, "RH:  . %");
	OLED_ShowString(2, 1, " T:  . C");

	while(1)
	{
//		if(Serial_GetRxFlag())
//		{
			DHT11_GetDataPack(Data);
			DHT11_WaitRxFlag();
			if(DHT11_GetData(&Rh, &Temp))
			{
//				Serial_Printf("         RH: %d.%d %%\n", Data[0], Data[1]);
//				Serial_Printf("Temperature: %d.%d degree Celsius\n\n", Data[2], Data[3]);
				OLED_ShowNum(1, 4, Data[0], 2);
				OLED_ShowNum(2, 4, Data[2], 2);
				OLED_ShowNum(1, 7, Data[1], 1);
				OLED_ShowNum(2, 7, Data[3], 1);
			}
			Delay_ms(100);
//			Serial_SendByte('\n');
//		}
	}
}

/*
int main()
{
	App_W25Q64_Serial_Init();
	
	Serial_SendString("Ready.\n\nCommands:\n");
	App_W25Q64_Serial_CheckCommand();
	
	while(1)
	{
		OLED_ShowString(1, 1, "Pending...");
		Serial_SendString("\nWaiting for command...\n\n");
		while(!Serial_GetRxFlag());
		App_W25Q64_Cmd = Serial_RxPacket[0];
			switch(App_W25Q64_Cmd)
			{
				default:
					
					OLED_Clear();
					OLED_ShowString(1, 1, "Invalid Command.");
					Serial_SendString("Invalid Command.\n");
					App_W25Q64_Serial_CheckCommand();
					OLED_Clear();
					break;
				
				case 0x00:
					
					Serial_SendString("Mode: Check Commands.\n");
					OLED_ShowString(1, 1, "Mode: Check Commands.");
					App_W25Q64_Serial_CheckCommand();
					OLED_Clear();
					break;
				
				case 0x01:
					
					App_W25Q64_Read();
					break;
				
				case 0x02:
					
					App_W25Q64_Write();
					break;
					
			}
	}
}*/
