#include "stm32f10x.h"
#include "OLED_Font.h"

/* pin 腳設定*/
#define OLED_W_SCL(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(x)) //決定 OLED 的 SCL 接在板上的哪個 pin
#define OLED_W_SDA(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_9, (BitAction)(x)) //決定 OLED 的 SDA 接在板上的哪個 pin


// pin 初始化
void OLED_I2C_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 開漏輸出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C 開始
  * @param  無
  * @retval 無
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);
	OLED_W_SCL(1);
	OLED_W_SDA(0);
	OLED_W_SCL(0);
}

/**
  * @brief  I2C 停止
  * @param  無
  * @retval 無
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C 發送一個字節
  * @param  Byte 欲傳送的 8-bit 資料內容
  * @retval 無（此函式不回傳值）
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(!!(Byte & (0x80 >> i))); // 0x80 是 1000 0000，代表最左邊那一位（MSB），>>i 表示每次右移一位 // Byte & ... 讓你得到「該位是否為 1」 // !!x 將結果壓成純 0 或 1
		OLED_W_SCL(1);
		OLED_W_SCL(0);
	}
	OLED_W_SCL(1);	
	OLED_W_SCL(0);
}

/**
  * @brief  OLED 寫命令
  * @param  Command 要寫入的命令
  * @retval 無（此函式不回傳值）
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		
	OLED_I2C_SendByte(0x00);		
	OLED_I2C_SendByte(Command); 
	OLED_I2C_Stop();
}

/**
  * @brief  OLED 寫數據
  * @param  Data 要寫入的數據
  * @retval 無（此函式不回傳值）
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		
	OLED_I2C_SendByte(0x40);		
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED 設置光標位置
  * @param  Y 以左上角為原點，向下方向的座標，範圍：0~7
  * @param  X 以左上角為原點，向右方向的座標，範圍：0~127
  * @retval 無（此函式不回傳值）
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	
	OLED_WriteCommand(0x00 | (X & 0x0F));			
}

/**
  * @brief  OLED 清空螢幕
  * @param  無
  * @retval 無
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED
  * @param  Line 
  * @param  Column 
  * @param  Char 
  * @retval 無
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		
	}
}

/**
  * @brief  OLED 顯示 string
  * @param  Line 起始行位置，範圍 1-4
  * @param  Column 起始列位置，範圍 1-16
  * @param  String String 要顯示的內容，範圍：ASCII 可見的字符
  * @retval 無（此函式不回傳值）
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED 次方函數
  * @retval 返回值等於 X 的 Y 次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED 顯示數字 (十進制，正數)
  * @param  Line 起始行位置，範圍 1-4
  * @param  Column 起始列位置，範圍 1-16
  * @param  Number 要顯示的數字，範圍：0 - 4294967295
  * @param  Length 要顯示的數字的長度，範圍：1-10
  * @retval 無（此函式不回傳值）
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED 顯示數字 (十進制，帶符號數)
  * @param  Line 起始行位置，範圍 1-4
  * @param  Column 起始列位置，範圍 1-16
  * @param  Number 要顯示的數字，範圍：-2147483648 - 2147483647
  * @param  Length 要顯示的數字的長度，範圍：1-10
  * @retval 無（此函式不回傳值）
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED 顯示數字 (十六進制，正數)
  * @param  Line 起始行位置，範圍 1-4
  * @param  Column 起始列位置，範圍 1-16
  * @param  Number 要顯示的數字，範圍：0-0xFFFFFFFF
  * @param  Length 要顯示的數字的長度，範圍：1-8
  * @retval 無（此函式不回傳值）
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED
  * @param  Line 
  * @param  Column 
  * @param  Number 
  * @param  Length 
  * @retval 
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  OLED
  * @param  
  * @retval 
  */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			
	
	OLED_WriteCommand(0xAE);	
	
	OLED_WriteCommand(0xD5);	
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	
	
	OLED_WriteCommand(0xA1);	
	
	OLED_WriteCommand(0xC8);	

	OLED_WriteCommand(0xDA);	
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	

	OLED_WriteCommand(0xA6);	

	OLED_WriteCommand(0x8D);	
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	
		
	OLED_Clear();				
}
