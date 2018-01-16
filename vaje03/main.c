/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define	GPIOAd ((GPIO_device *) 0x40020000)
#define	GPIOBd ((GPIO_device *) 0x40020400)
#define	GPIOCd ((GPIO_device *) 0x40020800)
#define	GPIODd ((GPIO_device *) 0x40020C00)
#define	GPIOEd ((GPIO_device *) 0x40021000)
#define	GPIOFd ((GPIO_device *) 0x40021400)
#define	GPIOGd ((GPIO_device *) 0x40021800)
#define	GPIOHd ((GPIO_device *) 0x40021C00)
#define	GPIOId ((GPIO_device *) 0x40022000)

#define RCCC ((uint32_t *) 0x40023830)

#define	IN 0
#define	OUT 1

#define	NO_PULL 0
#define	PULL_UP 1
#define	PULL_DOWN 2

#define	PUSH_PULL 0
#define	OPEN_DRAIN 1

#define	S2MHz 0
#define	S25MHz 1
#define	S50MHz 2
#define	S100MHz 3

typedef struct
{
	uint32_t MODER;
	uint32_t OTYPER;
	uint32_t OSPEEDR;
	uint32_t PUPDR;
	uint32_t IDR;
	uint32_t ODR;
	uint16_t SET;
	uint16_t RESET;
} GPIO_device; 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void init_GPIO(GPIO_device * GPIO_addr, uint32_t Pin, uint32_t Mode, uint32_t PUPD, uint32_t OType, uint32_t OSpeed);
void ClockOn(GPIO_device * GPIO_addr);
void GPIO_Output(GPIO_device * GPIO_addr, uint32_t Pin, uint32_t val);
uint32_t GPIO_Read(GPIO_device * GPIO_addr, uint32_t Pin);

/* Private functions ---------------------------------------------------------*/

int main(void)
{
	// init CLOCKS
	ClockOn(GPIOAd);
	ClockOn(GPIODd);

	// init LED
	init_GPIO(GPIODd, 12, OUT, NO_PULL, PUSH_PULL, S2MHz);
	init_GPIO(GPIODd, 13, OUT, NO_PULL, PUSH_PULL, S2MHz);
	init_GPIO(GPIODd, 14, OUT, NO_PULL, PUSH_PULL, S2MHz);
	init_GPIO(GPIODd, 15, OUT, NO_PULL, PUSH_PULL, S2MHz);
	
	// init BTN
	init_GPIO(GPIOAd, 0, IN, NO_PULL, PUSH_PULL, S2MHz);

	volatile uint32_t d = 0x01000000;
	
	while (1)
	{
		if((GPIO_Read(GPIOAd,0)) & 1)
		{
			// lights on
			GPIO_Output(GPIODd, 12, 1);
			while(d--);
			d = 0x01000000;
			GPIO_Output(GPIODd, 13, 1);
			while(d--);
			d = 0x01000000;
			GPIO_Output(GPIODd, 14, 1);
			while(d--);
			d = 0x01000000;
			GPIO_Output(GPIODd, 15, 1);
			while(d--);
			d = 0x01000000;
			// lights off
			GPIO_Output(GPIODd, 12, 0);
			GPIO_Output(GPIODd, 13, 0);
			GPIO_Output(GPIODd, 14, 0);
			GPIO_Output(GPIODd, 15, 0);
			while(d--);
			d = 0x01000000;
		}
	}
}

void init_GPIO(GPIO_device * GPIO_addr, uint32_t Pin, uint32_t Mode, uint32_t PUPD, uint32_t OType, uint32_t OSpeed) {
    GPIO_addr->MODER &= ~(3 << 2*Pin);
    GPIO_addr->MODER |= Mode << 2*Pin;
    GPIO_addr->PUPDR &= ~(3 << 2*Pin);
    GPIO_addr->PUPDR |= PUPD << 2*Pin;
	
    if (Mode == 1) {
    // izhod
        GPIO_addr->OTYPER &= ~(1 << Pin);
        GPIO_addr->OTYPER |= OType << Pin;
        GPIO_addr->OSPEEDR &= ~(3 << Pin*2);
		GPIO_addr->OSPEEDR |= OSpeed << Pin*2;
    }
}

void ClockOn(GPIO_device * GPIO_addr) {
	uint32_t bit_position = (uint32_t)((uint32_t)GPIO_addr - (uint32_t)GPIOAd) / 0x400;
	*RCCC |= 1 << bit_position;
}

void GPIO_Output(GPIO_device * GPIO_addr, uint32_t Pin, uint32_t val) {
	/*
	if (val == 0) {
		GPIO_addr->RESET = 1 << Pin;
	} else {
		GPIO_addr->SET = 1 << Pin;
	}
	*/
	GPIO_addr -> ODR = (GPIO_addr -> ODR & ~(1 << Pin)) | (val << Pin);
}

uint32_t GPIO_Read(GPIO_device * GPIO_addr, uint32_t Pin) {
	uint32_t t = GPIO_addr->IDR & (1 << Pin);
	if (t == 0) {
		return 0;
	}  else {
		return 1;
	}
}
