// Rx

/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int8_t tabela[16];

/* Private function prototypes -----------------------------------------------*/
void delay(uint32_t delay);

/* Private functions ---------------------------------------------------------*/
void delay(uint32_t delay) {
  while(delay--);
}

void initLED()
{
  // RCC
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  // init PD12-PD15
  GPIO_InitTypeDef leds;
  leds.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  leds.GPIO_Mode = GPIO_Mode_OUT;
  leds.GPIO_OType = GPIO_OType_PP;
  leds.GPIO_PuPd = GPIO_PuPd_NOPULL;
  leds.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOD, &leds);
}

void initUSART6()
{
  // RCC
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); // USART naprava
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);  // Za pine, ki jih uporablja USART

  // USART6 init
  USART_InitTypeDef USART_InitStruct;
  USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART6, &USART_InitStruct);

  // USART6 enable
  USART_Cmd(USART6, ENABLE);

  // PB6 and PB7 for USART6 Tx and Rx
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
}

void initDMA2()
{
  // RCC
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

  // init DMA2
  DMA_InitTypeDef DMA_InitStructure;
  DMA_InitStructure.DMA_Channel = DMA_Channel_5;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) tabela;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(USART6->DR);
  DMA_InitStructure.DMA_BufferSize = 16;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  //DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  //DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);
          USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);

  // enable DMA2 stream
  DMA_Cmd(DMA2_Stream1, ENABLE);  // enable USART6 DMA Rx
}

// calculate average x
int8_t avgX()
{
    return (tabela[0]+tabela[2]+tabela[4]+tabela[6]+tabela[8]+tabela[10]+tabela[12]+tabela[14])/8;
}

// calculate average y
int8_t avgY()
{
  return (tabela[1]+tabela[3]+tabela[5]+tabela[7]+tabela[9]+tabela[11]+tabela[13]+tabela[15])/8;

}

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  // LED
  initLED();
  
  // USART6
  initUSART6();
  
  // DMA2
  initDMA2();
  
  volatile int8_t x, y;
  
  while (1)
  {
    // get average x and y
    x = avgX();
    y = avgY();
    
    // x-axis leds
    if (x < -6) {
      // Vklopi ledico
      GPIO_SetBits(GPIOD, GPIO_Pin_12);
      delay(500);
      GPIO_ResetBits(GPIOD, GPIO_Pin_12);
    } 

    if (x > 6) {
      // Vklopi drugo ledico
      GPIO_SetBits(GPIOD, GPIO_Pin_14);
      delay(500);
      GPIO_ResetBits(GPIOD, GPIO_Pin_14);
    }
    
    // y-axis leds
    if (y < -6) {
      // Vklopi ledico
      GPIO_SetBits(GPIOD, GPIO_Pin_13);
      delay(500);
      GPIO_ResetBits(GPIOD, GPIO_Pin_13);
    } 

    if (y > 6) {
      // Vklopi drugo ledico
      GPIO_SetBits(GPIOD, GPIO_Pin_15);
      delay(500);
      GPIO_ResetBits(GPIOD, GPIO_Pin_15);
    }
  }
}