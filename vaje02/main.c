/* Includes ------------------------------------------------------------------*/
#include "stm32f4_discovery.h"

// Inicializacijski registri naprave
#define INIT1 ((uint32_t*)0x40023830)
#define INIT2 ((uint32_t*)0x40020C00)
#define INIT3 ((uint32_t*)0x40020C04)
#define INIT4 ((uint32_t*)0x40020C08)
#define INIT5 ((uint32_t*)0x40020C0C)
#define INIT6 ((uint32_t*)0x40020000)
#define INIT7 ((uint32_t*)0x4002000C)

// IO naprave
#define BUTT ((uint16_t*)0x40020010)
#define LED_ON_REGISTER ((uint16_t*)0x40020C18) //SET REGISTER
#define LED_OFF_REGISTER ((uint16_t*)0x40020C1A) //RESET REGISTER

#define G 0x1000
#define Y 0x2000
#define GY 0x3000
#define R 0x4000
#define RG 0x5000
#define RY 0x6000
#define RGY 0x7000
#define B 0x8000
#define GB 0x9000
#define BY 0xA000
#define GBY 0xB000
#define RB 0xC000
#define RGB 0xD000
#define RBY 0xE000
#define RGBY 0xF000

#define DELAY 5000000

// Vrednosti
#define VAL1 0x9
#define VAL2 0x55555555
#define VAL3 0x0
#define VAL4 0xA8000000
#define VAL5 0x64000000

void delay() {
  for (volatile int d = 0; d < DELAY; d++) {
    if ((*BUTT & 0x1) == 1) return;
  }
  return;
}

int main(void)
{  
  *INIT1 = VAL1;
  *INIT2 = VAL2;
  *INIT3 = VAL3;
  *INIT4 = VAL3;
  *INIT5 = VAL3;
  *INIT6 = VAL4;
  *INIT7 = VAL5;
  /*
  WORK IN SCHOOL
  while (1) {
    if((*BUTT & 0x1) == 1) {
      *LED_ON_REGISTER = RGBY;
      for (volatile int d = 0; d < DELAY; d++){}
      *LED_OFF_REGISTER = RGBY;
    }
  }
*/
  while (1) {
    if ((*BUTT & 0x1) == 1) break;
    *LED_ON_REGISTER = Y;
    delay();
    *LED_OFF_REGISTER = Y;
    
    if ((*BUTT & 0x1) == 1) break;
    *LED_ON_REGISTER = R;
    delay();
    *LED_OFF_REGISTER = R;

    if ((*BUTT & 0x1) == 1) break;
    *LED_ON_REGISTER = B;
    delay();
    *LED_OFF_REGISTER = B;

    if ((*BUTT & 0x1) == 1) break;
    *LED_ON_REGISTER = G;
    delay();
    *LED_OFF_REGISTER = G;
  }
}