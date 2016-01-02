#include "main.h"
#include "MAX35101.h"
#include "stm32f30x_usart.h"

#define RST_LOW 	GPIO_ResetBits(GPIOC, GPIO_Pin_3)
#define RST_HIGH 	GPIO_SetBits (GPIOC, GPIO_Pin_3)

#define HB_LED_ON	GPIO_SetBits (GPIOE, GPIO_Pin_14)
#define HB_LED_OFF	GPIO_ResetBits(GPIOE, GPIO_Pin_14)

#define TEST_LED_ON	GPIO_SetBits (GPIOE, GPIO_Pin_15)
#define TEST_LED_OFF	GPIO_ResetBits(GPIOE, GPIO_Pin_15)

#define CMP_OUT		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)
#define WDO_OUT		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)
#define INT_OUT		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)

uint16_t timeresults, InterruptRegisterValue;

int main()
{
	uint16_t i=0;
	uint8_t j, tx = 'D', rx;

	ucInit();
	delay();
	MAX35101_SendConfigs ();
	TEST_LED_ON;
	RST_LOW;
	MAX35101_Send_Opcode(RESET);
	delay();
	MAX35101_Send_Opcode(INITIALIZE);
	delay();
	MAX35101_Write_Register(MINS_HRS, 0x5923);	//Set the time
	MAX35101_Write_Register(DAY_DATE, 0x0019);	//Set the Date
	MAX35101_Write_Register(MONTH_YEAR, 0x0914);//Set the year
	delay();
	uart_puts ("WFM\r\n");
	delay(); delay();
	TEST_LED_OFF;
	while (1)
	{
		MAX35101_CS_LOW ();
		rx = MAX35101_SendByte(tx);
		USART_SendData(USART1, rx);
		MAX35101_CS_HIGH ();

		if(blink)
		{
			HB_LED_ON;//GPIO_SetBits (GPIOE, GPIO_Pin_14);
			TEST_LED_OFF;
		}
		else
		{
			HB_LED_OFF;//GPIO_ResetBits(GPIOE, GPIO_Pin_14);
			TEST_LED_ON;
		}

		for(j = 0; j < 50; j++){
			for(i=0; i<30000; i++);
		}
		delay();
		blink ^= 1;
		delay();
		USART_SendData (USART1,'O');
		USART_SendData (USART1,'K');
		delay();
		USART_SendData (USART1,'\n');

	}
	return 0;
}

void delay (void)
{
	uint16_t i=0;
		uint8_t j;

	for(j = 0; j < 10; j++){
				for(i=0; i<20000; i++);
			}
}

void ucInit (void)
{
	ucGPIOInit();
	//ucSPI1Init ();
	//ucSPI2Init ();
	ucUSARTInit ();
	//ucSPIInit();
	MAX35101Init();
}

void ucGPIOInit(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	/* GPIOE Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOE|RCC_AHBPeriph_GPIOC, ENABLE);


	/* Configure PE14 and PE15 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* Configure PE14 and PE15 in output pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void MAX35101Init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	/* Enable the SPI periph */
	RCC_APB1PeriphClockCmd(MAX35101_SPI_CLK, ENABLE);
	/* Enable SCK, MOSI and MISO GPIO clocks */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOE|RCC_AHBPeriph_GPIOC, ENABLE);


	GPIO_InitStructure.GPIO_Pin = MAX35101_SPI_SCK_PIN|MAX35101_SPI_MOSI_PIN|MAX35101_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(MAX35101_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	GPIO_PinAFConfig(MAX35101_SPI_SCK_GPIO_PORT, MAX35101_SPI_SCK_SOURCE, MAX35101_SPI_SCK_AF);
	GPIO_PinAFConfig(MAX35101_SPI_MISO_GPIO_PORT, MAX35101_SPI_MISO_SOURCE, MAX35101_SPI_MISO_AF);
	GPIO_PinAFConfig(MAX35101_SPI_MOSI_GPIO_PORT, MAX35101_SPI_MOSI_SOURCE, MAX35101_SPI_MOSI_AF);

	/* Configure GPIO PIN for Chip select */
	GPIO_InitStructure.GPIO_Pin = MAX35101_SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;//GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(MAX35101_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

	/* SPI configuration -------------------------------------------------------*/
	SPI_I2S_DeInit(MAX35101_SPI);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(MAX35101_SPI, &SPI_InitStructure);

	/* Configure the RX FIFO Threshold */
	SPI_RxFIFOThresholdConfig(MAX35101_SPI, SPI_RxFIFOThreshold_QF);

	/* Deselect : Chip Select high */
	GPIO_SetBits(MAX35101_SPI_CS_GPIO_PORT, MAX35101_SPI_CS_PIN);

	/* Configure GPIO PINs to detect Interrupts */
/*	GPIO_InitStructure.GPIO_Pin = MAX35101_SPI_INT1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(MAX35101_SPI_INT1_GPIO_PORT, &GPIO_InitStructure);
	*/
	/* Enable SPI1  */
	SPI_Cmd(MAX35101_SPI, ENABLE);
}


