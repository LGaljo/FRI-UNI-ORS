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

	// timer 4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	// init TIM4
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 10000;
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitTypeDef TIM_OCInitStructure1;
	TIM_OCInitStructure1.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure1.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure1.TIM_Pulse = 10000;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure1);

	TIM_OCInitTypeDef TIM_OCInitStructure2;
	TIM_OCInitStructure2.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure2.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure2.TIM_Pulse = 5000;
	TIM_OC2Init(TIM4, &TIM_OCInitStructure2);

	// init LED
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);

	TIM_Cmd(TIM4, ENABLE);

	// init LED classic
	GPIO_InitTypeDef led;
	led.GPIO_Pin = GPIO_Pin_14;
	led.GPIO_Mode = GPIO_Mode_OUT;
	led.GPIO_Speed = GPIO_Speed_2MHz;
	led.GPIO_OType = GPIO_OType_PP;
	led.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &led);

	volatile int counter = 0;

	while (1)
	{
		if (TIM_GetFlagStatus(TIM4, TIM_FLAG_CC1)) {
			counter++;

			if (counter == 5) {
				GPIO_SetBits(GPIOD, GPIO_Pin_14);
			} else if (counter == 6) {
				GPIO_ResetBits(GPIOD, GPIO_Pin_14);
				counter = 0;
			}

			TIM_ClearFlag(TIM4, TIM_FLAG_CC1);
		}
	}
}
