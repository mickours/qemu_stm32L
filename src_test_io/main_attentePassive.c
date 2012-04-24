/*----------------------------------------------------------------------------
 * Name:    Exti.c
 * Purpose: EXTI usage for STM32
 * Version: V1.00
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * Copyright (c) 2005-2007 Keil Software. All rights reserved.
 *----------------------------------------------------------------------------*/
 
typedef unsigned int uint32_t;

//GPIO 
#define RCC_AHBENR 0x4002381c
#define GPIOB 0x40020400 /* port B */
#define GPIOB_MODER (GPIOB + 0x00) /* port mode register */
#define LED_PORT_ODR (GPIOB + 0x14) /* port output data register */

#define LED_BLUE (1 << 6) /* port B, pin 6 */
#define LED_GREEN (1 << 7) /* port B, pin 7 */
#define LED_ORANGE 0
#define LED_RED 0

//IRQ
/*EXTI0 -> IT 6*/
#define EXTI0 0x00000058
#define EXTI0_IMR (EXTI0+0x00)
#define EXTI0_EMR (EXTI0+0x04)
#define EXTI0_RTSR (EXTI0+0x08)
#define EXTI0_FTSR (EXTI0+0x0C)
#define EXTI0_PR (EXTI0+0x14)

#define EXTI0_IMR_RSTVAL 0x00000000
#define EXTI0_EMR_RSTVAL 0x00000000
#define EXTI0_RTSR_RSTVAL 0x00000000
#define EXTI0_FTSR_RSTVAL 0x00000000
#define EXTI0_PR_RSTVAL 0x00000000




/*----------------------------------------------------------------------------
  Initialise les LEDs et le RCC
 *----------------------------------------------------------------------------*/
static inline void setup_leds()
{
  *(volatile uint32_t*)RCC_AHBENR |= (1 << 1); /*Activation de l'horloge du port B*/
  /* configure port 6 and 7 as output */
  *(volatile uint32_t*)GPIOB_MODER |= (1 << (7 * 2)) | (1 << (6 * 2));
}

static inline void switch_leds_on(void)
{
  *(volatile uint32_t*)LED_PORT_ODR = LED_BLUE | LED_GREEN;
}

static inline void switch_leds_off(void)
{
  *(volatile uint32_t*)LED_PORT_ODR = 0;
}



/*----------------------------------------------------------------------------
  EXTI0 Interrupt Handler      for WKUP button connected to GPIOA.0
 *----------------------------------------------------------------------------*/
unsigned int ledExti = 0;
static inline void EXTI0_IRQHandler(void)
{
  if (EXTI0_PR & (1<<0)) {                        // EXTI13 interrupt pending?
    *(volatile uint32_t*)EXTI0_PR |= (1<<0);                           // clear pending interrupt

    if ((ledExti ^=1) == 0) 
    	switch_leds_off();
    else
	    switch_leds_on();
  }
}

/*----------------------------------------------------------------------------
  Initialise l'entrée EXTI0
 *----------------------------------------------------------------------------*/
void EXTI0_init() {
    //RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
    //AFIO->EXTICR[0] &= 0xFFF0;                              // clear used pin
    //AFIO->EXTICR[0] |= (0x000F & __AFIO_EXTICR1);           // set pin to use

    *(volatile uint32_t*)EXTI0_IMR		|= ((1 << 0) & EXTI0_IMR_RSTVAL);       // unmask interrupt
    *(volatile uint32_t*)EXTI0_EMR       |= ((1 << 0) & EXTI0_EMR_RSTVAL);       // unmask event
    *(volatile uint32_t*)EXTI0_RTSR      |= ((1 << 0) & EXTI0_RTSR_RSTVAL);      // set rising edge
    //EXTI0_FTSR      |= ((1 << 0) & EXTI0_FTSR_RSTVAL);      // set falling edge

    //if (__EXTI_INTERRUPTS & (1 << 0)) {                     // interrupt used
    //  NVIC->ISER[0] |= (1 << (EXTI0_IRQChannel & 0x1F));    // enable interrupt EXTI 0
    //}
}



/*----------------------------------------------------------------------------
  MAIN function
  User button -> PA0
 *----------------------------------------------------------------------------*/
int main (void) {
  while (1) {                                     // Loop forever
    ;
  } // end while
} // end main

void SystemInit(void){
	setup_leds();
	EXTI0_init();
}
