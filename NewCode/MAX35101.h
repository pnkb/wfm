#ifndef MAX35101_H
#define MAX35101_H
#include "stm32f30x.h"
#include "stm32f30x_usart.h"
#include "stm32f30x_spi.h"

/*************************************** SPI ***********************************/
#define MAX35101_SPI 						SPI2

#define MAX35101_FLAG_TIMEOUT 				((uint32_t)0x1000)
#define DUMMY_BYTE							((uint8_t)0x00)

#define MAX35101_SPI_CLK					RCC_APB1Periph_SPI2

#define MAX35101_SPI_SCK_PIN             	GPIO_Pin_13                  /* PB.13 */
#define MAX35101_SPI_SCK_GPIO_PORT         	GPIOB                       /* GPIOA */
#define MAX35101_SPI_SCK_GPIO_CLK          	RCC_AHBPeriph_GPIOB
#define MAX35101_SPI_SCK_SOURCE            	GPIO_PinSource13
#define MAX35101_SPI_SCK_AF                	GPIO_AF_5

#define MAX35101_SPI_MISO_PIN              	GPIO_Pin_14                 /* PB.14 */
#define MAX35101_SPI_MISO_GPIO_PORT        	GPIOB                       /* GPIOA */
#define MAX35101_SPI_MISO_GPIO_CLK         	RCC_AHBPeriph_GPIOB
#define MAX35101_SPI_MISO_SOURCE           	GPIO_PinSource14
#define MAX35101_SPI_MISO_AF               	GPIO_AF_5

#define MAX35101_SPI_MOSI_PIN              	GPIO_Pin_15                 /* PB.14 */
#define MAX35101_SPI_MOSI_GPIO_PORT        	GPIOB                       /* GPIOB */
#define MAX35101_SPI_MOSI_GPIO_CLK         	RCC_AHBPeriph_GPIOB
#define MAX35101_SPI_MOSI_SOURCE           	GPIO_PinSource15
#define MAX35101_SPI_MOSI_AF               	GPIO_AF_5

#define MAX35101_SPI_CS_PIN                	GPIO_Pin_12                  /* PE.03 */
#define MAX35101_SPI_CS_GPIO_PORT          	GPIOB                       /* GPIOB */
#define MAX35101_SPI_CS_GPIO_CLK           	RCC_AHBPeriph_GPIOB

#define MAX35101_SPI_INT1_PIN              	GPIO_Pin_1                  /* PB.1 */
#define MAX35101_SPI_INT1_GPIO_PORT        	GPIOB                       /* GPIOB */
#define MAX35101_SPI_INT1_GPIO_CLK         	RCC_AHBPeriph_GPIOB
#define MAX35101_SPI_INT1_EXTI_LINE        	EXTI_Line1
#define MAX35101_SPI_INT1_EXTI_PORT_SOURCE 	EXTI_PortSourceGPIOB
#define MAX35101_SPI_INT1_EXTI_PIN_SOURCE  	EXTI_PinSource1
#define MAX35101_SPI_INT1_EXTI_IRQn        	EXTI1_IRQn

#define MAX35101_CS_LOW()       GPIO_ResetBits(MAX35101_SPI_CS_GPIO_PORT, MAX35101_SPI_CS_PIN)
#define MAX35101_CS_HIGH()      GPIO_SetBits(MAX35101_SPI_CS_GPIO_PORT, MAX35101_SPI_CS_PIN)

/*******************************Opcode and Register Definition*********************************/
//MAX35101 Opcode Defines
#define TOF_UP				0x00
#define TOF_DOWN			0x01
#define TOF_DIFF			0x02
#define TEMPERATURE			0x03
#define RESET				0x04
#define INITIALIZE			0x05
#define TRANSFER_TO_FLASH	0x06
#define EVTMG1				0x07
#define EVTMG2				0x08
#define EVTMG3				0x09
#define HALT				0x0a
#define LDO_TIMED			0x0b
#define LDO_ON				0x0c
#define LDO_OFF				0x0d
#define CALIBRATES			0x0e
#define READ_FLASH			0x90
#define WRITE_FLASH			0x10
#define BLOCK_ERASE_FLASH	0x13

//MAX35101 Register Memory Map Base Address
//(Write Opcode Values, Read routines will set the MSBit to get correct read opcode value)
#define SECONDS 			0x30
#define MINS_HRS 			0x31
#define DAY_DATE			0x32
#define MONTH_YEAR			0x33
#define INTERRUPT_STATUS 	0xFE
#define T1_REG				0xE7
#define T2_REG				0xE9
#define T3_REG				0xEB
#define T4_REG				0xED
#define T1_AVG				0xF0
#define T2_AVG				0xF2
#define T3_AVG				0xF4
#define T4_AVG				0xF6
#define HIT1UP_REG			0xC5
#define HIT2UP_REG			0xC7
#define HIT3UP_REG			0xC9
#define HIT4UP_REG			0xCB
#define HIT5UP_REG			0xCD
#define HIT6UP_REG			0xCF
#define AVGUP_REG			0xD1
#define HIT1DOWN_REG		0xD4
#define HIT2DOWN_REG		0xD6
#define HIT3DOWN_REG		0xD8
#define HIT4DOWN_REG		0xDA
#define HIT5DOWN_REG		0xDC
#define HIT6DOWN_REG		0xDE
#define AVGDOWN_REG			0xE0
#define TOF_DIFF_REG		0xE2
#define TOF_DIFF_AVG_REG	0xE5

//MAX35101 Bit weighting definition for interrupt status Register
#define INTERRUPT_REG_TO 			0x8000
#define INTERRUPT_REG_TOF 			0x1000
#define INTERRUPT_REG_TE 			0x0800
#define INTERRUPT_REG_TOF_EVTMG 	0x0200
#define INTERRUPT_REG_TEMP_EVTMG	0x0100
#define INTERRUPT_REG_TOF_FLASH 	0x0080
/*******************************END - Opcode and Register Definition*********************************/
enum {false=0, true=1};

void MAX35101Init(void);
uint8_t MAX35101_SendConfigs(void);
uint8_t MAX35101_SendByte (uint8_t byte);
uint8_t MAX35101_Read_2WordValue(uint8_t startingAddress, uint32_t * results);
uint8_t MAX35101_Write_Register(uint8_t address, uint16_t DatatoWrite);
uint8_t MAX35101_Send_Opcode(uint8_t opcode);
uint8_t MAX35101_Read_Register(uint8_t address, uint16_t* resutls);
uint32_t MAX35101_TIMEOUT_UserCallback(void);

#endif // MAX35101_H
