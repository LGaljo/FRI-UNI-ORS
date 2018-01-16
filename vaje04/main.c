/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int main(void)
{
	// init CLOCKS
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// init LED
	GPIO_InitTypeDef led;
	led.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	led.GPIO_Mode = GPIO_Mode_OUT;
	led.GPIO_Speed = GPIO_Speed_2MHz;
	led.GPIO_OType = GPIO_OType_PP;
	led.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOD, &led);

	// init BTN
	GPIO_InitTypeDef btn;
	btn.GPIO_Pin = GPIO_Pin_0;
	btn.GPIO_Mode = GPIO_Mode_IN;
	btn.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA, &btn);

	volatile uint32_t d = 0x01000000;
	
	while (1)
	{
		if( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) & 1 )
		{
			// lights on
			GPIO_SetBits(GPIOD, GPIO_Pin_12);
			while(d--);
			d = 0x01000000;
			GPIO_SetBits(GPIOD, GPIO_Pin_13);
			while(d--);
			d = 0x01000000;
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
			while(d--);
			d = 0x01000000;
			GPIO_SetBits(GPIOD, GPIO_Pin_15);
			while(d--);
			d = 0x01000000;
			// lights off
			GPIO_ResetBits(GPIOD, GPIO_Pin_12);
			GPIO_ResetBits(GPIOD, GPIO_Pin_13);
			GPIO_ResetBits(GPIOD, GPIO_Pin_14);
			GPIO_ResetBits(GPIOD, GPIO_Pin_15);
		}
	}
}
