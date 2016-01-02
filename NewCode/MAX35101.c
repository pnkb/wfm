#include "stm32f30x.h"
#include"stm32f30x_gpio.h"
#include "MAX35101.h"


__IO uint32_t  MAX35101Timeout = MAX35101_FLAG_TIMEOUT;

uint32_t MAX35101_TIMEOUT_UserCallback(void)
{
	uint16_t i=0;
	uint8_t j;

  /* Block communication and all processes */
  while (1)
  {
	  USART_SendData (USART1,'T');// print error message
	  for(j = 0; j < 50; j++){
	  	for(i=0; i<30000; i++);
	  }
  }
}

uint8_t MAX35101_SendByte (uint8_t opcode)
{
	MAX35101Timeout=MAX35101_FLAG_TIMEOUT;
	/* Loop while DR register in not empty */
	while (SPI_I2S_GetFlagStatus(MAX35101_SPI, SPI_I2S_FLAG_TXE) == RESET)
	{
	   if((MAX35101Timeout--) == 0) return MAX35101_TIMEOUT_UserCallback();
	}
	/* Send a Byte through the SPI peripheral */
	SPI_SendData8(MAX35101_SPI, opcode);

	/* Wait to receive a Byte */
	MAX35101Timeout = MAX35101_FLAG_TIMEOUT;
	while (SPI_I2S_GetFlagStatus(MAX35101_SPI, SPI_I2S_FLAG_RXNE) == RESET)
	{
	   if((MAX35101Timeout--) == 0) return MAX35101_TIMEOUT_UserCallback();
	}
	/* Return the Byte read from the SPI bus */

	return (uint8_t)SPI_ReceiveData8(MAX35101_SPI);
}

uint8_t MAX35101_Read_Register(uint8_t address, uint16_t* resutls)
{
	uint8_t success = true, data1, data2;
	MAX35101_CS_LOW();
	MAX35101_SendByte(address);
	data1 = MAX35101_SendByte(DUMMY_BYTE);
	data2 = MAX35101_SendByte(DUMMY_BYTE);
	MAX35101_CS_HIGH();
	*resutls = (uint16_t) (((uint16_t)data1 << 8) | data2);
	return success;
}

uint8_t MAX35101_Send_Opcode(uint8_t opcode)
{
	uint8_t success = true;
	MAX35101_CS_LOW();
	MAX35101_SendByte(opcode);
	MAX35101_CS_HIGH();
	return success;
}

uint8_t MAX35101_Write_Register(uint8_t address, uint16_t DatatoWrite)
{
	uint8_t success=true;
	MAX35101_CS_LOW();
	MAX35101_SendByte(address);
	MAX35101_SendByte((uint8_t)(DatatoWrite>>8));
	MAX35101_SendByte((uint8_t)(DatatoWrite & 0x00FF));
	MAX35101_CS_HIGH();
	return success;
}

uint8_t MAX35101_Read_2WordValue(uint8_t startingAddress, uint32_t * results)
{
	uint8_t success = true, data1, data2, data3, data4;
	MAX35101_CS_LOW();
	MAX35101_SendByte(startingAddress);
	data1 = MAX35101_SendByte(DUMMY_BYTE);
	data2 = MAX35101_SendByte(DUMMY_BYTE);
	data3 = MAX35101_SendByte(DUMMY_BYTE);
	data4 = MAX35101_SendByte(DUMMY_BYTE);
	MAX35101_CS_HIGH();
	*results = (uint16_t) ((uint32_t)data1 << 24) | ((uint32_t)data2<<16) | ((uint16_t)data3 << 8) | data4;

	return success;
}

uint8_t MAX35101_SendConfigs(void)	//need to rewrite
{
	uint8_t success = false;
	uint16_t i=0;
	uint8_t j;

	MAX35101_Write_Register(0x38, 0x1311);
	MAX35101_Write_Register(0x39, 0xA3F2);
	MAX35101_Write_Register(0x3A, 0x0809);
	MAX35101_Write_Register(0x3B, 0x0A0B);
	MAX35101_Write_Register(0x3C, 0x0C0D);
	MAX35101_Write_Register(0x3D, 0x0048);
	MAX35101_Write_Register(0x3E, 0x0048);


	MAX35101_Write_Register(0x3F, 0x0180); 			// TOF_DIFF: 0.5s, 4 cycles, Temp: 1s, 2 cycles.
	MAX35101_Write_Register(0x40, 0x0FCF);			// Use calibration, at the beginning of TOF_DIFF and TEMP sequences.
													// Measure T1, T2, T3. PRECYC = 3, PORTCYC = 512us.


	MAX35101_Write_Register(0x41, 0x0108);
	MAX35101_Write_Register(0x42, 0x0343);	// CMP_EN=0, CMP_SEL=0, INT_EN=1, ET_CONT=1, CONT_INT=0, clock settling=5.13ms, 4MHz calibration=4 32kHz cycles.
	MAX35101_Write_Register(0x43, 0x0080);	// 32K_BP=0, 32K_EN=0(no 32KOUT), enable 32kHz, no alarm, no watch dog.

	MAX35101_Send_Opcode(TRANSFER_TO_FLASH);
	for(j = 0; j < 50; j++){
				for(i=0; i<30000; i++);
			}
	return success;
}