void ucSPIInit (void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOE|RCC_AHBPeriph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	//// SPI Pin initialization
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_5); // SPI SCK
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_5); // SPI MISO
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_5); // SPI MOSI

	//CS Initialization
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI_I2S_DeInit(SPI2);
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; // Slow
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_Init(SPI2, &SPI_InitStructure);
	/* Configure the RX FIFO Threshold */
	SPI_RxFIFOThresholdConfig(SPI2, SPI_RxFIFOThreshold_QF);

	SPI_Cmd(SPI2, ENABLE);

}

void ucUSARTInit (void)
{
	USART_InitTypeDef USART_InitStruct;
		GPIO_InitTypeDef GPIO_InitStruct;
		USART_ClockInitTypeDef USART_ClockInitStructure;
//https://code.google.com/p/lxyppc-tetrix/source/browse/trunk/quad/ctrl/boards/
		//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOE, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

		/* Reset GPIO init structure parameters values */
		GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_9|GPIO_Pin_10;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

		GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_7);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);

		USART_ClockStructInit(&USART_ClockInitStructure);
		USART_ClockInit(USART1, &USART_ClockInitStructure);

		USART_StructInit(&USART_InitStruct);
		USART_InitStruct.USART_BaudRate = 115200;
		USART_Init(USART1, &USART_InitStruct);
		USART_Cmd(USART1, ENABLE);
}

void uartwriteuint (uint16_t val, uint8_t field_length)
{
	/***************************************************************
	This function writes a integer type value to LCD module

	Arguments:
	1)int val	: Value to print

	2)unsigned int field_length :total length of field in which the value is printed
	must be between 1-5 if it is -1 the field length is no of digits in the val

	****************************************************************/

	char str[6]={0,0,0,0,0,0};
	int i=5,j=0;
	uint16_t local_copy = val;

	while(val)
	{
		str[i]=val%10;
		val=val/10;
		i--;
	}
	if(field_length==-1)
		while(str[j]==0) j++;
	else
	{
		if(local_copy <= 9){j = 5;}
		else if(local_copy <= 99){j = 4;}
		else if(local_copy <= 999){j = 3;}
		else if(local_copy <= 9999){j = 2;}
		else if(local_copy <= 99999){j = 1;}
		else if(local_copy <= 999999)j = 0;
		//j=6-field_length;
	}

	if(val<0)
		USART_SendData(USART1, '-');

	for(i=j;i<6;i++)
	{
		USART_SendData(USART1, 48+str[i]);;
	}
}


void uartwritefloat(float val, uint8_t decimal_length) // Decimal Length between 1 to 6
{
	/***************************************************************
	This function writes a integer type value to LCD module

	Arguments:
	1)int val	: Value to print

	2)unsigned int field_length :total length of field in which the value is printed
	must be between 1-5 if it is -1 the field length is no of digits in the val

	****************************************************************/
	char str[6]={0,0,0,0,0,0};
	char dec_str[6] = {0, 0, 0, 0, 0, 0};
	int i=5,j=0, m = 5, n = 0;
	uint16_t Process_val_int, Process_val_dec, local_copy_dec = 0;
	uint16_t int_component;
	float dec_component;

	if(val < 0)
	{
		USART_SendData(USART1,'-');
		val = -1*val;
		int_component = val;
		dec_component = (val - int_component);
	}
	else
	{
		int_component = val;
		dec_component = (val - int_component);
	}

	if(decimal_length == 1) {local_copy_dec = (uint16_t) (dec_component * 10);n = 5;}
	else if(decimal_length == 2){local_copy_dec = (uint16_t) (dec_component * 100);n = 4;}
	else if(decimal_length == 3){local_copy_dec = (uint16_t) (dec_component * 1000);n = 3;}
	else if(decimal_length == 4){local_copy_dec = (uint16_t) (dec_component * 10000);n = 2;}
	else if(decimal_length == 5){local_copy_dec = (uint16_t) (dec_component * 100000);n = 1;}
	else if(decimal_length == 6){local_copy_dec = (uint16_t) (dec_component * 100000);n = 0;}

	Process_val_int = int_component;

	while(Process_val_int){
		str[i]=Process_val_int%10;
		Process_val_int=Process_val_int/10;
		i--;
	}

	Process_val_dec = local_copy_dec;

	while(Process_val_dec)
	{
		dec_str[m] = Process_val_dec%10;
		Process_val_dec = Process_val_dec/10;
		m--;
	}

	if(int_component <= 9){j = 5;}
	else if(int_component <= 99){j = 4;}
	else if(int_component <= 999){j = 3;}
	else if(int_component <= 9999){j = 2;}
	else if(int_component <= 99999){j = 1;}
	else if(int_component <= 999999){j = 0;}

	for(i=j;i<6;i++)
	{
		USART_SendData(USART1, 48+str[i]);
	}

	USART_SendData(USART1, '.');
	for(m=n;m<6;m++)
	{
		USART_SendData(USART1, 48+dec_str[m]);
		delay();
	}
}

/*************************************************************************
Function: uart_puts()
Purpose:  transmit string to UART
Input:    string to be transmitted
Returns:  none
**************************************************************************/
void uart_puts(const char *s )
{
    while (*s)
    	USART_SendData(USART1, *s++);

}/* uart_puts */



