/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DELAY 0x01000000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SPI1_Write(int8_t data);
int8_t SPI1_Read();
void delay(uint32_t delay);

/* Private functions ---------------------------------------------------------*/
void SPI1_Write(int8_t data) {
	// Pisanje
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {}
	SPI_I2S_SendData(SPI1, data);

	// Slepo branje
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {}
	SPI_I2S_ReceiveData(SPI1);
}

int8_t SPI1_Read() {
	// Slepo pisanje
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {}
	SPI_I2S_SendData(SPI1, 0x00);

	// branje
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {}
	return SPI_I2S_ReceiveData(SPI1);
}

void delay(uint32_t delay) {
	while(delay--);
}

int main(void)
{
	// Clock init
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	// led init
	GPIO_InitTypeDef leds;
	leds.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	leds.GPIO_Mode = GPIO_Mode_OUT;
	leds.GPIO_OType = GPIO_OType_PP;
	leds.GPIO_PuPd = GPIO_PuPd_NOPULL;
	leds.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &leds);

	// GPIO Pin init
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	// ms init
	GPIO_InitTypeDef ss;
	ss.GPIO_Pin = GPIO_Pin_3;
	ss.GPIO_Mode = GPIO_Mode_OUT;
	ss.GPIO_OType = GPIO_OType_PP;
	ss.GPIO_PuPd = GPIO_PuPd_NOPULL;
	ss.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE, &ss);
	GPIO_SetBits(GPIOE, GPIO_Pin_3);
	
	// Init SPI1
	SPI_InitTypeDef spi;
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_NSS = SPI_NSS_Soft;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	spi.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &spi);

	// Vklop SPI1
	SPI_Cmd(SPI1, ENABLE);

	// Aktiviram slave
	delay(500);
	GPIO_ResetBits(GPIOE, GPIO_Pin_3);
	// Zapisem inicializacijo senzorja
	// Zapisem 0x47 na naslov 0x20
	SPI1_Write(0x20);
	SPI1_Write(0x47);
	// Deaktiviram slave
	GPIO_SetBits(GPIOE, GPIO_Pin_3);
	delay(500);
	
	while (1)
	{
		// Branje na naslovu 0x29 -- x os
		GPIO_ResetBits(GPIOE, GPIO_Pin_3);
		SPI1_Write(0x29 | 0x80);
		volatile int8_t x = SPI1_Read();
		GPIO_SetBits(GPIOE, GPIO_Pin_3);

		delay(500);

		// Branje na naslovu 0x2B -- y os
		GPIO_ResetBits(GPIOE, GPIO_Pin_3);
		SPI1_Write(0x2B | 0x80);
		volatile int8_t y = SPI1_Read();
		GPIO_SetBits(GPIOE, GPIO_Pin_3);

		if (x < -5) {
			// Vklopi ledico
			GPIO_SetBits(GPIOD, GPIO_Pin_12);
			delay(500);
			GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		}

		if (x > 5) {
			// Vklopi drugo ledico
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
			delay(500);
			GPIO_ResetBits(GPIOD, GPIO_Pin_14);
		}

		if (y < -5) {
			// Vklopi ledico
			GPIO_SetBits(GPIOD, GPIO_Pin_15);
			delay(500);
			GPIO_ResetBits(GPIOD, GPIO_Pin_15);
		}

		if (y > 5) {
			// Vklopi drugo ledico
			GPIO_SetBits(GPIOD, GPIO_Pin_13);
			delay(500);
			GPIO_ResetBits(GPIOD, GPIO_Pin_13);
		}
	}
}
