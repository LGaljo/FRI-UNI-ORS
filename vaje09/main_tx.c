// Tx

/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile int8_t tabela[2];
/* Private function prototypes -----------------------------------------------*/
void delay(uint32_t delay);

/* Private functions ---------------------------------------------------------*/
void delay(uint32_t delay) {
  while(delay--);
}

// write to SPI1
void SPI1_Write(int8_t data)
{
  // short delay
  volatile int d = 500;
  while(d--);

  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
  SPI_I2S_SendData(SPI1, data);

  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){}
  SPI_I2S_ReceiveData(SPI1);
}

// read from SPI1
int8_t SPI1_Read()
{
  // short delay
  volatile int d = 500;
  while(d--);

  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
  SPI_I2S_SendData(SPI1, 0x00);

  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){}
  return SPI_I2S_ReceiveData(SPI1);
}

void initSPI1(void)
{
  // RCC
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // PA5, PA6, PA7 for MISO, MOSI, and SCLK
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

  // SPI1 INIT
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

  // SPI1 ENABLE
  SPI_Cmd(SPI1, ENABLE);
}

void initMotionSensor()
{
  // RCC
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  // PE3 for slave select
  GPIO_InitTypeDef ss;
  ss.GPIO_Pin = GPIO_Pin_3;
  ss.GPIO_Mode = GPIO_Mode_OUT;
  ss.GPIO_OType = GPIO_OType_PP;
  ss.GPIO_PuPd = GPIO_PuPd_NOPULL;
  ss.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOE, &ss);
  GPIO_SetBits(GPIOE, GPIO_Pin_3);

  // configure and start sensor
  delay(500);
  GPIO_ResetBits(GPIOE, GPIO_Pin_3);
  // Zapisem inicializacijo senzorja
  // Zapisem 0x47 na naslov 0x20
  SPI1_Write(0x20);
  SPI1_Write(0x47);
  // Deaktiviram slave
  GPIO_SetBits(GPIOE, GPIO_Pin_3);
  delay(500);
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

  // PC6 and PC7 for USART6 Tx and Rx
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
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) tabela;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(USART6->DR);
  DMA_InitStructure.DMA_BufferSize = 2;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  //DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  //DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream6, &DMA_InitStructure);

  // enable USART6 DMA Tx
  USART_DMACmd(USART6, USART_DMAReq_Tx, ENABLE);

  // enable DMA2 stream
  DMA_Cmd(DMA2_Stream6, ENABLE);  // enable USART6 DMA Rx

}

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  // SPI1
  initSPI1();
  
  // LIS motion sensor
  initMotionSensor();
  
  // USART6
  initUSART6();
  
  // DMA2
  initDMA2();
  
  while (1)
  {
    // read x
    // Branje na naslovu 0x29 -- x os
    GPIO_ResetBits(GPIOE, GPIO_Pin_3);
    SPI1_Write(0x29 | 0x80);
    tabela[0] = SPI1_Read();
    GPIO_SetBits(GPIOE, GPIO_Pin_3);
    delay(500);

    // read y
    // Branje na naslovu 0x2B -- y os
    GPIO_ResetBits(GPIOE, GPIO_Pin_3);
    SPI1_Write(0x2B | 0x80);
    tabela[1] = SPI1_Read();
    GPIO_SetBits(GPIOE, GPIO_Pin_3);
  }
}