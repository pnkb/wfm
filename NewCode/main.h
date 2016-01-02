#ifndef MAIN_H_
#define MAIN_H_

// Incude files
#include "stm32f30x.h"
#include"stm32f30x_gpio.h"
#include"stm32f30x_rcc.h"
#include "stm32f30x_spi.h"



void ucInit (void);
void ucGPIOInit (void);
void ucSPI2Init (void);
void ucSPI1Init (void);
void ucSPIInit(void);
void ucUSARTInit (void);
void delay (void);

void uartwriteuint (uint16_t val, uint8_t field_length);
void uartwritefloat(float val, uint8_t decimal_length);
void uart_puts(const char *s );


static uint8_t blink;


#endif //MAIN_H_
